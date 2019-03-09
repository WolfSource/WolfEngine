/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_url.h
	Description		 : url based on curl Copyright (C) 1998 - 2018, Daniel Stenberg, <daniel@haxx.se>, et al. https://curl.haxx.se/docs/copyright.html
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_URL_H__
#define __W_URL_H__

#include "w_system_export.h"
#include "w_object.h"
#include <string>
#include <initializer_list>
#include "w_point.h"

namespace wolf
{
	namespace system
	{
		class w_url_pimp;
		class w_url : public system::w_object
		{
		public:
			WSYS_EXP	w_url();
			WSYS_EXP	~w_url();

			WSYS_EXP	W_RESULT request_url(_In_z_ const std::string& pURL, 
				_Inout_ std::string& pResultPage,
				_In_ w_point& pAbortIfSlowerThanNumberOfBytesInSeconds,
				_In_ const uint32_t& pConnectionTimeOutInMilliSeconds = 5000);

			WSYS_EXP const std::string encoded_URL(_In_z_ const std::string& pURL);
			
			WSYS_EXP	W_RESULT send_rest_post(
				_In_z_ const std::string& pURL,
				_In_z_ const std::string& pMessage,
				_In_ const size_t& pMessageLenght,
				_Inout_ std::string& pResult,
				_In_ w_point& pAbortIfSlowerThanNumberOfBytesInSeconds,
				_In_ const uint32_t& pConnectionTimeOutInMilliSeconds = 5000,
				_In_z_ std::initializer_list<std::string> pHeaders = { "content-type:application/x-www-form-urlencoded" });

			WSYS_EXP	ULONG release();

		private:
			typedef		system::w_object        _super;
			w_url_pimp*                         _pimp;
		};
	}
}

#endif //__W_XML_H__
