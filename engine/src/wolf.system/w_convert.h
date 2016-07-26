/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_convert.h
	Description		 : Helper functions for converting types to each others
	Comment			 :
*/

#ifndef __W_CONVERT_H__
#define __W_CONVERT_H__

#include <string.h>
#include <iostream>
#include <AtlConv.h>

namespace wolf
{
	namespace system
	{
		namespace convert
		{
			inline std::wstring chars_to_wstring(CHAR* value)
			{
				std::string str = value;
				return std::wstring(str.begin(), str.end());
			}

			inline std::wstring string_to_wstring(std::string value)
			{
				return std::wstring(value.begin(), value.end());
			}

			inline std::string wstring_to_string(std::wstring value)
			{
				return std::string(value.begin(), value.end());
			}

			inline std::wstring tchars_to_wstring(TCHAR* pValue, const UINT pLength)
			{
				auto wct = new wchar_t[pLength];
				for (size_t i = 0; i < pLength; ++i)
				{
					wct[i] = pValue[i];
				}
				return std::wstring(&wct[0]);
			}

			inline std::string tchars_to_string(TCHAR* pValue, const UINT pLength)
			{
				auto wstr = tchars_to_wstring(pValue, pLength);
				return wstring_to_string(wstr);
			}

			inline HRESULT chars_to_GUID(std::wstring value, GUID& GUID)
			{
				LPOLESTR guid = W2OLE((wchar_t*)value.c_str());
				auto hr = S_OK;
				CLSIDFromString(guid, (LPCLSID)&GUID);
				return hr;
			}

#pragma region sub string and convert functions
			
			template<class T >
			auto substr_function(const std::string& pStr, size_t pOffset, size_t pCount, _Inout_ std::vector<T>& pResult) -> typename std::enable_if<std::is_integral<T>::value, void>::type
			{
				if (std::is_same<T, int>::value || std::is_same<T, unsigned int>::value ||
					std::is_same<T, short>::value || std::is_same<T, unsigned short>::value)
				{
					pResult.push_back(std::atoi(pStr.substr(pOffset, pCount).c_str()));
				}
				else if (std::is_same<T, long>::value || std::is_same < T, unsigned long> ::value)
				{
					pResult.push_back(std::atol(pStr.substr(pOffset, pCount).c_str()));
				}
				else if (std::is_same<T, long long>::value || std::is_same < T, unsigned long long> ::value)
				{
					pResult.push_back(std::atoll(pStr.substr(pOffset, pCount).c_str()));
				}
				return;
			}

			template<class T >
			auto substr_function(const std::string& pStr, size_t pOffset, size_t pCount, _Inout_ std::vector<T>& pResult) -> typename std::enable_if<std::is_floating_point<T>::value, void>::type
			{
				pResult.push_back(std::atof(pStr.substr(pOffset, pCount).c_str()));
				return;
			}

			template<class T >
			auto substr_function(const std::string& pStr, size_t pOffset, size_t pCount, _Inout_ std::vector<T>& pResult) -> typename std::enable_if<std::is_same<T, std::string>::value, void>::type
			{
				pResult.push_back(pStr.substr(pOffset, pCount));
				return;
			}

#pragma endregion

#pragma region sub wstring convert functions

			template<class T >
			auto subwstr_function(const std::wstring& pStr, size_t pOffset, size_t pCount, _Inout_ std::vector<T>& pResult) -> typename std::enable_if<std::is_integral<T>::value, void>::type
			{
				if (std::is_same<T, int>::value)
				{
					pResult.push_back(std::atoi(pStr.substr(pOffset, pCount).c_str()));
				}
				else if (std::is_same<T, long>::value)
				{
					pResult.push_back(std::atol(pStr.substr(pOffset, pCount).c_str()));
				}
				else if (std::is_same<T, long long>::value)
				{
					pResult.push_back(std::atoll(pStr.substr(pOffset, pCount).c_str()));
				}
				return;
			}

			template<class T >
			auto subwstr_function(const std::wstring& pStr, size_t pOffset, size_t pCount, _Inout_ std::vector<T>& pResult) -> typename std::enable_if<std::is_floating_point<T>::value, void>::type
			{
				pResult.push_back(std::atof(pStr.substr(pOffset, pCount).c_str()));
				return;
			}

			template<class T >
			auto subwstr_function(const std::wstring& pStr, size_t pOffset, size_t pCount, _Inout_ std::vector<T>& pResult) -> typename std::enable_if<std::is_same<T, std::wstring>::value, void>::type
			{
				pResult.push_back(pStr.substr(pOffset, pCount));
				return;
			}

#pragma endregion

			template<class T>
			inline void split_string_then_convert_to(const std::string& pStr, const std::string& pSplit, _Inout_ std::vector<T>& pResult)
			{
				using namespace std;

				size_t _last = 0, _next = 0;
				const auto _size = pSplit.size();
				while ((_next = pStr.find(pSplit, _last)) != string::npos)
				{
					substr_function(pStr, _last, _next - _last, pResult);
					_last = _next + _size;
				}

				//still we have string, add the finals
				auto _str_size = pStr.size();
				if (_last < _str_size)
				{
					substr_function(pStr, _last, _next - _last, pResult);
				}
			}

			template<class T>
			inline void split_wstring_then_convert_to(const std::wstring& pStr, const std::wstring& pSplit, _Inout_ std::vector<T>& pResult)
			{
				using namespace std;

				size_t _last = 0, _next = 0;
				const auto _size = pSplit.size();
				while ((_next = pStr.find(pSplit, _last)) != wstring::npos)
				{
					subwstr_function(pStr, _last, _next - _last, pResult);
					_last = _next + _size;
				}

				//still we have string, add the finals
				auto _str_size = pStr.size();
				if (_last < _str_size)
				{
					subwstr_function(pStr, _last, _next - _last, pResult);
				}
			}

			inline void split_string(const std::string& pStr, const std::string& pSplit, _Inout_ std::vector<std::string>& pResult)
			{
				split_string_then_convert_to<std::string>(pStr, pSplit, pResult);
			}

			inline void split_wstring(const std::wstring& pStr, const std::wstring& pSplit, _Inout_ std::vector<std::wstring>& pResult)
			{
				split_wstring_then_convert_to<std::wstring>(pStr, pSplit, pResult);
			}
		}
	}
}

#endif