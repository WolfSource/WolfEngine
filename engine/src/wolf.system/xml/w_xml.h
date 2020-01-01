/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_xml.h
	Description		 : XML serializer & parser using rapid xml "https://github.com/dwd/rapidxml"
	Comment          :
*/

#pragma once

#include <wolf.h>
#include <xml/rapidxml/rapidxml.hpp>

struct w_xml_data
{
	struct w_xml_data_attribute
	{
		wchar_t* name;
		wchar_t* value;
	};

	wchar_t* node;
	w_xml_data_attribute* attributes;
	w_xml_data* children;
};

/**
 * save xml to file
 * @param pPath path to the file
 * @param pXMLData xml data
 * @param pPreComment path to the file
 * @return result
 */
W_SYSTEM_EXPORT W_RESULT w_xml_save_to_file(
	_In_z_ const wchar_t* pPath,
	_In_ w_xml_data* pXMLData,
	_In_z_ const wchar_t* pPreComment);

/**
 * get xml node value
 * @param pNode xml node
 * @return node value
 */
W_SYSTEM_EXPORT const char* w_xml_get_node_value(_In_ rapidxml::xml_node<>* pNode);
	
/**
 * get xml node as wide char
 * @param pNode xml node
 * @return node value
 */
W_SYSTEM_EXPORT const wchar_t* w_xml_get_node_value_wchar(_In_ rapidxml::xml_node<> * pNode);
	
/**
 * get xml node attribute
 * @param pNode xml node
 * @param pAttribute xml node attribute
 * @return node value
 */
W_SYSTEM_EXPORT const char* w_xml_get_node_attribute(
	_In_ rapidxml::xml_node<>* pNode,
	_In_z_ const char* const pAttribute);

/**
 * get xml node as wide char
 * @param pNode xml node
 * @param pAttribute xml node attribute
 * @return node attribute value
 */
W_SYSTEM_EXPORT const wchar_t* w_xml_get_node_attribute_wide_char(
	_In_ rapidxml::xml_node<>* pNode, 
	_In_z_ const char* pAttribute);
	