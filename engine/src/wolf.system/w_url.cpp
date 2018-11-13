#include "w_system_pch.h"
#include "w_url.h"
#include "curl/curl.h"

namespace wolf
{
	namespace system
	{
		class w_url_pimp
		{
		public:
			w_url_pimp()
			{
				this->_curl = curl_easy_init();
				if (!this->_curl)
				{
					logger.error("could not initialize curl");
				}
			}

			W_RESULT request_url(_In_z_ const char* pURL, _Inout_ std::string& pResultPage)
			{
				if (!this->_curl) return W_FAILED;
				
				curl_easy_setopt(_curl, CURLOPT_URL, pURL);
				curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1L);
				curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, _write_callback);
				curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &pResultPage);
				//perform the request
				auto _result = curl_easy_perform(_curl);

				//check for errors
				if (_result != CURLE_OK)
				{
					logger.error(
						"could not get result of requested url : {}", pURL);
					return W_FAILED;
				}
				return W_PASSED;
			}

			W_RESULT send_rest_post(
				_In_z_ const char* pURL, 
				_In_z_ const char* pMessage, 
				_In_ const size_t& pMessageLenght, 
				_Inout_ std::string& pResult,
				_In_z_ std::initializer_list<std::string> pHeaders)
			{
				if (!_curl) return W_FAILED;

				//set POST url
				curl_easy_setopt(_curl, CURLOPT_URL, pURL);
				//now specify the POST data
				curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, pMessage);
				curl_easy_setopt(_curl, CURLOPT_POSTFIELDSIZE, pMessageLenght);
				curl_easy_setopt(_curl, CURLOPT_POST, 1L);
				curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1L);
				curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, _write_callback);
				curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &pResult);

				//set http header
				//for example "Accept: application/json";
				struct curl_slist* _chunk = NULL;
				for (auto _header : pHeaders)
				{
					_chunk = curl_slist_append(_chunk, _header.c_str());
				}
				curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, _chunk);
				//perform the request
				auto _result = curl_easy_perform(_curl);

				//free chuck
				curl_slist_free_all(_chunk);

				//check for errors
				if (_result != CURLE_OK)
				{
					logger.error(
						"could not get result of requested rest post : {} / {}", pURL, pMessage);
					return W_FAILED;
				}
				return W_PASSED;
			}

			void release()
			{
				if (this->_curl)
				{
					curl_easy_cleanup(this->_curl);
					this->_curl = nullptr;
				}
			}

		private:
			static size_t _write_callback(
				void* pContents,
				size_t pSize,
				size_t pNmemb,
				void* pUserp)
			{
				((std::string*)pUserp)->append((char*)pContents, pSize * pNmemb);
				return pSize * pNmemb;
			}

			CURL*									_curl;
		};
	}
}

using namespace wolf::system;

w_url::w_url() : _pimp(new w_url_pimp())
{
	_super::set_class_name("w_url");
}

w_url::~w_url()
{
	release();
}

W_RESULT w_url::request_url(_In_z_ const char* pURL, _Inout_ std::string& pResultPage)
{
	return this->_pimp ? this->_pimp->request_url(pURL, pResultPage) : W_FAILED;
}

W_RESULT w_url::send_rest_post(
	_In_z_ const char* pURL,
	_In_z_ const char* pMessage,
	_In_ const size_t& pMessageLenght,
	_Inout_ std::string& pResult,
	_In_z_ std::initializer_list<std::string> pHeaders)
{
	return this->_pimp ? this->_pimp->send_rest_post(
		pURL,
		pMessage,
		pMessageLenght,
		pResult,
		pHeaders) : W_FAILED;
}

ULONG w_url::release()
{
	if (this->get_is_released()) return 1;

	SAFE_RELEASE(this->_pimp);

	return _super::release();
}
