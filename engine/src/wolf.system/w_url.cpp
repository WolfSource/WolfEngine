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
			w_url_pimp() :
				_curl(nullptr)
			{
				this->_curl = curl_easy_init();
				if (!this->_curl)
				{
					logger.error("could not initialize curl");
					return;
				}
			}

			W_RESULT request_url(_In_z_ const char* pURL, _Inout_ std::string& pResultPage)
			{
				if (!this->_curl) return W_FAILED;
				
				//reset memory
				_chunk.reset();

				curl_easy_setopt(_curl, CURLOPT_URL, pURL);
				curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1L);
				curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, _write_callback);
				curl_easy_setopt(_curl, CURLOPT_WRITEDATA, (void*)(&this->_chunk));
				//some servers don't like requests that are made without a user-agent field, so we provide one
				curl_easy_setopt(_curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
				
				//perform the request
				auto _result = curl_easy_perform(_curl);

				//check for errors
				if (_result != CURLE_OK)
				{
					logger.error(
						"could not get result of requested url : {} because {}", 
						pURL, curl_easy_strerror(_result));
					return W_FAILED;
				}
				
				_chunk.copyto(pResultPage);
				_chunk.reset();

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

				//reset memory
				_chunk.reset();

				//set POST url
				curl_easy_setopt(_curl, CURLOPT_URL, pURL);
				//now specify the POST data
				curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, pMessage);
				curl_easy_setopt(_curl, CURLOPT_POSTFIELDSIZE, pMessageLenght);
				curl_easy_setopt(_curl, CURLOPT_POST, 1L);
				curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1L);
				curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, _write_callback);
				curl_easy_setopt(_curl, CURLOPT_WRITEDATA, (void*)(&this->_chunk));
				
				//set http header
				//for example "Accept: application/json";
				struct curl_slist* _headers = NULL;
				for (auto _header : pHeaders)
				{
					_headers = curl_slist_append(_headers, _header.c_str());
				}
				curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, _headers);
				//perform the request
				auto _result = curl_easy_perform(_curl);

				//free chuck
				curl_slist_free_all(_headers);

				//check for errors
				if (_result != CURLE_OK)
				{
					logger.error(
						"could not get result of requested rest post : {} / {} because {}",
						pURL, pMessage,  curl_easy_strerror(_result));
					return W_FAILED;
				}

				_chunk.copyto(pResult);
				_chunk.reset();

				return W_PASSED;
			}

			void release()
			{
				if (this->_curl)
				{
					curl_easy_cleanup(this->_curl);
					free(this->_chunk.memory);
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
				size_t _real_size = pSize * pNmemb;
				struct url_memory* _mem = (struct url_memory*)pUserp;

				auto _ptr = (char*)realloc(_mem->memory, _mem->size + _real_size + 1);
				if (_ptr == NULL)
				{
					//out of memory
					wolf::logger.error("could not allocate memory for result of url");
					return 0;
				}

				_mem->memory = _ptr;
				memcpy(&(_mem->memory[_mem->size]), pContents, _real_size);
				_mem->size += _real_size;
				_mem->memory[_mem->size] = 0;

				return _real_size;
			}

			CURL*									_curl;
			struct url_memory
			{
				char*	memory = nullptr;
				size_t	size = 0;

				void reset()
				{
					this->size = 0;
					if (this->memory)
					{
						free(this->memory);
					}
					this->memory = (char*)malloc(1);//will be grown as needed by the realloc above
				}

				void copyto(_Inout_ std::string& pDestination)
				{
					if (this->size && this->memory)
					{
						pDestination.resize(this->size);
						strcpy(&pDestination[0], this->memory);
					}
				}

			} _chunk;
		};
	}
}

using namespace wolf::system;

static std::once_flag _once_init;
static std::once_flag _once_release;

w_url::w_url() : _pimp(new w_url_pimp())
{
	_super::set_class_name("w_url");
	std::call_once(_once_init, [&]()
	{
		curl_global_init(CURL_GLOBAL_ALL);
	});
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
