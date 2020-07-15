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

typedef struct apr_xml_attr* w_xml_attr;
typedef struct apr_xml_elem* w_xml_elem;
typedef struct apr_xml_doc* w_xml_doc;
typedef struct apr_xml_parser* w_xml_parser;

/**
 * Create an XML parser
 * @param pMemPool The pool for allocating the parser and the parse results.
 * @return The new parser.
 */
W_SYSTEM_EXPORT
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
W_SYSTEM_EXPORT
W_RESULT    w_xml_parse_file(_In_       w_mem_pool pMemPool,
                             _Inout_    w_xml_parser* pParser,
                             _Inout_    w_xml_doc* pDoc,
                             _In_       w_file  pXMLFD,
                             _In_       size_t pBufferLength);

/**
 * Feed input into the parser
 * @param pParser The XML parser for parsing this data.
 * @param pData The data to parse.
 * @param pLen The length of the data.
 * @return Any errors found during parsing.
 * @remark Use w_xml_parser_get_error() to get more error information.
 */
W_SYSTEM_EXPORT
W_RESULT    w_xml_parser_feed(_In_      w_xml_parser pParser,
                              _In_z_    const char* pData,
                              _In_      size_t pLen);

/**
 * Terminate the parsing and return the result
 * @param pParser The XML parser for parsing this data.
 * @param pPDOC The resulting parse information. May be NULL to simply terminate the parsing without fetching the info.
 * @return Any errors found during the final stage of parsing.
 * @remark Use w_xml_parser_get_error() to get more error information.
 */
W_SYSTEM_EXPORT
W_RESULT    w_xml_parser_done(_In_      w_xml_parser pParser,
                              _Inout_   w_xml_doc* pPDOC);

/**
 * Fetch additional error information from the parser.
 * @param pParser The XML parser to query for errors.
 * @param pErrorBuffer A buffer for storing error text.
 * @param pErrorBufferSize The length of the error text buffer.
 * @return The error buffer
 */
W_SYSTEM_EXPORT
char*       w_xml_parser_get_error(_In_      w_xml_parser pParser,
                                   _Inout_   char* pErrorBuffer,
                                   _In_      size_t pErrorBufferSize);
/**
 * Converts an XML element tree to flat text
 * @param pMemPool The pool to allocate out of
 * @param pElemenet The XML element to convert
 * @param pStyle How to covert the XML.  One of:
 * <PRE>
 *     APR_XML_X2T_FULL                start tag, contents, end tag
 *     APR_XML_X2T_INNER               contents only
 *     APR_XML_X2T_LANG_INNER          xml:lang + inner contents
 *     APR_XML_X2T_FULL_NS_LANG        FULL + ns defns + xml:lang
 *     APR_XML_X2T_PARSED              original prefixes
 * </PRE>
 * @param pNamespaces The namespace of the current XML element
 * @param pNSMap Namespace mapping
 * @param pBuffer Buffer to put the converted text into
 * @param pSize Size of the converted text
 */
W_SYSTEM_EXPORT
void        w_xml_to_text(_In_ w_mem_pool pMemPool,
                          _In_ const w_xml_elem pElemenet,
                          _In_ int pStyle,
                          _In_ w_array pNamespaces,
                          _Inout_ int* pNSMap,
                          _Inout_ const char** pBuffer,
                          _Inout_ size_t* pSize);

/**
 * empty XML element
 * @param pMemPool The pool to allocate out of
 * @param pElemenet The XML element to empty
 * @return the string that was stored in the XML element
 */
W_SYSTEM_EXPORT
const char* w_xml_empty_element(_In_ w_mem_pool pMemPool,
                                _In_ const w_xml_elem pElemenet);


/**
 * quote an XML string
 * Replace '\<', '\>', and '\&' with '\&lt;', '\&gt;', and '\&amp;'.
 * @param pMemPool The pool to allocate out of
 * @param pString The string to quote
 * @param pQuotes If quotes is true, then replace '&quot;' with '\&quot;'.
 * @return The quoted string
 * @note If the string does not contain special characters, it is not
 * duplicated into the pool and the original string is returned.
 */
W_SYSTEM_EXPORT
const char* w_xml_quote_string(_In_ w_mem_pool pMemPool,
                               _In_z_ const char* pString,
                               _In_ int pQuotes);

/**
 * Quote an XML element
 * @param pMemPool The pool to allocate out of
 * @param pElement The element to quote
 */
W_SYSTEM_EXPORT
void w_xml_quote_element(_In_ w_mem_pool pMemPool,
                         _In_ w_xml_elem pElement);

/**
 * return the URI's (existing) index, or insert it and return a new index
 * @param pURIArray array to insert into
 * @param pURI The uri to insert
 * @return int The uri's index
 */
W_SYSTEM_EXPORT
int w_xml_insert_uri(_In_    w_array pURIArray,
                     _In_z_  const char* pURI);

#ifdef __cplusplus
}
#endif
