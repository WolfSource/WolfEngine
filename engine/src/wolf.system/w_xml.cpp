#include "w_system_pch.h"
#include "w_xml.h"
#include <rapidxml_print.hpp>
#include <fstream>
#include <w_convert.h>

using namespace wolf::system;
using namespace rapidxml;

w_xml::w_xml()
{
}


w_xml::~w_xml()
{
}

#if defined(__WIN32) || defined(__UWP)
HRESULT w_xml::save(_In_z_ const wchar_t* pPath, _In_ bool pUTF_8, _In_ wolf::system::w_xml_data& pData, _In_z_ const std::wstring pPreComment)
{
#else
HRESULT w_xml::save(_In_z_ const char* pPath, _In_ bool pUTF_8, _In_ wolf::system::w_xml_data& pData, _In_z_ const std::wstring pPreComment)
{	
#endif
	
	std::wofstream _file(pPath);
	if (!_file) return S_FALSE;

	if (pUTF_8)
	{
		_file.imbue(std::locale(std::locale(""), new std::codecvt_utf8<wchar_t, 0x10ffff, std::generate_header>));
	}

	xml_document<wchar_t> _doc;

	//Add xml version & encoding
	auto _node = _doc.allocate_node(node_declaration);
	_node->append_attribute(_doc.allocate_attribute(L"version", L"1.0"));
	_node->append_attribute(_doc.allocate_attribute(L"encoding", L"utf-8"));
	_doc.append_node(_node);

	_write_element(pData, _doc, nullptr);
	
	//print xml to stream
	std::wstring _xml_as_string;
	rapidxml::print(std::back_inserter(_xml_as_string), _doc);

	_file << pPreComment;
	_file << _xml_as_string;

	_file.flush();
	_file.close();
	
	_doc.clear();

	return S_OK;
}

void w_xml::_write_element(_In_ w_xml_data& pData, _In_ xml_document<wchar_t>& pDoc, _Inout_ xml_node<wchar_t>** pParentNode)
{
	auto _node = pDoc.allocate_node(node_element, pData.node.c_str());
	auto _attributeSize = pData.attributes.size();
	for (size_t i = 0; i < _attributeSize; ++i)
	{
		auto _attr = pData.attributes.at(i);
		auto _name = pDoc.allocate_string(_attr.name.c_str());
		auto _value = pDoc.allocate_string(_attr.value.c_str());

		auto _attribute = pDoc.allocate_attribute(_name, _value);
		_node->append_attribute(_attribute);
	}

	//Add to the parent

	auto _childSize = pData.children.size();
	if (_childSize != 0)
	{
		for (size_t i = 0; i < _childSize; ++i)
		{
			_write_element(pData.children.at(i), pDoc, &_node);
		}
	}

	if (pParentNode)
	{
		(*pParentNode)->append_node(_node);
	}
	else
	{
		pDoc.append_node(_node);
	}
}

const std::string w_xml::get_node_value(_In_ rapidxml::xml_node<>* pNode)
{
	if (pNode == nullptr) return "";
	return pNode->value();
}

const std::string w_xml::get_node_attribute(_In_ rapidxml::xml_node<>* pNode, _In_z_ const char* const pAttribute)
{
	if (pNode == nullptr) return "";

	//read the value of attribute
	auto _value = pNode->first_attribute(pAttribute);
	if (_value)
	{
		return _value->value();
	}

	return "";
}

const std::wstring w_xml::get_node_value_utf8(_In_ rapidxml::xml_node<>* pNode)
{
	if (pNode == nullptr) return L"";

	return wolf::system::convert::from_utf8(pNode->value());
}

const std::wstring w_xml::get_node_attribute_utf8(_In_ rapidxml::xml_node<>* pNode, _In_z_ const char* const pAttribute)
{
	if (pNode == nullptr) return L"";

	//read the value of attribute
	auto _value = pNode->first_attribute(pAttribute);
	if (_value)
	{
		return wolf::system::convert::from_utf8(_value->value());
	}

	return L"";
}
