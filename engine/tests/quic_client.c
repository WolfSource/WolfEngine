#include <wolf.h>
#include <net/w_net.h>

#define MAX_BUFFER_SIZE 65535
w_mem_pool s_mem_pool = NULL;

void s_quic_debug_log_callback(const char* pLine, void* pArgp)
{
    printf("%s", pLine);
}

static bool _req_sent = false;

W_RESULT s_quic_sending_stream_callback(
    uint8_t* pConnectionID,
    uint64_t pStreamIndex,
    const uint8_t* pProtocol,
    const size_t pProtocolLen)
{
    if (!_req_sent)
    {
        static w_buffer_t _b;
        _b.data = (uint8_t*)("GET / index.html\0");
        _b.len = strlen((const char*)_b.data);

        size_t _len = w_net_send_msg_quic(
            pConnectionID,
            pStreamIndex,
            &_b,
            false);
        _req_sent = false;

        printf("\np is %p", pProtocol);
    }

    return W_SUCCESS;
}

w_buffer_t _buffer;
W_RESULT s_quic_receiving_stream_callback(
    uint8_t* pConnectionID,
    uint64_t pStreamIndex,
    const uint8_t* pProtocol,
    const size_t pProtocolLen)
{
    bool _fin = false;
    
    _buffer.len = MAX_BUFFER_SIZE;
    if (!_buffer.data)
    {
        _buffer.data = (uint8_t*)w_malloc(s_mem_pool, sizeof(uint8_t));
    }

    size_t _recv_len = w_net_receive_msg_quic(
        pConnectionID,
        pStreamIndex,
        &_buffer,
        &_fin);
    if (_recv_len < 0)
    {
        return W_FAILURE;
    }

    //logger printf("%.*s", (int)recv_len, buf);
    W_RESULT _ret = W_SUCCESS;
    if (_fin)
    {
        _ret = w_net_close_quic_socket();
    }
    return _ret;
}

int main()
{
    wolf_init();

    w_mem_pool_init(&s_mem_pool);

    w_net_open_quic_socket(
        "localhost",
        5555,
        quic_dialer,
        NULL,
        NULL,
        s_quic_debug_log_callback,
        s_quic_receiving_stream_callback,
        s_quic_sending_stream_callback);

    w_mem_pool_fini(&s_mem_pool);
    wolf_fini();

    return 0;
}
