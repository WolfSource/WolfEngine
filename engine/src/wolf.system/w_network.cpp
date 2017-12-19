#include "w_system_pch.h"
#include "w_network.h"
#include <nanomsg/nn.h>
#include <nanomsg/pipeline.h>

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

            HRESULT setup_one_way_pusher(
                const char* pURL, 
                wolf::system::w_signal<void(const int& pSocketID)> pOnConnectionEstablished)
            {
                const std::string _trace_info = this->_name + "::setup_one_way_pusher";

                this->_socket = nn_socket(AF_SP, NN_PUSH);
                if (this->_socket < 0)
                {
                    V(S_FALSE, "creating socket", _trace_info, 3);
                    return S_FALSE;
                }
                if (nn_connect(this->_socket, pURL) < 0)
                {
                    V(S_FALSE, "connecting to socket", _trace_info, 3);
                    return S_FALSE;
                }
                
                return S_OK;
            }

            HRESULT setup_one_way_puller()
            {

                return S_OK;
            }

            void release()
            {

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
    w_signal<void(const int& pSocketID)> pOnConnectionEstablishedCallback)
{
    if (!this->_pimp) return S_FALSE;
    return this->_pimp->setup_one_way_pusher(pURL, pOnConnectionEstablishedCallback);
}

HRESULT w_network::setup_one_way_puller()
{
    if (!this->_pimp) return S_FALSE;
    return this->_pimp->setup_one_way_puller();
}

ULONG w_network::release()
{
    if (!this->_pimp || _super::get_is_released()) return 1;
    
    SAFE_RELEASE(this->_pimp);

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