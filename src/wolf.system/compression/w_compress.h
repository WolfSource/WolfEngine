/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_compress.hpp
	Description		 : compress stream based on https://github.com/lz4/lz4
	Comment          :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "wolf.h"
#include "os/w_platform.h"
#include "memory/w_array.h"

	typedef enum
	{
		W_DEFAULT,
		W_FAST
	}
	w_compress_mode;

	typedef struct w_compress_result
	{
		size_t                size_in;
		size_t                size_out;
		char* data;
	} w_compress_result;

	typedef struct w_msgpack_t
	{
		void*				msgpack_obj;
	} w_msgpack_t;
	typedef w_msgpack_t* w_msgpack;

	typedef void (*w_unpack_msg_fn)(w_std_types /*pDataType*/, const void* /*pValue*/);

	/**
	 * compress stream using lz4 algorithm
	 * @param pSrcBuffer is the source buffer
	 * @param pCompressResult is the result of compress process
	 * @param pMode can be W_DEFAULT or W_FAST
	 * @param pAcceleration is the acceleration of process. the default value is 1
	 * @return result
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_compress_lz4(_In_    const char* pSrcBuffer,
			_In_	w_compress_mode pMode,
			_In_	int pAcceleration,
			_Inout_ w_compress_result* pCompressResult);

	/**
	 * compress stream using lz4 algorithm
	 * @param pCompressedBuffer is the compressed source buffer
	 * @param pDecompressInfo is the result of decompressed buffer
	 * @return result
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_decompress_lz4(
			_In_z_	const char* pCompressedBuffer,
			_Inout_	w_compress_result* pDecompressInfo);

#ifdef W_PLATFORM_WIN

	/**
	 * compress stream using lzma algorithm
	 * @param pSrcBuffer is the source buffer
	 * @param pCompressResult is the result of compress process
	 * @return result
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_compress_lzma(
			_In_ const uint8_t* pSrcBuffer,
			_Inout_ w_compress_result* pCompressResult);

	/**
	* decompress stream using lzma algorithm
	* @param pCompressedBuffer is the compressed source buffer
	* @param pDecompressInfo is the result of decompressed buffer
	* @return result
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_decompress_lzma(
			_In_ const uint8_t* pCompressedBuffer,
			_Inout_ w_compress_result* pDecompressInfo);
#endif

	/**
	 * initialize msgpack
	 * @param pMemPool a memory pool to allocate msgpack
	 * @param pMsgPack a pointer to msgpack object
	 * @return result
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_compress_msgpack_init(
			_Inout_ w_mem_pool pMemPool,
			_Inout_ w_msgpack* pMsgPack);

	/**
	 * append a boolean to msgpack
	 * @param pMsgPack a pointer to msgpack object
	 * @param pValue the boolean value
	 * @return result
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_compress_msgpack_append_boolean(
			_Inout_ w_msgpack pMsgPack,
			_In_ bool pValue);

	/**
	 * append an integer to msgpack
	 * @param pMsgPack a pointer to msgpack object
	 * @param pValue the integer value
	 * @return result
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_compress_msgpack_append_int(
			_Inout_ w_msgpack pMsgPack,
			_In_ int pValue);

	/**
	 * append a float to msgpack
	 * @param pMsgPack a pointer to msgpack object
	 * @param pValue the float value
	 * @return result
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_compress_msgpack_append_float(
			_Inout_ w_msgpack pMsgPack,
			_In_ float pValue);

	/**
	 * append a double to msgpack
	 * @param pMsgPack a pointer to msgpack object
	 * @param pValue the double value
	 * @return result
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_compress_msgpack_append_double(
			_Inout_ w_msgpack pMsgPack,
			_In_ double pValue);

	/**
	 * append a string to msgpack
	 * @param pMsgPack a pointer to msgpack object
	 * @param pString the string value
	 * @param pStringLen the length of string
	 * @return result
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_compress_msgpack_append_string(
			_Inout_ w_msgpack pMsgPack,
			_In_ const char* pString,
			_In_ size_t pStringLen);	

	/**
	 *  append an array to msgpack
	 * @param pMsgPack a pointer to msgpack object
	 * @param pArray the array object 
	 * @param pArrayElementType the element's type of array
	 * @return result
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_compress_msgpack_append_array(
			_Inout_ w_msgpack pMsgPack,
			_In_ w_array pArray,
			_In_ w_std_types pArrayElementType);

	/**
	 * finitialize the msgpack. release all resources of msgpack and 
	 * make a copy of final data which will be avaiable in w_buffer
	 * @param pMemPool the pool which will be used for allocating w_buffer
	 * @param pMsgPack the msgpack object 
	 * @param pBuffer the output buffer
	 * @return result
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_compress_msgpack_fini(
			_Inout_ w_mem_pool pMemPool,
			_Inout_ w_msgpack pMsgPack,
			_Inout_ w_buffer* pBuffer);

	/**
	 * unpack the msgpacked data. the last object is W_TYPE_NULL
	 * @param pMemPool the pool which will be used for allocating data
	 * @param pBuffer the buffer that contains msgpack data
	 * @return result
	*/
	W_SYSTEM_EXPORT
		W_RESULT w_decompress_msgpack(
			_Inout_ w_mem_pool pMemPool,
			_Inout_ w_buffer pBuffer,
			_In_ w_unpack_msg_fn pMsgUnPackFunction);

	///**
	//* unpack string message using msgpack
	//* @return result
	//*/
	//W_SYSTEM_EXPORT
	//	W_RESULT w_msgunpack();

	//TODO
			/*static w_compress_result compress_file(_In_ FILE* pFileStreamIn, _Inout_ FILE* pCompressedFileOut)
			{
				auto _err_log = (char*)malloc(256 * sizeof(char));
				auto _result = compress_file_c(pFileStreamIn, pCompressedFileOut, _err_log);
				if (_result.error)
				{
					logger.error(_err_log);
				}
				free(_err_log);

				if (pFileStreamIn) fclose(pFileStreamIn);
				if (pCompressedFileOut) fclose(pCompressedFileOut);

				return _result;
			}
			static w_compress_result decompress_file(_In_ FILE* pCompressedFileIn, _Inout_ FILE* pFileStreamOut)
			{
				auto _err_log = (char*)malloc(256 * sizeof(char));
				auto _result = decompress_file_c(pCompressedFileIn, pFileStreamOut, _err_log);
				if (_result.error)
				{
					logger.error(_err_log);
				}
				free(_err_log);

				if (pCompressedFileIn) fclose(pCompressedFileIn);
				if (pFileStreamOut) fclose(pFileStreamOut);

				return _result;
			}*/


#ifdef __cplusplus
}
#endif
