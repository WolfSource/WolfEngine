#include "w_table.h"
#include <apr-1/apr_tables.h>

W_RESULT w_table_init(_Inout_ w_table* pTable, _In_ size_t pInitSize, _In_ w_mem_pool pMemPool)
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
            W_ASSERT(false, "could not get default memory pool. trace info: w_table_init");
            return W_FAILURE;
        }
    }
    *pTable = apr_table_make(_pool, pInitSize);
    if (*pTable)
    {
        return W_SUCCESS;
    }
    return W_FAILURE;
}

W_RESULT w_table_set(_In_ w_table pTable, char* pKey, char* pValue, bool pMakeACopy)
{
    if (!pTable)
    {
        W_ASSERT(false, "Missing pTable. trace info: w_table_set");
        return APR_BADARG;
    }

    if (pMakeACopy)
    {
        apr_table_set(pTable, pKey, pValue);
    }
    else
    {
        //no copy
        apr_table_setn(pTable, pKey, pValue);
    }

    return W_SUCCESS;
}

W_RESULT w_table_unset(_In_ w_table pTable, char* pKey)
{
    if (!pTable)
    {
        W_ASSERT(false, "Missing pTable. trace info: w_table_unset");
        return APR_BADARG;
    }
    apr_table_unset(pTable, pKey);
    return W_SUCCESS;
}

W_RESULT w_table_add(_In_ w_table pTable, char* pKey, char* pValue)
{
    if (!pTable)
    {
        W_ASSERT(false, "Missing pTable. trace info: w_table_add");
        return APR_BADARG;
    }
    apr_table_add(pTable, pKey, pValue);
    return W_SUCCESS;
}

int w_table_get_size(_In_ w_table pTable)
{
    if (!pTable)
    {
        W_ASSERT(false, "Missing pTable. trace info: w_table_get_size");
        return -1;
    }
    const apr_array_header_t* _header = apr_table_elts(pTable);
    return _header->nelts;
}

int w_table_is_empty(_In_ w_table pTable)
{
    if (!pTable)
    {
        W_ASSERT(false, "Missing pTable. trace info: w_table_get_size");
        return -1;
    }
    const apr_array_header_t* _header = apr_table_elts(pTable);
    return apr_is_empty_table(_header);
}

void w_table_clear(_In_ w_table pTable)
{
    if (!pTable)
    {
        W_ASSERT(false, "Missing pTable. trace info: w_table_get_size");
        return;
    }
    apr_table_clear(pTable);
}

int w_table_do(_In_ w_table pTable,
               _In_ w_table_do_callback pCallBack,
               _In_ void* pArg)
{
    if (!pTable)
    {
        W_ASSERT(false, "Missing pTable. trace info: w_table_do");
        return FALSE;
    }
    return apr_table_do(pCallBack, pArg, pTable, NULL);
}

int w_table_do_with_filter(
    _In_ w_table pTable,
    _In_ w_table_do_callback pCallBack,
    _In_ void* pArg,
    _In_ va_list pKeys)
{
    if (!pTable)
    {
        W_ASSERT(false, "Missing pTable. trace info: w_table_do_with_filter");
        return FALSE;
    }
    return apr_table_do(pCallBack, pArg, pTable, pKeys,NULL);
}

const w_table_entry_iterator w_table_get_entry(_In_ w_table pTable)
{
    if (!pTable)
    {
        W_ASSERT(false, "Missing pTable. trace info: w_table_get_entry");
        return FALSE;
    }
    const apr_array_header_t* _header = apr_table_elts(pTable);
    return (const apr_table_entry_t*)_header->elts;
}

char* w_table_get_key(_In_ const w_table_entry_iterator pTableEntry, _In_ size_t pIndex)
{
    return pTableEntry ? pTableEntry[pIndex].key : NULL;
}

char* w_table_get_value(_In_ const w_table_entry_iterator pTableEntry, _In_ size_t pIndex)
{
    return pTableEntry ? pTableEntry[pIndex].val : NULL;
}

size_t w_table_get_key_checksum(_In_ const w_table_entry_iterator pTableEntry, _In_ size_t pIndex)
{
    return pTableEntry ? pTableEntry[pIndex].key_checksum : 0;
}
