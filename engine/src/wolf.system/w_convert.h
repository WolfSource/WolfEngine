/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_convert.h
	Description		 : Helper functions for converting types to each others
	Comment			 :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_CONVERT_H__
#define __W_CONVERT_H__

#include <vector>
#include <codecvt>
#include <string.h>

#if	defined(__WIN32) || defined(__UWP)

#include <AtlConv.h>
#include <string.h>
#include <sstream>

#elif defined(__ANDROID) || defined(__linux) || defined(__APPLE__)

#include "w_std.h"

#endif

namespace wolf
{
	namespace system
	{
		namespace convert
		{
#if	defined(__WIN32) || defined(__UWP)

			inline W_RESULT chars_to_GUID(const std::wstring& pStr, GUID& pGUID)
			{
				LPOLESTR guid = W2OLE((wchar_t*)pStr.c_str());
				auto hr = W_RESULT::W_PASSED;
				CLSIDFromString(guid, (LPCLSID)&pGUID);
				return hr;
			}

			inline void ANSI_to_UNICODE(_In_z_ const std::string& pANSI, _In_z_ std::wstring& pResult, _In_ const int pSize = 1024)
			{
				LPSTR _str = const_cast<LPSTR>(pANSI.c_str());
				auto _w = new WCHAR[pSize];
				std::memset(_w, 0, pSize);

				MultiByteToWideChar(CP_ACP, 0, &_str[0], -1, _w, pSize); // ANSI to UNICODE

				pResult = _w;

				delete[] _w;
			}

			//inline void ANSI_to_UTF8(_In_z_ const std::string& pANSI, _In_z_ std::wstring& pResult, _In_ const int pSize = 1024)
			//{
			//	LPSTR _str = const_cast<LPSTR>(pANSI.c_str());
			//	auto _w = new WCHAR[pSize];
			//	std::memset(_w, 0, pSize);

			//	MultiByteToWideChar(CP_ACP, 0, &_str[0], -1, _w, pSize); // ANSI to UNICODE
			//	WideCharToMultiByte(CP_UTF8, 0, _w, -1, &_str[0], pSize, 0, 0); // UNICODE to UTF-8

			//	pResult = _w;

			//	delete[] _w;
			//}

#endif //__WIN32 || __UWP

            // convert UTF-8 string to wstring
            inline std::wstring from_utf8(const std::string& pStr)
            {
                std::wstring_convert<std::codecvt_utf8<wchar_t>> _convert;
                return _convert.from_bytes(pStr);
            }
            
            // convert UTF-16 string to wstring
            inline std::wstring from_utf16(const std::string& pStr)
            {
                std::wstring_convert<std::codecvt_utf16<wchar_t>> _convert;
                return _convert.from_bytes(pStr);
            }
            
            // convert wstring to UTF-8 string
            inline std::string to_utf8(const std::wstring& pStr)
            {
                std::wstring_convert<std::codecvt_utf8<wchar_t>> _convert;
                return _convert.to_bytes(pStr);
            }
            
            // convert wstring to UTF-16 string
            inline std::string to_utf16(const std::wstring& pStr)
            {
                std::wstring_convert<std::codecvt_utf16<wchar_t>> _convert;
                return _convert.to_bytes(pStr);
            }
            
			inline int to_hex(const std::string& pStr)
			{
				std::stringstream _ss;
				_ss << pStr;
				int _hex;
				_ss >> std::hex >> _hex;
				_ss.clear();

				return _hex;
			}

			inline std::wstring chars_to_wstring(char* pValue)
			{
				std::string _str(pValue);
				return std::wstring(_str.begin(), _str.end());
			}

			inline std::wstring string_to_wstring(std::string pValue)
			{
				return std::wstring(pValue.begin(), pValue.end());
			}

			inline std::string wstring_to_string(std::wstring pValue)
			{
				return std::string(pValue.begin(), pValue.end());
			}

			inline std::wstring tchars_to_wstring(wchar_t* pValue, const size_t pLength)
			{
				auto _w = new wchar_t[pLength];
				for (size_t i = 0; i < pLength; ++i)
				{
					_w[i] = pValue[i];
				}
				auto _str = std::wstring(&_w[0]);

				delete[] _w;
				return _str;
			}

			inline std::string tchars_to_string(wchar_t* pValue, const size_t pLength)
			{
				auto wstr = tchars_to_wstring(pValue, pLength);
				return wstring_to_string(wstr);
			}

            inline bool has_string_start_with(_In_z_ const std::string& pString, _In_z_ const std::string pStartWith)
            {
                return strncmp(pStartWith.c_str(), pString.c_str(), pStartWith.size()) == 0;
            }

            inline bool has_wstring_start_with(_In_z_ const std::wstring& pString, _In_z_ const std::wstring pStartWith)
            {
                return wcsncmp(pStartWith.c_str(), pString.c_str(), pStartWith.size()) == 0;
            }
            
            inline bool has_string_end_with(_In_z_ std::string const& pStr, _In_z_ std::string const& pEnding)
            {
                if (pStr.length() >= pEnding.length())
                {
                    return (0 == pStr.compare(pStr.length() - pEnding.length(), pEnding.length(), pEnding));
                }
                return false;
            }

            inline bool has_wstring_end_with(_In_z_ std::wstring const& pStr, _In_z_ std::wstring const& pEnding)
            {
                if (pStr.length() >= pEnding.length())
                {
                    return (0 == pStr.compare(pStr.length() - pEnding.length(), pEnding.length(), pEnding));
                }
                return false;
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
#ifdef __WIN32
				else if (std::is_same<T, long long>::value || std::is_same < T, unsigned long long> ::value)
				{
					pResult.push_back(std::atoll(pStr.substr(pOffset, pCount).c_str()));
				}
#endif
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

			inline void split_string(const std::string& pStr, const std::string& pSplit, _Inout_ std::vector<std::string>& pResult)
			{
				split_string_then_convert_to<std::string>(pStr, pSplit, pResult);
			}
            
            template<class T>
            inline void find_all_numbers_then_convert_to(const std::string& pStr, _Inout_ std::vector<T>& pResult)
            {
                using namespace std;
                
                std::string _number;
                for(size_t i = 0 ; i < pStr.size() ; ++i)
                {
                    if ((pStr[i] >= '0' && pStr[i] <= '9') || pStr[i] == '.' || pStr[i] == '+' || pStr[i] == '-' || pStr[i] == 'e')
                    {
                        _number += pStr[i];
                    }
                    else
                    {
                        if(_number.size())
                        {
                            substr_function(_number, 0, _number.size(), pResult);
                            _number.clear();
                        }
                        continue;
                    }
                }

				if (_number.size())
				{
					substr_function(_number, 0, _number.size(), pResult);
					_number.clear();
				}
            }

#pragma endregion

#pragma region sub wstring convert functions

#ifdef __WIN32
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

			inline void split_wstring(const std::wstring& pStr, const std::wstring& pSplit, _Inout_ std::vector<std::wstring>& pResult)
			{
				split_wstring_then_convert_to<std::wstring>(pStr, pSplit, pResult);
			}

            template<class T>
            inline void find_all_numbers_then_convert_toW(const std::wstring& pStr, _Inout_ std::vector<T>& pResult)
            {
                using namespace std;
                
                std::string _number;
                for(size_t i = 0 ; i < pStr.size() ; ++i)
                {
                    if ((pStr[i] >= L'0' && pStr[i] <= L'9') || pStr[i] == L'.' || pStr[i] == L'+' || pStr[i] == L'-')
                    {
                        _number += pStr[i];
                    }
                    else
                    {
                        if(_number.size())
                        {
                            substr_function(_number, 0, _number.size(), pResult);
                            _number.clear();
                        }
                        continue;
                    }
                }

				if (_number.size())
				{
					substr_function(_number, 0, _number.size(), pResult);
					_number.clear();
				}
            }
            
#endif

#pragma endregion

		}
	}
}

#endif //__W_CONVERT_H__
