#include "w_system_pch.h"
#include "w_network.h"

#include <nanomsg/nn.h>
#include <nanomsg/pipeline.h>
#include <nanomsg/pair.h>
#include <nanomsg/pubsub.h>

namespace wolf
{
    namespace system
    {
        class w_network_pimp
        {
        public:
            w_network_pimp() :
                _name("w_network")
            {

            }

            HRESULT initialize(
				_In_z_ const char* pURL,
                _In_ const int& pDomain,
                _In_ const int& pProtocol,
                _In_ const bool& pConnectOrBind,
				_In_ w_signal<void(const int& pSocketID)> pOnConnectOrBindEstablished)
            {
				const std::string _trace_info = this->_name + "::setup_one_way_puller";

				this->_socket = nn_socket(pDomain, pProtocol);
				if (this->_socket < 0)
				{
					V(S_FALSE, "creating socket", _trace_info, 3);
					return S_FALSE;
				}
                
                if (pConnectOrBind)
                {
                    //on conneect, which used for client
                    if (nn_connect(this->_socket, pURL) < 0)
                    {
                        V(S_FALSE, "connecting", _trace_info, 3);
                        return S_FALSE;
                    }
                }
                else
                {
                    //on bind, which used for server
                    if (nn_bind(this->_socket, pURL) < 0)
                    {
                        V(S_FALSE, "binding", _trace_info, 3);
                        return S_FALSE;
                    }
                }

				//rise on connect or bind
				pOnConnectOrBindEstablished(this->_socket);
                return S_OK;
            }
            
            //http://nanomsg.org/v0.1/nn_setsockopt.3.html
            HRESULT set_socket_option(_In_ const int pSocketLevel, const int& pOption,
                    const int& pOptionValue, const int& pOptionValueLength)
            {
                const std::string _trace_info = this->_name + "::set_socket_option";
                if(nn_setsockopt(this->_socket, pSocketLevel, pOption, &pOptionValue, pOptionValueLength) < 0)
                {
                    V(S_FALSE, "setting socket option. Level: " + std::to_string(pSocketLevel) +
                      " Option:" + std::to_string(pOption) + " Value: " + std::to_string(pOptionValue), _trace_info, 3);
                    return S_FALSE;
                }
                return S_OK;
            }
            
            int release()
            {
				return nn_shutdown(this->_socket, 0);
            }

        private:
            std::string     _name;
            int             _socket;
        };
    }
}

using namespace wolf::system;

w_network::w_network() : _pimp(new w_network_pimp())
{
}

w_network::~w_network()
{
    release();
}

HRESULT w_network::setup_one_way_pusher(
    _In_z_ const char* pURL,
	_In_ w_signal<void(const int& pSocketID)> pOnConnectionEstablishedCallback)
{
    if (!this->_pimp) return S_FALSE;
    return this->_pimp->initialize(pURL, AF_SP, NN_PUSH, true, pOnConnectionEstablishedCallback);
}

HRESULT w_network::setup_one_way_puller(
	_In_z_ const char* pURL,
	_In_ w_signal<void(const int& pSocketID)> pOnBindEstablishedCallback)
{
    if (!this->_pimp) return S_FALSE;
    return this->_pimp->initialize(pURL, AF_SP, NN_PULL, false, pOnBindEstablishedCallback);
}

HRESULT w_network::setup_two_way_server(
    _In_z_ const char* pURL,
    _In_ const int& pReceiveTime,
    _In_ w_signal<void(const int& pSocketID)> pOnBindEstablishedCallback)
{
    if (!this->_pimp) return S_FALSE;
    if(this->_pimp->initialize(pURL, AF_SP, NN_PAIR, false, pOnBindEstablishedCallback) == S_OK)
    {
        return this->_pimp->set_socket_option(NN_SOL_SOCKET, NN_RCVTIMEO, pReceiveTime, sizeof(pReceiveTime));
    }
    return S_FALSE;
}

HRESULT w_network::setup_two_way_client(
    _In_z_ const char* pURL,
    _In_ const int& pReceiveTime,
    _In_ w_signal<void(const int& pSocketID)> pOnConnectionEstablishedCallback)
{
    if (!this->_pimp) return S_FALSE;
    if(this->_pimp->initialize(pURL, AF_SP, NN_PAIR, true, pOnConnectionEstablishedCallback) == S_OK)
    {
        return this->_pimp->set_socket_option(NN_SOL_SOCKET, NN_RCVTIMEO, pReceiveTime, sizeof(pReceiveTime));
    }
    return S_FALSE;
}

WSYS_EXP HRESULT w_network::setup_broadcast_publisher_server(
    _In_z_ const char* pURL,
    _In_ w_signal<void(const int& pSocketID)> pOnBindEstablishedCallback)
{
    if (!this->_pimp) return S_FALSE;
    return this->_pimp->initialize(pURL, AF_SP, NN_PUB, false, pOnBindEstablishedCallback);
}

WSYS_EXP HRESULT w_network::setup_broadcast_subscription_client(
    _In_z_ const char* pURL,
    _In_ w_signal<void(const int& pSocketID)> pOnConnectionEstablishedCallback)
{
    if (!this->_pimp) return S_FALSE;
    if(this->_pimp->initialize(pURL, AF_SP, NN_SUB, true, pOnConnectionEstablishedCallback) == S_OK)
    {
        return this->_pimp->set_socket_option(NN_SUB, NN_SUB_SUBSCRIBE, 0, 0);
    }
    return S_FALSE;
}

ULONG w_network::release()
{
    if (!this->_pimp || _super::get_is_released()) return 1;
    
	if (this->_pimp->release() < 0) return 1;

    return _super::release();
}

int w_network::send(_In_ const int& pSocketID, _In_z_ const char* pMessage, _In_ const size_t& pMessageSize)
{
    return nn_send(pSocketID, pMessage, pMessageSize, 0);
}

int w_network::receive(_In_ const int& pSocketID, _In_opt_z_ char** pBuffer)
{
    return nn_recv(pSocketID, pBuffer, NN_MSG, 0);
}
