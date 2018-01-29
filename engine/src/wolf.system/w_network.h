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
#include <initializer_list>

namespace wolf
{
	namespace system
	{
		enum w_network_error
		{
			W_EPERM				= 1,
			W_ENOENT			= 2,
			W_ESRCH				= 3,
			W_EINTR				= 4,
			W_EIO				= 5,
			W_ENXIO				= 6,
			W_E2BIG				= 7,
			W_ENOEXEC			= 8,
			W_EBADF				= 9,
			W_ECHILD			= 10,
			W_EAGAIN			= 11,
			W_ENOMEM			= 12,
			W_EACCES			= 13,
			W_EFAULT			= 14,
			W_EBUSY				= 16,
			W_EEXIST			= 17,
			W_EXDEV				= 18,
			W_ENODEV			= 19,
			W_ENOTDIR			= 20,
			W_EISDIR			= 21,
			W_EINVAL			= 22,
			W_ENFILE			= 23,
			W_EMFILE			= 24,
			W_ENOTTY			= 25,
			W_EFBIG				= 27,
			W_ENOSPC			= 28,
			W_ESPIPE			= 29,
			W_EROFS				= 30,
			W_EMLINK			= 31,
			W_EPIPE				= 32,
			W_EDOM				= 33,
			W_ERANGE			= 34,
			W_EDEADLK			= 36,
			W_ENAMETOOLONG		= 38,
			W_ENOLCK			= 39,
			W_ENOSYS			= 40,
			W_ENOTEMPTY			= 41,
			W_EILSEQ			= 42,
			W_STRUNCATE			= 80,
			W_EADDRINUSE		= 100,
			W_EADDRNOTAVAIL		= 101,
			W_EAFNOSUPPORT		= 102,
			W_EALREADY			= 103,
			W_EBADMSG			= 104,
			W_ECANCELED			= 105,
			W_ECONNABORTED		= 106,
			W_ECONNREFUSED		= 107,
			W_ECONNRESET		= 108,
			W_EDESTADDRREQ		= 109,
			W_EHOSTUNREACH		= 110,
			W_EIDRM				= 111,
			W_EINPROGRESS		= 112,
			W_EISCONN			= 113,
			W_ELOOP				= 114,
			W_EMSGSIZE			= 115,
			W_ENETDOWN			= 116,
			W_ENETRESET			= 117,
			W_ENETUNREACH		= 118,
			W_ENOBUFS			= 119,
			W_ENODATA			= 120,
			W_ENOLINK			= 121,
			W_ENOMSG			= 122,
			W_ENOPROTOOPT		= 123,
			W_ENOSR				= 124,
			W_ENOSTR			= 125,
			W_ENOTCONN			= 126,
			W_ENOTRECOVERABLE	= 127,
			W_ENOTSOCK			= 128,
			W_ENOTSUP			= 129,
			W_EOPNOTSUPP		= 130,
			W_EOTHER			= 131,
			W_EOVERFLOW			= 132,
			W_EOWNERDEAD		= 133,
			W_EPROTO			= 134,
			W_EPROTONOSUPPORT	= 135,
			W_EPROTOTYPE		= 136,
			W_ETIME				= 137,
			W_ETIMEDOUT			= 138,
			W_ETXTBSY			= 139,
			W_EWOULDBLOCK		= 140
		};

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
                _In_ int pReceiveTime,
                _In_ w_signal<void(const int& pSocketID)> pOnBindEstablishedCallback);

            WSYS_EXP HRESULT setup_two_way_client(
                _In_z_ const char* pURL,
                _In_ int pReceiveTime,
                _In_ w_signal<void(const int& pSocketID)> pOnConnectionEstablishedCallback);
            
            WSYS_EXP HRESULT setup_broadcast_publisher(
                _In_z_ const char* pURL,
                _In_ w_signal<void(const int& pSocketID)> pOnBindEstablishedCallback);
            
            WSYS_EXP HRESULT setup_broadcast_subscriptore(
                _In_z_ const char* pURL,
                _In_ w_signal<void(const int& pSocketID)> pOnConnectionEstablishedCallback);
            
			WSYS_EXP HRESULT setup_survey_server(
				_In_z_ const char* pURL,
				_In_ w_signal<void(const int& pSocketID)> pOnBindEstablishedCallback);

			WSYS_EXP HRESULT setup_survey_client(
				_In_z_ const char* pURL,
				_In_ w_signal<void(const int& pSocketID)> pOnConnectionEstablishedCallback);

			WSYS_EXP HRESULT setup_bus_node(
				_In_z_ const char* pBindURL,
				_In_ int pReceiveTime,
				_In_ w_signal<void(const int& pSocketID)> pOnBindEstablishedCallback,
				_In_ std::initializer_list<const char*> pConnectURLs);


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

			//free message buffer
			WSYS_EXP static ULONG free_buffer(_In_z_ char* pBuffer);

			//get last error
			WSYS_EXP static w_network_error get_last_error();

		private:
            typedef system::w_object            _super;
            w_network_pimp*                     _pimp;
		};
	}
}

#endif //__W_NETWORK_H__
