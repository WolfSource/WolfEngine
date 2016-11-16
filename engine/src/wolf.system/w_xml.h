/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_xml.h
	Description		 : XML serializer & parser using rapid xml "https://github.com/dwd/rapidxml"
	Comment          :
*/

#ifndef __W_XML_H__
#define __W_XML_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "w_system_export.h"
#include <rapidxml.hpp>
#include <vector>

namespace wolf
{
	namespace system
	{
		struct w_xml_data
		{
			struct w_xml_data_attribute 
			{
				std::wstring name;
				std::wstring value;
			};

			std::wstring node;
			std::vector<w_xml_data_attribute> attributes;
			std::vector<w_xml_data> children;
		};

		class w_xml
		{
		public:
			WSYS_EXP w_xml();
			WSYS_EXP ~w_xml();

			//save xml
			WSYS_EXP static HRESULT save(_In_z_ const char* pPath, _In_ bool pUTF_8, _In_ wolf::system::w_xml_data& pData);
			//get xml node value
			WSYS_EXP static const std::string	get_node_value(_In_ rapidxml::xml_node<>* pNode);
			//get xml node attribute value
			WSYS_EXP static const std::string	get_node_attribute(_In_ rapidxml::xml_node<>* pNode, _In_z_ const char* const pAttribute);

#ifdef WIN32
			//get xml node value as utf8
			WSYS_EXP static const std::wstring	get_node_value_utf8(_In_ rapidxml::xml_node<>* pNode);
			//get xml node attribute value as utf8
			WSYS_EXP static const std::wstring	get_node_attribute_utf8(_In_ rapidxml::xml_node<>* pNode, _In_z_ const char* pAttribute);
#endif

		private:
			static void _write_element(_In_ wolf::system::w_xml_data& pData, _In_ rapidxml::xml_document<wchar_t>& pDoc, _Inout_ rapidxml::xml_node<wchar_t>** pParentNode);
		};
	}
}

#endif //__W_XML_H__