#include "w_xml.h"
#include <io/w_io.h>
#include <memory/w_string.h>
#include "rapidxml/rapidxml_print.hpp"

using namespace rapidxml;

W_RESULT w_xml_save_to_file(
	_In_z_	const wchar_t* pPath, 
	_In_	w_xml_data* pXMLData, 
	_In_z_	const wchar_t* pPreComment)
{
	xml_document<wchar_t> _doc;

	//Add xml version & encoding
	auto _node = _doc.allocate_node(node_declaration);
	_node->append_attribute(_doc.allocate_attribute(L"version", L"1.0"));
	_node->append_attribute(_doc.allocate_attribute(L"encoding", L"utf-8"));
	_doc.append_node(_node);

	//print xml to stream
	std::wstring _xml_as_wstring = pPreComment;
	rapidxml::print(std::back_inserter(_xml_as_wstring), _doc);

	//convert wchar_t* to utf8 char* for path
	size_t _in_len = wcslen(pPath), _out_len = 0;
	char* _new_path = (char*)w_alloc(_in_len * 2);
	W_RESULT _ret = w_io_ucs2_to_utf8(
		(apr_uint16_t*)pPath,
		&_in_len,
		_new_path,
		&_out_len);
	if (_ret != APR_SUCCESS)
	{
		return W_FAILED;
	}

	//convert wchar_t* to utf8 char* for content
	_in_len = _xml_as_wstring.size(), _out_len = 0;
	char* _xml_content = (char*)w_alloc(_in_len * 2);
	_ret = w_io_ucs2_to_utf8(
		(apr_uint16_t*)_xml_as_wstring.size(),
		&_in_len,
		_xml_content,
		&_out_len);
	if (_ret != APR_SUCCESS)
	{
		return W_FAILED;
	}

	//save it to file
	_ret = w_io_save_to_file(
		_new_path,
		_xml_content,
		false,
		false,
		false,
		false,
		false,
		true,
		false);

	_doc.clear();

	return W_FAILED;
}

void _xml_write_element(
	_In_ w_xml_data* pData, 
	_In_ xml_document<wchar_t>* pDoc, 
	_Inout_ xml_node<wchar_t>** pParentNode)
{
	auto _node = pDoc->allocate_node(node_element, pData->node);
	auto _attribute_len = sizeof(pData->attributes) / sizeof(pData->attributes[0]);
	for (size_t i = 0; i < _attribute_len; ++i)
	{
		auto _attr = pData->attributes[i];
		auto _name = pDoc->allocate_string(_attr.name);
		auto _value = pDoc->allocate_string(_attr.value);

		auto _attribute = pDoc->allocate_attribute(_name, _value);
		_node->append_attribute(_attribute);
	}

	//Add to the parent

	auto _child_len = sizeof(pData->children) / sizeof(pData->children[0]);
	if (_child_len != 0)
	{
		for (size_t i = 0; i < _child_len; ++i)
		{
			_xml_write_element(
				&pData->children[i],
				pDoc,
				&_node);
		}
	}

	if (pParentNode)
	{
		(*pParentNode)->append_node(_node);
	}
	else
	{
		pDoc->append_node(_node);
	}
}

const char* w_xml_get_node_value(_In_ rapidxml::xml_node<>* pNode)
{
	if (pNode == nullptr) return "";
	return pNode->value();
}

const wchar_t* w_xml_get_node_value_wchar(_In_ rapidxml::xml_node<>* pNode)
{
	if (pNode == nullptr) return L"";

	char* _nv = pNode->value();
	size_t _in_len = strlen(_nv), _out_len = 0;
	apr_uint16_t* _new_value = (apr_uint16_t*)w_alloc(_in_len * 2);

	W_RESULT _ret = w_io_utf8_to_ucs2(
		_nv,
		&_in_len,
		_new_value,
		&_out_len);
	if (_ret != APR_SUCCESS)
	{
		return L"";
	}

	return (wchar_t*)_new_value;
}

const char* w_xml_get_node_attribute(
	_In_ rapidxml::xml_node<>* pNode, 
	_In_z_ const char* const pAttribute)
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

const wchar_t* w_xml_get_node_attribute_wide_char(
	_In_ rapidxml::xml_node<>* pNode,
	_In_z_ const char* pAttribute)
{
	if (pNode == nullptr) return L"";
	//read the value of attribute
	auto _value = pNode->first_attribute(pAttribute);
	char* _nv = _value->value();
	size_t _in_len = strlen(_nv), _out_len = 0;
	apr_uint16_t* _new_value = (apr_uint16_t*)w_alloc(_in_len * 2);

	//convert from char* to wchar*
	W_RESULT _ret = w_io_utf8_to_ucs2(
		_nv,
		&_in_len,
		_new_value,
		&_out_len);
	if (_ret != APR_SUCCESS)
	{
		return L"";
	}

	return (wchar_t*)_new_value;
}