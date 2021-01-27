#include "w_table.h"
#include <apr_tables.h>
#include "log/w_log.h"

W_RESULT w_table_init(
    _Inout_ w_mem_pool pMemPool,
    _Inout_ w_table* pTable, 
    _In_ size_t pInitSize)
{
    const char* _trace_info = "w_table_init";
    if (!pMemPool)
    {
        W_ASSERT_P(false, "invalid parameter! trace info: %s", _trace_info);
        return APR_BADARG;
    }

    apr_pool_t* _pool = w_mem_pool_get_apr_pool(pMemPool);
    if (_pool)
    {
        *pTable = apr_table_make(_pool, (int)pInitSize);
        if (*pTable)
        {
            return W_SUCCESS;
        }
    }
    return W_FAILURE;
}

W_RESULT w_table_set(_In_ w_table pTable, char* pKey, char* pValue, bool pMakeACopy)
{
    const char* _trace_info = "w_table_set";

    if (!pTable || !pKey)
    {
        W_ASSERT_P(false, "invalid parameters! trace info: %s", _trace_info);
        return APR_BADARG;
    }

    if (pMakeACopy)
    {
        // make a copy
        apr_table_set(pTable, pKey, pValue);
    }
    else
    {
        // no copy
        apr_table_setn(pTable, pKey, pValue);
    }

    return W_SUCCESS;
}

W_RESULT w_table_unset(_In_ w_table pTable, char* pKey)
{
    const char* _trace_info = "w_table_unset";
    if (!pTable || !pKey)
    {
        W_ASSERT_P(false, "invalid parameters! trace info: %s", _trace_info);
        return APR_BADARG;
    }
    apr_table_unset(pTable, pKey);
    return W_SUCCESS;
}

W_RESULT w_table_add(_In_ w_table pTable, char* pKey, char* pValue)
{
    const char* _trace_info = "w_table_add";
    if (!pTable)
    {
        W_ASSERT_P(false, "invalid parameter! trace info: %s", _trace_info);
        return APR_BADARG;
    }
    apr_table_add(pTable, pKey, pValue);
    return W_SUCCESS;
}

int w_table_get_size(_In_ w_table pTable)
{
    const char* _trace_info = "w_table_get_size";
    if (!pTable)
    {
        W_ASSERT_P(false, "invalid parameter! trace info: %s", _trace_info);
        return -1;
    }
    const apr_array_header_t* _header = apr_table_elts(pTable);
    return _header->nelts;
}

int w_table_is_empty(_In_ w_table pTable)
{
    const char* _trace_info = "w_table_is_empty";
    if (!pTable)
    {
        W_ASSERT_P(false, "invalid parameter! trace info: %s", _trace_info);
        return -1;
    }
    return apr_is_empty_table(pTable);
}

void w_table_clear(_In_ w_table pTable)
{
    const char* _trace_info = "w_table_clear";
    if (!pTable)
    {
        W_ASSERT_P(false, "invalid parameter! trace info: %s", _trace_info);
        return;
    }
    apr_table_clear(pTable);
}

int w_table_do(_In_ w_table pTable,
               _In_ w_table_do_callback pCallBack,
               _In_ void* pArg)
{
    const char* _trace_info = "w_table_do";
    if (!pTable)
    {
        W_ASSERT_P(false, "invalid parameter! trace info: %s", _trace_info);
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
    const char* _trace_info = "w_table_do_with_filter";
    if (!pTable)
    {
        W_ASSERT_P(false, "invalid parameter! trace info: %s", _trace_info);
        return FALSE;
    }
    return apr_table_do(pCallBack, pArg, pTable, pKeys, NULL);
}

const w_table_entry_iterator w_table_get_entry(_In_ w_table pTable)
{
    const char* _trace_info = "w_table_get_entry";
    if (!pTable)
    {
        W_ASSERT_P(false, "invalid parameter! trace info: %s", _trace_info);
        return FALSE;
    }
    const apr_array_header_t* _header = apr_table_elts(pTable);
    return (const w_table_entry_iterator)_header->elts;
}

char* w_table_get_key(
    _In_ const w_table_entry_iterator pTableEntry,
    _In_ size_t pIndex)
{
    return pTableEntry ? pTableEntry[pIndex].key : NULL;
}

char* w_table_get_value(
    _In_ const w_table_entry_iterator pTableEntry, 
    _In_ size_t pIndex)
{
    return pTableEntry ? pTableEntry[pIndex].val : NULL;
}

size_t w_table_get_key_checksum(
    _In_ const w_table_entry_iterator pTableEntry, 
    _In_ size_t pIndex)
{
    return pTableEntry ? pTableEntry[pIndex].key_checksum : 0;
}
