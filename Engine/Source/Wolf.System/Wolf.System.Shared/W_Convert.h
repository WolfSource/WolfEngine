/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @PooyaEimandar on twitter
	Name			 : W_Convert.h
	Description		 : Helper functions for converting types to each others
	Comment			 :
*/

#pragma once

#include <string.h>
#include <iostream>
#include <AtlConv.h>

namespace Wolf
{
	namespace System
	{
		namespace Convert
		{
			inline std::wstring CharsToWString(CHAR* value)
			{
				std::string str = value;
				return std::wstring(str.begin(), str.end());
			}

			inline std::wstring StringToWString(std::string value)
			{
				return std::wstring(value.begin(), value.end());
			}

			inline std::string WStringToString(std::wstring value)
			{
				return std::string(value.begin(), value.end());
			}

			inline std::wstring TCharsToWString(TCHAR* pValue, const UINT pLength)
			{
				auto wct = new wchar_t[pLength];
				for (size_t i = 0; i < pLength; ++i)
				{
					wct[i] = pValue[i];
				}
				return std::wstring(&wct[0]);
			}

			inline std::string TCharsToString(TCHAR* pValue, const UINT pLength)
			{
				auto wstr = TCharsToWString(pValue, pLength);
				return WStringToString(wstr);
			}

			inline HRESULT CharsToGUID(std::wstring value, GUID& GUID)
			{
				LPOLESTR guid = W2OLE((wchar_t*)value.c_str());
				auto hr = S_OK;
				CLSIDFromString(guid, (LPCLSID)&GUID);
				return hr;
			}
		}
	}
}