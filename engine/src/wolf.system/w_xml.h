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

#include "w_system_dll.h"
#include <rapidxml.hpp>

namespace wolf
{
	namespace system
	{
		struct w_xml_data
		{
			std::wstring node;
			std::vector<std::tuple<std::wstring, std::wstring>> attributes;
			std::vector<w_xml_data> children;
		};

		class w_xml
		{
		public:
			SYS_EXP w_xml();
			SYS_EXP ~w_xml();

			SYS_EXP void save(_In_z_ std::wstring& pPath, bool pUTF_8, wolf::system::w_xml_data& pData);

		private:
			void _write_element(wolf::system::w_xml_data& pData, rapidxml::xml_document<wchar_t>& pDoc, _Inout_ rapidxml::xml_node<wchar_t>** pParentNode);
		};
	}
}

#endif