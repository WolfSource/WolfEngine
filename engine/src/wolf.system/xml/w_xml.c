#include "w_xml.h"
#include <apr-util/apr_xml.h>

w_xml_parser w_xml_parser_init(_In_ w_mem_pool pMemPool)
{
    w_mem_pool _pool = NULL;
    if (pMemPool)
    {
        _pool = pMemPool;
    }
    else
    {
        //get default thread pool
        _pool = w_mem_pool_get_default();
        if (!_pool)
        {
            W_ASSERT(false, "could not get default memory pool. trace info: apr_xml_parser_create");
            return NULL;
        }
    }
    
    return apr_xml_parser_create(_pool);
}

W_RESULT   w_xml_parse_file(_In_    w_mem_pool pMemPool,
                            _Inout_ w_xml_parser* pParser,
                            _Inout_ w_xml_doc* pDoc,
                            _In_    w_file  pXMLFD,
                            _In_    size_t pBufferLength)
{
    w_mem_pool _pool = NULL;
    if (pMemPool)
    {
        _pool = pMemPool;
    }
    else
    {
        //get default thread pool
        _pool = w_mem_pool_get_default();
        if (!_pool)
        {
            W_ASSERT(false, "could not get default memory pool. trace info: apr_xml_parser_create");
            return W_FAILURE;
        }
    }
    
    return apr_xml_parse_file(pMemPool,
                              pParser,
                              pDoc,
                              pXMLFD,
                              pBufferLength);
}

W_RESULT    w_xml_parser_feed(_In_      w_xml_parser pParser,
                              _In_z_    const char* pData,
                              _In_      size_t pLen)
{
    if (!pParser || !pData || !pLen)
    {
        W_ASSERT(false, "invalid parameters. trace info: w_xml_parser_feed");
        return APR_BADARG;
    }
    return apr_xml_parser_feed(pParser, pData, pLen);
}

W_RESULT    w_xml_parser_done(_In_      w_xml_parser pParser,
                              _Inout_   w_xml_doc* pPDOC)
{
    if (!pParser || !pPDOC)
    {
        W_ASSERT(false, "invalid parameters. trace info: w_xml_parser_done");
        return APR_BADARG;
    }
    return apr_xml_parser_done(pParser, pPDOC);
}

char*       w_xml_parser_get_error(_In_      w_xml_parser pParser,
                                   _Inout_   char* pErrorBuffer,
                                   _In_      size_t pErrorBufferSize)
{
    if (!pParser || !pErrorBuffer || !pErrorBufferSize)
    {
        W_ASSERT(false, "invalid parameters. trace info: w_xml_parser_get_error");
        return NULL;
    }
    return w_xml_parser_get_error(pParser, pErrorBuffer, pErrorBufferSize);
}

void        w_xml_to_text(_In_ w_mem_pool pMemPool,
                          _In_ const w_xml_elem pElemenet,
                          _In_ int pStyle,
                          _In_ w_array pNamespaces,
                          _Inout_ int* pNSMap,
                          _Inout_ const char** pBuffer,
                          _Inout_ size_t* pSize)
{
    w_mem_pool _pool = NULL;
    if (pMemPool)
    {
        _pool = pMemPool;
    }
    else
    {
        //get default thread pool
        _pool = w_mem_pool_get_default();
        if (!_pool)
        {
            W_ASSERT(false, "could not get default memory pool. trace info: w_xml_to_text");
            return;
        }
    }
    
    if (!pElemenet)
     {
         W_ASSERT(false, "invalid parameters. trace info: w_xml_to_text");
         return;
     }
     return w_xml_to_text(_pool,
                          pElemenet,
                          pStyle,
                          pNamespaces,
                          pNSMap,
                          pBuffer,
                          pSize);
}

const char* w_xml_empty_element(_In_ w_mem_pool pMemPool,
                                _In_ const w_xml_elem pElemenet)
{
    w_mem_pool _pool = NULL;
    if (pMemPool)
    {
        _pool = pMemPool;
    }
    else
    {
        //get default thread pool
        _pool = w_mem_pool_get_default();
        if (!_pool)
        {
            W_ASSERT(false, "could not get default memory pool. trace info: w_xml_empty_element");
            return NULL;
        }
    }
    
    if (!pElemenet)
    {
        W_ASSERT(false, "invalid parameters. trace info: w_xml_empty_elem");
        return NULL;
    }
     return w_xml_empty_element(_pool, pElemenet);
}

const char* w_xml_quote_string(_In_ w_mem_pool pMemPool,
                               _In_z_ const char* pString,
                               _In_ int pQuotes)
{
    w_mem_pool _pool = NULL;
    if (pMemPool)
    {
        _pool = pMemPool;
    }
    else
    {
        //get default thread pool
        _pool = w_mem_pool_get_default();
        if (!_pool)
        {
            W_ASSERT(false, "could not get default memory pool. trace info: w_xml_empty_element");
            return NULL;
        }
    }
       
    if (!pString)
    {
        W_ASSERT(false, "invalid parameters. trace info: w_xml_empty_elem");
        return NULL;
    }
    return apr_xml_quote_string(_pool, pString, pQuotes);
}

void w_xml_quote_element(_In_ w_mem_pool pMemPool,
                         _In_ w_xml_elem pElement)
{
    w_mem_pool _pool = NULL;
    if (pMemPool)
    {
        _pool = pMemPool;
    }
    else
    {
        //get default thread pool
        _pool = w_mem_pool_get_default();
        if (!_pool)
        {
            W_ASSERT(false, "could not get default memory pool. trace info: w_xml_quote_element");
            return;
        }
    }
       
    if (!pElement)
    {
        W_ASSERT(false, "invalid parameters. trace info: w_xml_empty_elem");
        return;
    }
    return w_xml_quote_element(_pool, pElement);
}

int w_xml_insert_uri(_In_    w_array pURIArray,
                     _In_z_  const char* pURI)
{       
     apr_array_header_t *temp;
    temp = ((apr_array_header_t*)&pURIArray);
    return apr_xml_insert_uri(temp, pURI);
}
