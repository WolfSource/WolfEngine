#include "w_system_pch.h"
#include "w_xml.h"
#include "rapidxml_print.hpp"

using namespace wolf::system;
using namespace rapidxml;

w_xml::w_xml()
{
}


w_xml::~w_xml()
{
}

void w_xml::save(_In_z_ std::wstring& p_path, bool p_UTF_8, w_xml_data& p_data)
{
	xml_document<wchar_t> _doc;
	
	//Add xml version & encoding
	auto _node = _doc.allocate_node(node_declaration);
	_node->append_attribute(_doc.allocate_attribute(L"version", L"1.0"));
	_node->append_attribute(_doc.allocate_attribute(L"encoding", L"utf-8"));
	_doc.append_node(_node);

	_write_element(p_data, _doc, nullptr);
	
	std::wstring _xml_as_string;
	rapidxml::print(std::back_inserter(_xml_as_string), _doc);

	//Logger.Write(_xml_as_string);

	_doc.clear();

	std::wofstream _file(p_path);
	if (p_UTF_8)
	{
		_file.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::generate_header>));
	}
	_file << _xml_as_string;
	_file.flush();
	_file.close();
}

void w_xml::_write_element(w_xml_data& pData, xml_document<wchar_t>& pDoc, _Inout_ xml_node<wchar_t>** pParentNode)
{
	auto _node = pDoc.allocate_node(node_element, pData.node.c_str());
	auto _attributeSize = pData.attributes.size();
	for (size_t i = 0; i < _attributeSize; ++i)
	{
		auto _tuple = pData.attributes.at(i);
		auto _name = pDoc.allocate_string(std::get<0>(_tuple).c_str());
		auto _value = pDoc.allocate_string(std::get<1>(_tuple).c_str());

		//Logger.Write(_name);
		//Logger.Write(_value);

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
