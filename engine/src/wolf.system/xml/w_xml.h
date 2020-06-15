/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_xml.h
	Description		 : XML serializer & parser using rapid xml "https://github.com/dwd/rapidxml"
	Comment          :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <wolf.h>

typedef apr_xml_attr* w_xml_attr;
typedef apr_xml_elem* w_xml_elem;
typedef apr_xml_doc* w_xml_doc;
typedef apr_xml_parser* w_xml_parser;

/**
 * Create an XML parser
 * @param pMemPool The pool for allocating the parser and the parse results.
 * @return The new parser.
 */
w_xml_parser w_xml_parser_init(_In_ w_mem_pool pMemPool);

/**
 * Parse a File, producing a xml_doc
 * @param pMemPool  The pool for allocating the parse results.
 * @param pParser    A pointer to *parser (needed so calling function can get errors), will be set to NULL on successful completion.
 * @param pDoc  A pointer to *apr_xml_doc (which has the parsed results in it)
 * @param pXMLFD  A file to read from.
 * @param pBufferLength Buffer length which would be suitable
 * @return result code
 */
W_RESULT    w_xml_parse_file(_In_       w_mem_pool pMemPool,
                             _Inout_    w_xml_parser* pParser,
                             _Inout_    w_xml_doc* pDoc,
                             _In_       w_file  pXMLFD,
                             _In_       size_t pBufferLength);

#ifdef __cplusplus
}
#endif


//
//
//
///**
// * Feed input into the parser
// * @param parser The XML parser for parsing this data.
// * @param data The data to parse.
// * @param len The length of the data.
// * @return Any errors found during parsing.
// * @remark Use apr_xml_parser_geterror() to get more error information.
// */
//APU_DECLARE(apr_status_t) apr_xml_parser_feed(apr_xml_parser *parser,
//                                              const char *data,
//                                              apr_size_t len);
//
///**
// * Terminate the parsing and return the result
// * @param parser The XML parser for parsing this data.
// * @param pdoc The resulting parse information. May be NULL to simply
// *             terminate the parsing without fetching the info.
// * @return Any errors found during the final stage of parsing.
// * @remark Use apr_xml_parser_geterror() to get more error information.
// */
//APU_DECLARE(apr_status_t) apr_xml_parser_done(apr_xml_parser *parser,
//                                              apr_xml_doc **pdoc);
//
///**
// * Fetch additional error information from the parser.
// * @param parser The XML parser to query for errors.
// * @param errbuf A buffer for storing error text.
// * @param errbufsize The length of the error text buffer.
// * @return The error buffer
// */
//APU_DECLARE(char *) apr_xml_parser_geterror(apr_xml_parser *parser,
//                                            char *errbuf,
//                                            apr_size_t errbufsize);
//
//
///**
// * Converts an XML element tree to flat text
// * @param p The pool to allocate out of
// * @param elem The XML element to convert
// * @param style How to covert the XML.  One of:
// * <PRE>
// *     APR_XML_X2T_FULL                start tag, contents, end tag
// *     APR_XML_X2T_INNER               contents only
// *     APR_XML_X2T_LANG_INNER          xml:lang + inner contents
// *     APR_XML_X2T_FULL_NS_LANG        FULL + ns defns + xml:lang
// *     APR_XML_X2T_PARSED              original prefixes
// * </PRE>
// * @param namespaces The namespace of the current XML element
// * @param ns_map Namespace mapping
// * @param pbuf Buffer to put the converted text into
// * @param psize Size of the converted text
// */
//APU_DECLARE(void) apr_xml_to_text(apr_pool_t *p, const apr_xml_elem *elem,
//                                  int style, apr_array_header_t *namespaces,
//                                  int *ns_map, const char **pbuf,
//                                  apr_size_t *psize);
//
///* style argument values: */
//#define APR_XML_X2T_FULL         0    /**< start tag, contents, end tag */
//#define APR_XML_X2T_INNER        1    /**< contents only */
//#define APR_XML_X2T_LANG_INNER   2    /**< xml:lang + inner contents */
//#define APR_XML_X2T_FULL_NS_LANG 3    /**< FULL + ns defns + xml:lang */
//#define APR_XML_X2T_PARSED       4    /**< original prefixes */
//
///**
// * empty XML element
// * @param p The pool to allocate out of
// * @param elem The XML element to empty
// * @return the string that was stored in the XML element
// */
//APU_DECLARE(const char *) apr_xml_empty_elem(apr_pool_t *p,
//                                             const apr_xml_elem *elem);
//
///**
// * quote an XML string
// * Replace '\<', '\>', and '\&' with '\&lt;', '\&gt;', and '\&amp;'.
// * @param p The pool to allocate out of
// * @param s The string to quote
// * @param quotes If quotes is true, then replace '&quot;' with '\&quot;'.
// * @return The quoted string
// * @note If the string does not contain special characters, it is not
// * duplicated into the pool and the original string is returned.
// */
//APU_DECLARE(const char *) apr_xml_quote_string(apr_pool_t *p, const char *s,
//                                               int quotes);
//
///**
// * Quote an XML element
// * @param p The pool to allocate out of
// * @param elem The element to quote
// */
//APU_DECLARE(void) apr_xml_quote_elem(apr_pool_t *p, apr_xml_elem *elem);
//
///* manage an array of unique URIs: apr_xml_insert_uri() and APR_XML_URI_ITEM() */
//
///**
// * return the URI's (existing) index, or insert it and return a new index
// * @param uri_array array to insert into
// * @param uri The uri to insert
// * @return int The uri's index
// */
//APU_DECLARE(int) apr_xml_insert_uri(apr_array_header_t *uri_array,
//                                    const char *uri);
//
///** Get the URI item for this XML element */
//#define APR_XML_GET_URI_ITEM(ary, i) (((const char * const *)(ary)->elts)[i])
//
//#if APR_CHARSET_EBCDIC
///**
// * Convert parsed tree in EBCDIC
// * @param p The pool to allocate out of
// * @param pdoc The apr_xml_doc to convert.
// * @param xlate The translation handle to use.
// * @return Any errors found during conversion.
// */
//APU_DECLARE(apr_status_t) apr_xml_parser_convert_doc(apr_pool_t *p,
//                                                     apr_xml_doc *pdoc,
//                                                     apr_xlate_t *convset);






//#include <xml/rapidxml/rapidxml.hpp>
//
//struct w_xml_data
//{
//	struct w_xml_data_attribute
//	{
//		wchar_t* name;
//		wchar_t* value;
//	};
//
//	wchar_t* node;
//	w_xml_data_attribute* attributes;
//	w_xml_data* children;
//};
//
///**
// * save xml to file
// * @param pPath path to the file
// * @param pXMLData xml data
// * @param pPreComment path to the file
// * @return result
// */
//W_SYSTEM_EXPORT W_RESULT w_xml_save_to_file(
//	_In_z_ const wchar_t* pPath,
//	_In_ w_xml_data* pXMLData,
//	_In_z_ const wchar_t* pPreComment);
//
///**
// * get xml node value
// * @param pNode xml node
// * @return node value
// */
//W_SYSTEM_EXPORT const char* w_xml_get_node_value(_In_ rapidxml::xml_node<>* pNode);
//
///**
// * get xml node as wide char
// * @param pNode xml node
// * @return node value
// */
//W_SYSTEM_EXPORT const wchar_t* w_xml_get_node_value_wchar(_In_ rapidxml::xml_node<>* pNode);
//
///**
// * get xml node attribute
// * @param pNode xml node
// * @param pAttribute xml node attribute
// * @return node value
// */
//W_SYSTEM_EXPORT const char* w_xml_get_node_attribute(
//	_In_ rapidxml::xml_node<>* pNode,
//	_In_z_ const char* const pAttribute);
//
///**
// * get xml node as wide char
// * @param pNode xml node
// * @param pAttribute xml node attribute
// * @return node attribute value
// */
//W_SYSTEM_EXPORT const wchar_t* w_xml_get_node_attribute_wchar(
//	_In_ rapidxml::xml_node<>* pNode,
//	_In_z_ const char* pAttribute);
	
