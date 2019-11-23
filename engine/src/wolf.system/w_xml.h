/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_xml.h
	Description		 : XML serializer & parser using rapid xml "https://github.com/dwd/rapidxml"
	Comment          :
*/

#pragma once

#include "w_system_export.h"
#include "rapidxml/rapidxml.hpp"
#include <string>
#include "w_std.h"

namespace wolf::system
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

#if defined(__WIN32) || defined(__UWP)
		WSYS_EXP static W_RESULT save(_In_z_ const wchar_t* pPath,
#else
		WSYS_EXP static W_RESULT save(_In_z_ const char* pPath,
#endif
			_In_ const bool& pUTF_8, _In_ wolf::system::w_xml_data & pData, _In_z_ const std::wstring pPreComment = L"");

		//get xml node value
		WSYS_EXP static const std::string	get_node_value(_In_ rapidxml::xml_node<> * pNode);
		//get xml node attribute value
		WSYS_EXP static const std::string	get_node_attribute(_In_ rapidxml::xml_node<> * pNode, _In_z_ const char* const pAttribute);

		//get xml node value as utf8
		WSYS_EXP static const std::wstring	get_node_value_utf8(_In_ rapidxml::xml_node<> * pNode);
		//get xml node attribute value as utf8
		WSYS_EXP static const std::wstring	get_node_attribute_utf8(_In_ rapidxml::xml_node<> * pNode, _In_z_ const char* pAttribute);

	private:
		static void _write_element(_In_ wolf::system::w_xml_data & pData, _In_ rapidxml::xml_document<wchar_t> & pDoc, _Inout_ rapidxml::xml_node<wchar_t> * *pParentNode);
	};
}