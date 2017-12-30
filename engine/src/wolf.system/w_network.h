/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_network.h
	Description		 : Network manager
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_NETWORK_H__
#define __W_NETWORK_H__

#include "w_object.h"
#include "w_signal.h"

#ifdef __GNUC__
#pragma GCC visibility push(default)
#endif

namespace wolf
{
	namespace system
	{
        class w_network_pimp;
		class w_network : w_object
		{
		public:

            WSYS_EXP w_network();
            WSYS_EXP ~w_network();

            
            WSYS_EXP HRESULT setup_one_way_pusher(
                _In_z_ const char* pURL, 
                _In_ w_signal<void(const int& pSocketID)> pOnConnectionEstablishedCallback);

            WSYS_EXP HRESULT setup_one_way_puller(
				_In_z_ const char* pURL,
				_In_ w_signal<void(const int& pSocketID)> pOnBindEstablishedCallback);

            WSYS_EXP HRESULT setup_two_way_server(
                _In_z_ const char* pURL,
                _In_ const int& pReceiveTime,
                _In_ w_signal<void(const int& pSocketID)> pOnBindEstablishedCallback);

            WSYS_EXP HRESULT setup_two_way_client(
                _In_z_ const char* pURL,
                _In_ const int& pReceiveTime,
                _In_ w_signal<void(const int& pSocketID)> pOnConnectionEstablishedCallback);
            
            WSYS_EXP HRESULT setup_broadcast_publisher_server(
                _In_z_ const char* pURL,
                _In_ w_signal<void(const int& pSocketID)> pOnBindEstablishedCallback);
            
            WSYS_EXP HRESULT setup_broadcast_subscription_client(
                _In_z_ const char* pURL,
                _In_ w_signal<void(const int& pSocketID)> pOnConnectionEstablishedCallback);
            
            WSYS_EXP ULONG release();

            /*
                send message via socket
                @param pSocketID, the id of socket object
                @param pMessage, the message you want to send
                @return, number of bytes sent
            */
            WSYS_EXP static int send(_In_ const int& pSocketID, _In_z_ const char* pMessage, _In_ const size_t& pMessageSize);

            /*
                receive buffer via socket
                @param pSocketID, the id of socket object
                @param pBuffer, the buffer which will be received 
                @return, number of bytes received 
            */
            WSYS_EXP static int receive(_In_ const int& pSocketID, _Inout_z_ char** pBuffer);

		private:
            typedef system::w_object            _super;
            w_network_pimp*                     _pimp;
		};
	}
}


#ifdef __GNUC__
#pragma GCC visibility pop
#endif

#endif //__W_NETWORK_H__
