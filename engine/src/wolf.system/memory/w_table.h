/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : w_table.h
    Description      : table with key and value. This table can only store text data.
    Comment          :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "wolf.h"
#include <stdarg.h>

    //forward declaration
    typedef struct apr_table_t* w_table;
    typedef struct apr_table_entry_t* w_table_entry_iterator;
    typedef int (w_table_do_callback)(void* /*rec*/, const char* /*pKey*/, const char* /*pValue*/);

    /*
     * Make a new table.
     * @param pMemPool The pool to allocate out of
     * @param pTable The new table
     * @param pInitSize The number of elements in the initial table.
     * @param pMemPool The memory pool to allocate. set NULL to use default pool.
     * @return result
     * @warning This table can only store text data
     */
    W_SYSTEM_EXPORT
        W_RESULT    w_table_init(
            _Inout_ w_mem_pool pMemPool,
            _Inout_ w_table* pTable,
            _In_ size_t pInitSize);

    /*
     * Add a key/value pair to a table. this will overwrite the old data
     * @param pTable , The table to add the data.
     * @param pKey , The key to use (case does not matter)
     * @param pValue , The value to add.
     * @param pMakeACopy true means when adding data, this table makes a copy of both the key and the value.
      * @return result
     */
    W_SYSTEM_EXPORT
        W_RESULT    w_table_set(_In_ w_table pTable, char* pKey, char* pValue, bool pMakeACopy);

    /**
     * Remove data from the table.
     * @param pTable , The table to remove data from
     * @param pKey , The key of the data being removed (case does not matter)
     * @return result
     */
    W_SYSTEM_EXPORT
        W_RESULT    w_table_unset(_In_ w_table pTable, char* pKey);

    /**
    * Add data to a table, regardless of whether there is another element with the same key.
    * @param pTable , The table to add to
    * @param pKey , The key to use
    * @param pValue , The value to add.
    * @return result
    * @remark When adding data, this function makes a copy of both the key and the value.
    */
    W_SYSTEM_EXPORT
        W_RESULT    w_table_add(_In_ w_table pTable, char* pKey, char* pValue);

    /**
    * get the number of active elements in the table
    * @param pTable , The table to get size
    * @return result
    */
    W_SYSTEM_EXPORT
        int         w_table_get_size(_In_ w_table pTable);

    /**
    * check table is empty
    * @param pTable , The table to check
    * @return result
    */
    W_SYSTEM_EXPORT
        int         w_table_is_empty(_In_ w_table pTable);

    /**
    * clear the table
    */
    W_SYSTEM_EXPORT
        void        w_table_clear(_In_ w_table pTable);

    /**
    * Iterate over a table running the provided function once for every element in the table.
    * @param pTable , The table to iterate over
    * @param pCallBack , The function to run
    * @param pArg , The data to pass as the first argument to the function
    * @return FALSE if one of the comp() iterations returned zero; TRUE if all iterations returned non-zero
    */
    W_SYSTEM_EXPORT
        int         w_table_do(
            _In_ w_table pTable,
            _In_ w_table_do_callback pCallBack,
            _In_ void* pArg);

    /**
    * Iterate over a table running the provided function once for selected elements in the table.
    * @param pTable , The table to iterate over
    * @param pCallBack , The function to run
    * @param pArg , The data to pass as the first argument to the function
    * @param pKeys keys to filter (char*)
    * @return FALSE if one of the comp() iterations returned zero; TRUE if all iterations returned non-zero
    */
    W_SYSTEM_EXPORT
        int         w_table_do_with_filter(
            _In_ w_table pTable,
            _In_ w_table_do_callback pCallBack,
            _In_ void* pArg,
            _In_ va_list pKeys);

    /**
    * get first entry of table's iterator
    * @param pTable , The table to iterate over
    * @return table entry iterator
    */
    W_SYSTEM_EXPORT
        const w_table_entry_iterator    w_table_get_entry(_In_ w_table pTable);

    /**
    * get key by index
    * @param pTableEntry , The table entry iterator
    * @param pIndex , The index of pair
    * @return key
    */
    W_SYSTEM_EXPORT
        char* w_table_get_key(
            _In_ const w_table_entry_iterator pTableEntry,
            _In_ size_t pIndex);

    /**
    * get value by index
    * @param pTableEntry , The table entry iterator
    * @param pIndex , The index of pair
    * @return value
    */
    W_SYSTEM_EXPORT
        char* w_table_get_value(
            _In_ const w_table_entry_iterator pTableEntry,
            _In_ size_t pIndex);

    /**
    * get key checksum
    * @param pTableEntry , The table entry iterator
    * @param pIndex , The index of pair
    * @return key check sum
    */
    W_SYSTEM_EXPORT
        size_t          w_table_get_key_checksum(_In_ const w_table_entry_iterator pTableEntry, _In_ size_t pIndex);

#ifdef __cplusplus
}
#endif
