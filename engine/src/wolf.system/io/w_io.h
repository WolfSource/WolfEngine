/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : w_io.h
    Description      : This class is responsible to managing IO functions
    Comment          :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "wolf.h"
#include <apr-1/apr_file_info.h>
#include <apr-1/apr_file_io.h>

enum base_64_mode { chromium, klomp_avx, fast_avx, fast_avx512, quick_time, scalar };

/**
 * create a file
 * @param pPath path of file
 * @param pContent content of file
 * @param pBinaryMode open file in binary mode (will be ignored on UNIX)
 * @param pBufferedMode open file in buffered mode
 * @param pNoneBlockMode open file in none block mode
 * @param pMultiThreadedMode allow multiple threads to use the file
 * @param pOpenAppendMode append to existing file
 * @param pIsLargFile is larg file
 * @param pErrorIfFileExists error whether file exists
 * @return file
*/
w_file w_io_create_file(_In_z_  const char* pPath,
                             _In_z_  const char* pContent,
                             _In_    const bool pBinaryMode,
                             _In_    const bool pBufferedMode,
                             _In_    const bool pNoneBlockMode,
                             _In_    const bool pMultiThreadedMode,
                             _In_    const bool pOpenAppendMode,
                             _In_    const bool pIsLargFile,
                             _In_    const bool pErrorIfFileExists);

/**
 * create and save content to file
 * @param pPath path of file
 * @param pContent content of file
 * @param pBinaryMode open file in binary mode (will be ignored on UNIX)
 * @param pBufferedMode open file in buffered mode
 * @param pNoneBlockMode open file in none block mode
 * @param pMultiThreadedMode allow multiple threads to use the file
 * @param pOpenAppendMode append to existing file
 * @param pIsLargFile is larg file
 * @param pErrorIfFileExists error whether file exists
 * @return result
*/
W_RESULT w_io_save_to_file(_In_z_  const char* pPath,
                           _In_z_  const char* pContent,
                           _In_    const bool pBinaryMode,
                           _In_    const bool pBufferedMode,
                           _In_    const bool pNoneBlockMode,
                           _In_    const bool pMultiThreadedMode,
                           _In_    const bool pOpenAppendMode,
                           _In_    const bool pIsLargFile,
                           _In_    const bool pErrorIfFileExists);

/**
 * check whether this path belongs to a file
 * @param pPath path of file
 * @return result
*/
W_RESULT w_io_get_is_file(_In_z_ const char* pPath);

/**
 * get file info
 * @param pPath path of file
 * @return file info
*/
w_file_info w_io_get_file_info_from_path(_In_z_ const char* pPath);

/**
 * get file info
 * @param pFile opened file
 * @return file info
*/
w_file_info w_io_get_file_info(_In_ w_file pFile);

/**
 * get file extension from file path
 * @param pFilePath path to the file
 * @return extension of file
*/
const char* w_io_get_file_extension_from_path(_In_z_ const char* pFilePath);

/**
 * get file extension from file
 * @param pFile path to the file
 * @return extension of file
*/
const char* w_io_get_file_extension(_In_ w_file pFile);

/**
 * get file name from file path
 * @param pFilePath path to the file
 * @return name of file
*/
const char* w_io_get_file_name_from_path(_In_z_ const char* pFilePath);

/**
 * get file name from file
 * @param pFile path to the file
 * @return name of file
*/
const char* w_io_get_file_name(_In_ w_file pFile);

/**
 * pick off base name in filename before dot(without extension) from path
 * @param pPath path of file
 * @return base file name
*/
const char* w_io_get_base_file_name_from_path(_In_z_ const char* pPath);

/**
 * pick off base name in filename before dot(without extension) from file
 * @param pFile path of file
 * @return base file name
*/
const char* w_io_get_base_file_name(_In_ w_file pFile);


/**
 * read a file with given path
 * @param pPath path of file
 * @return content of file
*/
void* w_io_read_file_with_path(_In_z_ const char* pPath);

/**
 * read a file
 * @param pFile file
 * @return content of file
*/
void* w_io_read_file(_In_z_ w_file pFile);

/**
 * delete file with path
 * @param pPath path of file
 * @return result
*/
W_RESULT w_io_delete_file_with_path(_In_z_ const char* pPath);

/**
 * delete file
 * @param pFile file which must be deleted
 * @return result
*/
W_RESULT w_io_delete_file(_In_ w_file pFile);

/**
 * get current directory in char
 * @return current directory path
*/
char* w_io_get_current_directory(void);

/**
 * check whether this path belongs to a directory
 * @param pPath path of directory
 * @return result
*/
W_RESULT w_io_get_is_directory(_In_z_ const char* pPath);

/**
 * get parent directory of given path
 * @param pPath path of directory
 * @return parent directory
*/
const char* w_io_get_parent_directory(_In_z_ const char* pPath);

/**
 * create a directory
 * @param pPath path of directory
 * @return result
*/
W_RESULT w_io_create_directory(_In_z_ const char* pPath);

/**
 * convert utf8 char* to wchar_t* 
 * @param pIn char* inputs,
 * @param pInLen lenght of inputs,
 * @param pOut outputs,
 * @param pOutLen lenght of output,
 * @return result
*/
W_RESULT w_io_utf8_to_ucs2(
	_In_	char* pIn,
	_In_	size_t* pInLen,
	_Out_	uint16_t* pOut,
	_Out_	size_t* pOutLen);

/**
 * convert wchar_t* to utf8 char*
 * @param pIn wchar_t* inputs,
 * @param pInLen lenght of inputs,
 * @param pOut outputs,
 * @param pOutLen lenght of output,
 * @return result
*/
W_RESULT w_io_ucs2_to_utf8(
	_In_	uint16_t* pIn,
	_In_	size_t* pInLen,
	_Out_	char* pOut,
	_Out_	size_t* pOutLen);

/**
 * convert hex string to decimal
 * @param pHexStr input string,
 * @return result decimal
*/
long w_io_to_hex(const char* pHexStr);

/**
 * check "pString" string starts with "pStartWith" string
 * @param pString input string,
 * @param pStartWith start with,
 * @return result
*/
W_RESULT w_io_has_string_start_with(_In_z_ const char* pString, _In_z_ const char* pStartWith);

/**
 * check "pString" string starts with "pStartWith" string
 * @param pString input string,
 * @param pStartWith start with,
 * @return result
*/
W_RESULT w_io_has_wstring_start_with(_In_z_ const wchar_t* pString, _In_z_ const wchar_t* pStartWith);

/**
 * check "pString" string ends with "pEndWith" string
 * @param pString input string
 * @param pEndWith end eith
 * @return result decimal
*/
W_RESULT w_io_has_string_end_with(_In_z_ const char* pString, _In_z_ const char* pEndWith);

/**
 * check "pString" string ends with "pEndWith" string
 * @param pString input string
 * @param pEndWith end eith
 * @return result decimal
*/
W_RESULT w_io_has_wstring_end_with(_In_z_ const wchar_t* pString, _In_z_ const wchar_t* pEndWith);

/**
 * split string and returns all possible splits
 * @param pString input string
 * @param pSplit split strings
 * @param pResults split strings
 * @returns W_SUCCESS when function successed
 * @returns W_FAILURE when function failed
*/
W_RESULT w_io_split_string(_In_z_ char* pString,
                           _In_z_ const char* pSplit,
                           _Out_ w_array* pResults);

/**
 * convert to base64
 * @param pDestinationBuffer destination buffer
 * @param pSourceBuffer source buffer
 * @param pSourceBufferLenght lenght of source buffer
 * @param pEncodeMode typed of encode algorithm
 * @return size of destination buffer
*/
size_t w_io_to_base_64(
    _Inout_z_ char** pDestinationBuffer,
    _In_z_ char* pSourceBuffer,
    _In_z_ const size_t pSourceBufferLenght,
    _In_ const enum base_64_mode pEncodeMode);


#ifdef __cplusplus
}
#endif

