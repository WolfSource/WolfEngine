#include "w_net.h"

#include <apr.h>
#include <apr-1/apr_general.h>

#include <nng/nng.h>
#include <core/nng_impl.h>
#include <nng/protocol/pipeline0/push.h>
#include <nng/protocol/pipeline0/pull.h>
#include <nng/protocol/pair1/pair.h>
#include <nng/protocol/reqrep0/rep.h>
#include <nng/protocol/reqrep0/req.h>
#include <nng/protocol/pubsub0/pub.h>
#include <nng/protocol/pubsub0/sub.h>
#include <nng/protocol/survey0/survey.h>
#include <nng/protocol/survey0/respond.h>
#include <nng/supplemental/util/platform.h>
#include <nng/supplemental/tls/tls.h>
#include <nng/transport/tls/tls.h>

#include "io/w_io.h"

#include <quiche.h>
#include <concurrency/libev/ev.h>
#include <hash/uthash.h>

#define     QUICHE_MAX_DATAGRAM_SIZE 1350

struct quiche_connection 
{
    int             s;
    struct conn_io* h;
};

#ifdef W_PLATFORM_WIN

#else
#include <arpa/inet.h>
#endif

#include <curl/curl.h>

const char* _net_error(_In_ W_RESULT pErrorCode,
                       _In_z_ const char* pUserDefinedMessage,
                       _In_z_ const char* pTraceInfo)
{
    const char* _error_msg = nng_strerror(pErrorCode);
    if (pErrorCode)
    {
        W_ASSERT_P(false,
                   "%s. error code: %d. error message: %s. trace info: %s.",
                   pUserDefinedMessage,
                   pErrorCode,
                   _error_msg,
                   pTraceInfo);
    }
    return _error_msg;
}

static W_RESULT _init_dialer_tls(_In_ nng_dialer pDialer,
                                 _In_z_ const char* pServerName,
                                 _In_ bool pOwnCert,
                                 _In_z_ const char* pCert,
                                 _In_z_ const char* pPrivateKey)
{
    nng_tls_config* _cfg;
    W_RESULT _rt;

    if ((_rt = nng_tls_config_alloc(&_cfg, NNG_TLS_MODE_CLIENT)) != 0)
    {
        return _rt;
    }

    if ((_rt = nng_tls_config_ca_chain(_cfg, pCert, NULL)) != 0)
    {
        goto out;
    }

    if ((_rt = nng_tls_config_server_name(_cfg, pServerName)) != 0)
    {
        goto out;
    }
    nng_tls_config_auth_mode(_cfg, NNG_TLS_AUTH_MODE_REQUIRED);

    if (pOwnCert)
    {
        if ((_rt = nng_tls_config_own_cert(_cfg, pCert, pPrivateKey, NULL)) != 0)
        {
            goto out;
        }
    }

    _rt = nng_dialer_setopt_ptr(pDialer, NNG_OPT_TLS_CONFIG, _cfg);

out:
    nng_tls_config_free(_cfg);
    return _rt;
}

static W_RESULT _init_listener_tls(_In_ nng_listener pListener,
                                   _In_ int pAuthMode,
                                   _In_z_ const char* pCert,
                                   _In_z_ const char* pPrivateKey)
{
    nng_tls_config* _cfg;
    W_RESULT _rt;

    if ((_rt = nng_tls_config_alloc(&_cfg, NNG_TLS_MODE_SERVER)) != 0)
    {
        return _rt;
    }
    if ((_rt = nng_tls_config_own_cert(_cfg, pCert, pPrivateKey, NULL)) != 0)
    {
        goto out;
    }
    if ((_rt = nng_listener_setopt_ptr(pListener, NNG_OPT_TLS_CONFIG, _cfg)) != 0)
    {
        goto out;
    }
    switch (pAuthMode)
    {
    case NNG_TLS_AUTH_MODE_REQUIRED:
    case NNG_TLS_AUTH_MODE_OPTIONAL:
        if ((_rt = nng_tls_config_ca_chain(_cfg, pCert, NULL)) != 0)
        {
            goto out;
        }
        break;
    default:
        break;
    }
    if ((_rt = nng_tls_config_auth_mode(_cfg, pAuthMode)) != 0)
    {
        goto out;
    }
out:
    nng_tls_config_free(_cfg);
    return W_SUCCESS;
}

W_RESULT w_net_init(void)
{
    return nni_init();
}

W_RESULT w_net_url_parse(_In_z_ const char* pUrlAddress, _Inout_ w_url pURL)
{
    const char* _trace_info = "w_net_url_parse";
    
    if (!pUrlAddress)
    {
        pURL = NULL;
        return NNG_ENOARG;
    }
    
    W_RESULT _rt = nng_url_parse(&pURL, pUrlAddress);
    if (_rt)
    {
        _net_error(_rt, "nng_url_parse got error", _trace_info);
        return _rt;
    }
    
    //check for NULL
    if (pURL == NULL)
    {
        W_ASSERT(false,
                 "nng_url_parse got NULL. trace info: w_net_url_parse");
        return NNG_EINVAL;
    }
    
    return _rt;
}

const char* w_net_url_encoded(_In_z_ const char* pUrlAddress)
{
    char* _encoded = NULL;
    CURL* _curl = curl_easy_init();
    if (!_curl)
    {
        W_ASSERT(false, "could not create curl handle. trace info: w_net_url_encoded");
        goto _out;
    }
    
    char* _output = curl_easy_escape(_curl, pUrlAddress, (int)strlen(pUrlAddress));
    if (_output)
    {
        //allocate memory for it from memory pool
        _encoded = w_malloc(strlen(_output), "w_net_url_encoded");
        // make a copy and free the original string
        strcpy(_encoded, _output);
        curl_free(_output);
    }
    
_out:
    if (_curl)
    {
        curl_easy_cleanup(_curl);
    }
    
    return _encoded;
}

void w_net_url_free(_Inout_ w_url pURL)
{
    nng_url_free(pURL);
}

W_RESULT w_net_resolve(_In_z_ const char* pURL,
                       _In_z_ const char* pPort,
                       _In_ w_tcp_socket_address_family pSocketAddressFamily,
                       _In_ int pBindOrConnect,
                       _Inout_ w_socket_address pSocketAddress)
{
    nng_aio*    _aio;
    W_RESULT    _rt;
    
    //allocate aio
    if((_rt = nng_aio_alloc(&_aio, NULL, NULL)) != 0)
    {
        goto out;
    }
    
    //resolve and wait for aio
    nni_tcp_resolv(pURL, pPort, pSocketAddressFamily, 1, _aio);
    nng_aio_wait(_aio);
    
    //get result
    if((_rt = nng_aio_result(_aio)) != 0)
    {
        goto out;
    }
    
    nni_aio_get_sockaddr(_aio, pSocketAddress);
    
out:
    nng_aio_free(_aio);
    return _rt;
}

W_RESULT w_net_open_tcp_socket(_In_z_ const char* pEndPoint,
                               _In_ w_socket_mode pSocketMode,
                               _In_ bool pNoDelayOption,
                               _In_ bool pKeepAliveOption,
                               _In_ bool pAsync,
                               _In_ bool pTLS,
                               _In_ int pAuthMode,
                               _In_z_ const char* pTLSServerName,
                               _In_ bool pOwnCert,
                               _In_z_ const char* pCert,
                               _In_z_ const char* pKey,
                               _Inout_ w_socket_tcp* pSocket)
{
    const char* _trace_info = "w_net_open_tcp_socket";
    
    if (!pSocket || !pEndPoint)
    {
        W_ASSERT(false, "pSocket is NULL! trace info: w_net_open_pair_tcp_socket");
        return NNG_ENOARG;
    }
    
    W_RESULT _rt = W_SUCCESS;
    
    nng_socket* _nng_socket = (nng_socket*)pSocket->s;
    nng_dialer* _nng_dialer = (nng_dialer*)pSocket->d;
    nng_listener* _nng_listener = (nng_listener*)pSocket->l;

    switch (pSocketMode)
    {
        case one_way_pusher:
            _rt = nng_push0_open(_nng_socket);
            break;
        case one_way_puller:
            _rt = nng_pull0_open(_nng_socket);
            break;
        case two_way_dialer:
        case two_way_listener:
            _rt = nng_pair_open(_nng_socket);
            break;
        case req_rep_dialer:
            _rt = nng_req0_open(_nng_socket);
            break;
        case req_rep_listener:
            _rt = nng_rep0_open(_nng_socket);
            break;
        case pub_sub_broadcaster:
            _rt = nng_pub0_open(_nng_socket);
            break;
        case pub_sub_subscriber:
            _rt = nng_sub0_open(_nng_socket);
            _rt = nng_setopt(*_nng_socket, NNG_OPT_SUB_SUBSCRIBE, "", 0);
            break;
        case survey_respond_server:
            _rt = nng_surveyor0_open(_nng_socket);
            break;
        case survey_respond_client:
            _rt = nng_respondent0_open(_nng_socket);
            break;
        case bus_node:
            break;
    }
    
    if(_rt)
    {
        _net_error(_rt, "could not open socket", _trace_info);
        goto out;
    }
    
    //set options
    _rt = nng_setopt_bool(*_nng_socket, NNG_OPT_TCP_NODELAY, pNoDelayOption);
    if(_rt)
    {
        _net_error(_rt, "could not set socket no delay option", _trace_info);
        goto out;
    }
    
    _rt = nng_setopt_bool(*_nng_socket, NNG_OPT_TCP_KEEPALIVE, pKeepAliveOption);
    if(_rt)
    {
        _net_error(_rt, "could not set socket keep alive option", _trace_info);
        goto out;
    }
    
    switch (pSocketMode)
    {
        default:
            _rt = NNG_ENOARG;
            break;
        case one_way_pusher:
        case two_way_dialer:
        case req_rep_dialer:
        case pub_sub_subscriber:
        case survey_respond_client:
        {
            //create dialer
            _rt = nng_dialer_create(_nng_dialer, *_nng_socket, pEndPoint);
            if(_rt)
            {
                _net_error(_rt, "could not create dialer object", _trace_info);
                break;
            }
        
            //set dialer options
            _rt = nng_dialer_setopt_bool(*_nng_dialer, NNG_OPT_TCP_NODELAY, pNoDelayOption);
            if(_rt)
            {
                _net_error(_rt, "could not set dialer no delay option", _trace_info);
                break;
            }
        
            _rt = nng_dialer_setopt_bool(*_nng_dialer, NNG_OPT_TCP_KEEPALIVE, pKeepAliveOption);
            if(_rt)
            {
                _net_error(_rt, "could not set dialer keep alive option", _trace_info);
                break;
            }
            
            if(pTLS)
            {
                _init_dialer_tls(*_nng_dialer,
                                 pTLSServerName,
                                 pOwnCert,
                                 pCert,
                                 pKey);
            }
            
            _rt = nng_dialer_start(*_nng_dialer, (pAsync ? NNG_FLAG_NONBLOCK : 0));
            if (_rt)
            {
                _net_error(_rt, "could not start dialer", _trace_info);
                break;
            }
        }
        break;
            
        case one_way_puller:
        case two_way_listener:
        case req_rep_listener:
        case pub_sub_broadcaster:
        case survey_respond_server:
        {
            //create listener
            _rt = nng_listener_create(_nng_listener, *_nng_socket, pEndPoint);
            if(_rt)
            {
                _net_error(_rt, "could not create listener object", _trace_info);
                break;
            }
       
            //set listener options
            _rt = nng_listener_setopt_bool(*_nng_listener, NNG_OPT_TCP_NODELAY, pNoDelayOption);
            if(_rt)
            {
                _net_error(_rt, "could not set listener no delay option", _trace_info);
                break;
            }
       
            _rt = nng_listener_setopt_bool(*_nng_listener, NNG_OPT_TCP_KEEPALIVE, pKeepAliveOption);
            if(_rt)
            {
                _net_error(_rt, "could not set listener keep alive option", _trace_info);
                break;
            }
       
            if (pTLS)
            {
                _rt = _init_listener_tls(*_nng_listener, pAuthMode, pCert, pKey);
                if (_rt)
                {
                    _net_error(_rt, "_init_listener_tls failed", _trace_info);
                    break;
                }
            }
            _rt = nng_listener_start(*_nng_listener, (pAsync ? NNG_FLAG_NONBLOCK : 0));
            if (_rt)
            {
                _net_error(_rt, "could not start listener", _trace_info);
                break;
            }
        }
        break;
    }
    
    if(_rt == 0)
    {
        return W_SUCCESS;
    }
    
out:
    if (_nng_dialer)
    {
        nng_dialer_close(*_nng_dialer);
    }
    if (_nng_listener)
    {
        nng_listener_close(*_nng_listener);
    }
    if (_nng_socket)
    {
        nng_close(*_nng_socket);
    }

    return _rt;
}

W_RESULT w_net_close_tcp_socket(_Inout_ w_socket_tcp* pSocket)
{
    if (!pSocket)
    {
        W_ASSERT(false, "pSocket is NULL! trace info: w_net_close_tcp_socket");
        return NNG_ENOARG;
    }
    
    nng_socket* _nng_socket = (nng_socket*)pSocket->s;
    nng_dialer* _nng_dialer = (nng_dialer*)pSocket->d;
    nng_listener* _nng_listener = (nng_listener*)pSocket->l;

    if (_nng_dialer)
    {
        nng_dialer_close(*_nng_dialer);
    }
    if (_nng_listener)
    {
        nng_listener_close(*_nng_listener);
    }
    if (_nng_socket)
    {
        nng_close(*_nng_socket);
    }
    
    return W_SUCCESS;
}

W_RESULT w_net_open_udp_socket(_In_z_ const char* pEndPoint, _Inout_ w_socket_udp* pSocket)
{
    W_RESULT _rt = W_SUCCESS;
    nng_url* _url = NULL;
    
    if (!pSocket)
    {
        return W_FAILURE;
    }
    nni_sockaddr* _sa = (nni_sockaddr*)pSocket->sa;
    if (!_sa)
    {
        return W_FAILURE;
    }
    nni_plat_udp* _udp_protocol = (nni_plat_udp*)pSocket->u;

    _url = (nng_url*)w_malloc(sizeof(nng_url), "");
    unsigned long _addr_hex = inet_addr(_url->u_hostname);
    int _port = atoi(_url->u_port);
    _rt = w_net_url_parse(pEndPoint, _url);
    if(_rt)
    {
        goto out;
    }
  
    if (_sa)
    {
        _sa->s_in.sa_family = NNG_AF_INET;
        _sa->s_in.sa_addr = htons(_addr_hex);
        _sa->s_in.sa_port = htons(_port);
    }

    //open udp socket
    _rt = nni_plat_udp_open(&_udp_protocol, _sa);
    if(_rt)
    {
        goto out;
    }
    
    //allocate aio
    _rt = nng_aio_alloc(&pSocket->a, NULL, NULL);
    if(_rt)
    {
        goto out;
    }
    
out:
    w_net_url_free(_url);
    return _rt;
}

void w_net_close_udp_socket(_Inout_ w_socket_udp* pSocket)
{
    if (!pSocket)
    {
        return;
    }

    nni_plat_udp* _udp_protocol = (nni_plat_udp*)pSocket->u;
    nng_aio* _aio = (nng_aio*)pSocket->a;

    if (_aio)
    {
        nng_aio_free(_aio);
    }
    if (_udp_protocol)
    {
        nni_plat_udp_close(_udp_protocol);
    }
}

W_RESULT _io_udp_socket(_In_ w_socket_mode pSocketMode,
                        _Inout_ w_socket_udp* pSocket,
                        _In_z_ char* pMessage,
                        _Inout_ size_t* pMessageLength)
{
    const char* _trace_info = "_io_udp_socket";
    
    if (!pSocket || !pMessage || !pMessageLength)
    {
        W_ASSERT(false, "parameters are NULL. trace info: _io_udp_socket");
        return NNG_ENOARG;
    }
    
    W_RESULT _rt = W_SUCCESS;
    size_t _sent_len = 0;
    const size_t _buf_len = (pSocketMode == two_way_dialer ? (*pMessageLength + 1) : *pMessageLength);
    
    nni_sockaddr* _sa = (nni_sockaddr*)pSocket->sa;
    nni_plat_udp* _udp = (nni_plat_udp*)pSocket->u;
    nng_aio* _aio = (nng_aio*)pSocket->a;
    nng_iov* _iov = (nng_iov*)pSocket->i;
    _iov->iov_buf = pMessage;
    _iov->iov_len = _buf_len;

    _rt = nng_aio_set_iov(_aio, 1, _iov);
    if (_rt)
    {
        _net_error(_rt, "nng_aio_set_iov failed", _trace_info);
        goto out;
    }
    _rt = nng_aio_set_input(_aio, 0, _sa);
    if (_rt)
    {
        _net_error(_rt, "nng_aio_set_input failed", _trace_info);
        goto out;
    }
    
    //send buffer over udp
    if (pSocketMode == two_way_dialer)
    {
        nni_plat_udp_send(_udp, _aio);
    }
    else
    {
        nni_plat_udp_recv(_udp, _aio);
    }
    
    //wait
    nng_aio_wait(_aio);

    _rt = nng_aio_result(_aio);
    if (_rt)
    {
        _net_error(_rt, "nng_aio_result failed", _trace_info);
        goto out;
    }
    
    _sent_len = nng_aio_count(_aio);
    if (_sent_len != _buf_len)
    {
        _net_error(_rt, "length of the sent buffer is not equal to inputted buffer", _trace_info);
        goto out;
    }
    
out:
    return _rt;
}

W_RESULT w_net_send_msg(_Inout_ w_socket_tcp* pSocket,
                        _In_z_ char* pMessage,
                        _In_ size_t pMessageLength,
                        _In_ bool pAsync)
{
    if (!pSocket || !pMessage || pMessageLength == 0)
    {
        W_ASSERT(false, "parameters are NULL trace info: w_net_send_msg");
        return NNG_ENOARG;
    }
    
    nng_socket* _nng_socket = (nng_socket*)pSocket->s;
    return nng_send(*_nng_socket,
                    pMessage,
                    pMessageLength + 1 /* 1 for '/0'*/,
                    pAsync ? NNG_FLAG_NONBLOCK : 0);
}

W_RESULT w_net_receive_msg(_Inout_ w_socket_tcp* pSocket,
                           _Inout_ char* pMessage,
                           _Inout_ size_t* pMessageLength)
{
    if (!pSocket)
    {
        W_ASSERT(false, "parameters are NULL. trace info: w_net_receive_msg");
        return NNG_ENOARG;
    }
    
    nng_socket* _nng_socket = (nng_socket*)pSocket->s;
    return nng_recv(*_nng_socket, &pMessage, pMessageLength, NNG_FLAG_ALLOC);
}

W_RESULT w_net_send_msg_udp(_Inout_ w_socket_udp* pSocket,
                            _In_z_ char* pMessage,
                            _In_z_ size_t pMessageLength)
{
    return _io_udp_socket(two_way_dialer, pSocket, pMessage, &pMessageLength);
}

W_RESULT w_net_receive_msg_udp(_Inout_ w_socket_udp* pSocket,
                               _In_z_ char* pMessage,
                               _In_z_ size_t* pMessageLength)
{
    return _io_udp_socket(two_way_listener, pSocket, pMessage, pMessageLength);
}

W_RESULT w_net_run_websocket_server(_In_ bool pSSL,
                                    _In_z_ const char* pCertFilePath,
                                    _In_z_ const char* pPrivateKeyFilePath,
                                    _In_z_ const char* pPassPhrase,
                                    _In_z_ const char* pRoot,
                                    _In_ int pPort,
                                    _In_ int pCompression,
                                    _In_ int pMaxPayloadLength,
                                    _In_ int pIdleTimeout,
                                    _In_ int pMaxBackPressure,
                                    _In_ ws_on_listened_fn pOnListened,
                                    _In_ ws_on_opened_fn pOnOpened,
                                    _In_ ws_on_message_fn pOnMessage,
                                    _In_ ws_on_closed_fn pOnClosed)
{
    W_RESULT _rt;
    
    ws _ws = ws_init();
    if(_ws)
    {
        _rt = ws_run(_ws,
                     pSSL,
                     pCertFilePath,
                     pPrivateKeyFilePath,
                     pPassPhrase,
                     pRoot,
                     pPort,
                     pCompression,
                     pMaxPayloadLength,
                     pIdleTimeout,
                     pMaxBackPressure,
                     pOnListened,
                     pOnOpened,
                     pOnMessage,
                     pOnClosed);
        ws_free(_ws);
    }
    else
    {
        W_ASSERT(false, "could not run websocket. trace info: w_net_run_websocket_server");
        _rt = W_FAILURE;
    }
    
    return _rt;
}

typedef struct
{
    char*    memory;
    size_t    size;
} curl_memory;

static size_t _curl_write_callback(
    void* pContents,
    size_t pSize,
    size_t pNmemb,
    void* pUserp)
{
    size_t _real_size = pSize * pNmemb;
    if (!_real_size)
    {
        W_ASSERT(false, "_real_size is zero. trace info: _curl_write_callback");
        return 0;
    }
    curl_memory* _mem = (curl_memory*)pUserp;
    _mem->memory = (char*)w_malloc(_mem->size + _real_size + 1, "_curl_write_callback");
    if (!_mem->memory)
    {
        //out of memory
        W_ASSERT(false, "out of curl_memory. trace info: _curl_write_callback");
        return 0;
    }

     memcpy(&(_mem->memory[_mem->size]), pContents, _real_size);
    _mem->size += _real_size;
    _mem->memory[_mem->size] = '\0';

    return _real_size;
}

W_RESULT w_net_send_http_request(_In_z_     const char* pURL,
                                 _In_       w_http_request_type pHttpRequestType,
                                 _In_       w_array pHttpHeaders,
                                 _In_z_     const char* pMessage,
                                 _In_       size_t pMessageLenght,
                                 _In_       size_t pLowSpeedLimit,
                                 _In_       size_t pLowSpeedTimeInSec,
                                 _In_       float pTimeOutInSecs,
                                 _Inout_    long* pResponseCode,
                                 _Inout_z_  char** pResponseMessage,
                                 _Inout_    size_t* pResponseMessageLength)
{
    CURLcode _rt;
    curl_memory* _curl_mem = NULL;
    //create handle
    CURL* _curl = curl_easy_init();
    if (!_curl)
    {
        _rt = CURLE_FAILED_INIT;
        W_ASSERT(false, "could not create curl handle. trace info: curl_easy_init");
        goto _out;
    }
    _curl_mem = w_malloc(sizeof(curl_memory), "w_net_send_http_request");
    if (!_curl_mem)
    {
        _rt = CURLE_OUT_OF_MEMORY;
        goto _out;
    }
    
    _curl_mem->memory = *pResponseMessage;
    _curl_mem->size = *pResponseMessageLength;
    
    double _timeout_in_ms = pTimeOutInSecs / 1000;
    
    //now specify the POST data
    switch (pHttpRequestType)
    {
        default:
        case HTTP_GET:
            break;
        case HTTP_POST:
            curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, pMessage);
            curl_easy_setopt(_curl, CURLOPT_POSTFIELDSIZE, pMessageLenght);
            curl_easy_setopt(_curl, CURLOPT_POST, 1L);
            break;
    }
    
    //set curl options
    curl_easy_setopt(_curl, CURLOPT_URL, pURL);
    curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(_curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(_curl, CURLOPT_CONNECTTIMEOUT_MS, _timeout_in_ms);
    curl_easy_setopt(_curl, CURLOPT_ACCEPTTIMEOUT_MS, _timeout_in_ms);
    curl_easy_setopt(_curl, CURLOPT_USERAGENT, "Mozilla/5.0");//libcurl-agent/1.0
    // abort if slower than bytes/sec
    curl_easy_setopt(_curl, CURLOPT_LOW_SPEED_LIMIT, pLowSpeedLimit);
    curl_easy_setopt(_curl, CURLOPT_LOW_SPEED_TIME, pLowSpeedTimeInSec);
    // store response message
    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, _curl_write_callback);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, (void*)_curl_mem);
    
    //set http header
    struct curl_slist* _headers = NULL;
    const char** _ppt;
    while ((_ppt = apr_array_pop(pHttpHeaders)))
    {
         _headers = curl_slist_append(_headers, *_ppt);
    }
    if (_headers)
    {
        curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, _headers);
    }
    
    // perform, then store the expected code in '_rt'
    _rt = curl_easy_perform(_curl);
    
    //free headers
    if (_headers)
    {
        curl_slist_free_all(_headers);
    }
    
    if (_rt == CURLE_OK)
    {
        curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &pResponseCode);
        //TODO: do we need to copy ?
            // _chunk.copyto(pResultPage);
        //        if (this->size && this->memory)
        //        {
        //            pDestination.resize(this->size);
        //            memcpy(&pDestination[0], &this->memory[0], this->size);
        //        }
    }
    
_out:
    w_free(_curl_mem);
    if (_curl)
    {
        curl_easy_cleanup(_curl);
    }
    return _rt;
}

W_RESULT w_net_open_quic_socket(_In_z_  const char* pAddress, 
                                _In_    int pPort,
                                _In_z_  const char* pCertFilePath,
                                _In_z_  const char* pPrivateKeyFilePath,
                                _Inout_ struct ev_loop** pEV,
                                _In_    quic_debug_log_callback pQuicDebugLogCallback,
                                _In_    quic_receive_callback pQuicReceiveCallback)
{
    const char* _trace_info = "w_net_open_quic_socket";
    
    if (!pQuicReceiveCallback)
    {
        W_ASSERT_P(false, "quic_receive_callback no set. trace info: %s", _trace_info);
        return APR_BADARG;
    }

    W_RESULT _ret = W_SUCCESS;
    quiche_config* _quiche_config = NULL;
    struct addrinfo* _address_info = NULL;

#ifdef W_PLATFORM_WIN
    SOCKET
#else
    int
#endif
    _socket = 0;

    struct quiche_connection _quiche_con = { 0 };
    const struct addrinfo _hints =
    {
        .ai_family = PF_UNSPEC,
        .ai_socktype = SOCK_DGRAM,
        .ai_protocol = IPPROTO_UDP
    };

    //convert integer port to string
    char* _port = w_malloc(6, _trace_info);//max port number is 65329
    if (!_port)
    {
        return W_FAILURE;
    }
    snprintf(_port, 6, "%d", pPort);

#ifdef W_PLATFORM_WIN
    WSADATA _wsa_data;
    WORD _version_requested = MAKEWORD(2, 2);
    int _result = WSAStartup(_version_requested, &_wsa_data);
    if (_result)
    {
        _ret = W_FAILURE;
        W_ASSERT_P(false, "WSAStartup failed. trace info: %s", _trace_info);
        goto out;
    }
    if (getaddrinfo(pAddress, _port, &_hints, &_address_info) != 0)
    {
        _ret = W_FAILURE;
        W_ASSERT_P(false, "failed to resolve host. trace info: %s", _trace_info);
        goto out;
    }
#else
    if (getaddrinfo(pAddress, _port, &_address_info, &_local) != 0)
    {
        _ret = W_FAILURE;
        W_ASSERT_P(false, "failed to resolve host. trace info: %s", _trace_info);
        goto out;
    }
#endif

    //enable quic debug logging
    if (pQuicDebugLogCallback)
    {
        quiche_enable_debug_logging(pQuicDebugLogCallback, NULL);
    }

    //create socket
    _socket = socket(_address_info->ai_family, SOCK_DGRAM, 0);
    if (
#ifdef W_PLATFORM_WIN
        _socket == 0
#else
        _socket < 0
#endif
        )
    {
        _ret = W_FAILURE;
        W_ASSERT_P(false, "failed to create a socket. trace info: %s", _trace_info);
        goto out;
    }

    //set non blocking option
#ifdef W_PLATFORM_WIN
    const u_long _non_blocking_enabled = TRUE;
    if (ioctlsocket(_socket, FIONBIO, &_non_blocking_enabled) != 0)
#else
    if (fcntl(_socket, F_SETFL, O_NONBLOCK) != 0)
#endif
    {
        _ret = W_FAILURE;
        W_ASSERT_P(false, "failed to make non-blocking socket. trace info: %s", _trace_info);
        goto out;
    }

    //bind socket to address
    if (bind(_socket, _address_info->ai_addr, _address_info->ai_addrlen) < 0)
    {
        _ret = W_FAILURE;
        W_ASSERT_P(false, "failed to connect socket. trace info: %s", _trace_info);
        goto out;
    }

    //create quic config
    _quiche_config = quiche_config_new(QUICHE_PROTOCOL_VERSION);
    if (_quiche_config == NULL) 
    {
        _ret = W_FAILURE;
        W_ASSERT_P(false, "failed to create config. trace info: %s", _trace_info);
        goto out;
    }

    //check enable ssl
    if (w_io_file_check_is_file(pCertFilePath) == W_SUCCESS &&
        w_io_file_check_is_file(pPrivateKeyFilePath) == W_SUCCESS)
    {
        _ret = quiche_config_load_cert_chain_from_pem_file(_quiche_config, pCertFilePath);
        if (_ret)
        {
            _ret = W_FAILURE;
            W_ASSERT_P(false, "failed to load cert chain from pem file for quic. trace info: %s", _trace_info);
            goto out;
        }
        _ret = quiche_config_load_priv_key_from_pem_file(_quiche_config, pPrivateKeyFilePath);
        if (_ret)
        {
            _ret = W_FAILURE;
            W_ASSERT_P(false, "failed to load private key from pem file for quic. trace info: %s", _trace_info);
            goto out;
        }
    }

    //use quic draft 27
    _ret = quiche_config_set_application_protos(_quiche_config,
        (uint8_t*)"\x05hq-29\x05hq-28\x05hq-27\x08http/0.9", 27);
    if (_ret)
    {
        _ret = W_FAILURE;
        W_ASSERT_P(false, "failed to set quic application protos. trace info: %s", _trace_info);
        goto out;
    }

    quiche_config_set_max_idle_timeout(_quiche_config, 5000);
    quiche_config_set_max_udp_payload_size(_quiche_config, QUICHE_MAX_DATAGRAM_SIZE);
    quiche_config_set_initial_max_data(_quiche_config, 10000000);
    quiche_config_set_initial_max_stream_data_bidi_local(_quiche_config, 1000000);
    quiche_config_set_initial_max_stream_data_bidi_remote(_quiche_config, 1000000);
    quiche_config_set_initial_max_streams_bidi(_quiche_config, 100);
    quiche_config_set_cc_algorithm(_quiche_config, QUICHE_CC_RENO);

#ifdef W_PLATFORM_WIN
    int _handle = _open_osfhandle(_socket, 0);
    //TODO: check hadnle result, 0 or 1 means fail?
    if (_handle)
    {
        _ret = W_FAILURE;
        W_ASSERT_P(false, "failed to open_osfhandle. trace info: %s", _trace_info);
        goto out;
    }
#endif

    _quiche_con.s = _socket;
    _quiche_con.h = NULL;
    
    ev_io _ev_watcher;
    *pEV = ev_default_loop(0);
    ev_io_init(&_ev_watcher, pQuicReceiveCallback, _socket, EV_READ);
    ev_io_start(*pEV, &_ev_watcher);
    _ev_watcher.data = &_quiche_con;//access to _quiche_con from ev_io->data
    ev_loop(*pEV, 0);

out:
    w_free(_port);
    if (_address_info)
    {
        freeaddrinfo(_address_info);
    }
    if (_socket)
    {
        closesocket(_socket);
    }
    if (_quiche_config)
    {
        quiche_config_free(_quiche_config);
    }
    return _ret;
}

W_RESULT w_net_close_quic_socket(void)
{
    return 0;
}

const char* w_net_error(_In_ W_RESULT pErrorCode)
{
    const char* _error_msg = nng_strerror(pErrorCode);
    if (_error_msg)
    {
        return _error_msg;
    }
    return curl_easy_strerror(pErrorCode);
}

void w_net_fini(void)
{
    nng_fini();
}
