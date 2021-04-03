#include "w_compress.h"
#include <stdlib.h>
#include <apr.h>
#include <apr_general.h>
#include <apr_strings.h>
#include <apr_tables.h>
#include <apr_file_io.h>
#include <apr_hash.h>
#include <w_log/w_log.h>
#include "lz4/lz4.h"
#include <w_memory/w_string.h>

/*
	NOTE:
	make sure add following define in vrefbuffer.h of msgpack
	iovec is already defined
#ifndef APR_IOVEC_DEFINED
	struct iovec {
		void  *iov_base;
		size_t iov_len;
	};
#endif
*/
#include <w_compression/msgpack/msgpack.h>

#ifdef W_PLATFORM_WIN
#include "lzma/LzmaEnc.h"
#include "lzma/LzmaDec.h"
#endif

typedef struct msgpack
{
	msgpack_sbuffer buf;
	msgpack_packer	pack;
} msgpack;

W_RESULT w_compress_lz4(_In_       const char* pSrcBuffer,
                        _In_       w_compress_mode pMode,
                        _In_       int pAcceleration,
                        _Inout_    w_compress_result* pResult)
{
    if (!pResult || !pSrcBuffer || pResult->size_in == 0) return APR_BADARG;
    
    //allocate size for compressed data
    const int _max_dst_size = LZ4_compressBound((int)(pResult->size_in));
    pResult->data = (char*)malloc(_max_dst_size);
    if (!pResult->data)
    {
        W_ASSERT(false, "allocating memory for compressed buffer. trace info: w_compress_lz4");
        return W_FAILURE;
    }

    int _compressed_buffer_size = -1;
    if (pMode == W_FAST)
    {
#ifdef __clang__
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wstrict-prototypes"
#endif

		_compressed_buffer_size =
			LZ4_compress_fast(
				pSrcBuffer,
				pResult->data,
				(int)pResult->size_in,
				_max_dst_size,
				pAcceleration);
#ifdef __clang__
        #pragma clang diagnostic pop
#endif
    }
    else
    {
        _compressed_buffer_size = LZ4_compress_default(
            pSrcBuffer,
            pResult->data,
            (int)pResult->size_in,
            _max_dst_size);
    }

    if (_compressed_buffer_size > 0)
    {
        //realloc compress_data to free up memory
        pResult->size_out = (size_t)_compressed_buffer_size;
        pResult->data = (char*)realloc(pResult->data, pResult->size_out);
        if (!pResult->data)
        {
            W_ASSERT(false, "could not fit memory of compressed buffer. trace info: compress_buffer_c");
            return W_FAILURE;
        }
    }
    else
    {
        W_ASSERT(false, "could not compress. trace info: compress_buffer_c");
        return W_FAILURE;
    }

    return W_SUCCESS;
}

W_RESULT w_decompress_lz4(_In_z_    const char* pCompressedBuffer,
                          _Inout_   w_compress_result* pDecompressResult)
{
  if (!pCompressedBuffer || pDecompressResult->size_in == 0) return APR_BADARG;

    //allocate memory for decompress
    size_t _destination_capacity = pDecompressResult->size_in * 2;
    pDecompressResult->data = (char*)malloc(_destination_capacity);
    if (!pDecompressResult->data)
    {
         W_ASSERT(false, "error on allocate buffer for decompressed buffer. trace info: w_decompress_lz4");
        return W_FAILURE;
    }

    int _decompressed_size = -1;
    int _number_of_try = 10;
    
    while (_number_of_try > 0)
    {
        _decompressed_size = LZ4_decompress_safe(
                pCompressedBuffer,
                &pDecompressResult->data[0],
                (int)pDecompressResult->size_in,
                (int)_destination_capacity);
        if (_decompressed_size >= 0)
        {
            break;
        }
        else
        {
            _destination_capacity *= 2;
            pDecompressResult->data = (char*)realloc(pDecompressResult->data, _destination_capacity);
            if (!pDecompressResult->data)
            {
                W_ASSERT(false, "could not re-allocate memory of de-compressed buffer. trace info: w_decompress_lz4");
                break;
            }
        }

        _number_of_try--;
    }

    if (_decompressed_size <= 0)
    {
        W_ASSERT(false, "decompress size must be greater than zero. trace info: w_decompress_lz4");
        free(pDecompressResult->data);
        pDecompressResult->data = NULL;
        pDecompressResult->size_out = 0;
        return W_FAILURE;
    }
    else
    {
        pDecompressResult->size_out = _decompressed_size;
        pDecompressResult->data = (char*)realloc(pDecompressResult->data, pDecompressResult->size_out);
        if (!pDecompressResult->data)
        {
             W_ASSERT(false, "could not fit memory for de-compressed buffer. trace info: w_decompress_lz4");
            return W_FAILURE;
        }
    }
    return W_SUCCESS;
}

#ifdef W_PLATFORM_WIN

static void* _lzma_alloc(ISzAllocPtr pPtr, size_t pSize)
{
	return malloc(pSize);
}
static void _lzma_free(ISzAllocPtr pPtr, void* pAddr)
{
	free(pAddr);
}

static ISzAlloc _alloc_funcs = { _lzma_alloc, _lzma_free };

int w_compress_lzma(_In_ const uint8_t* pSourceBuffer, _Inout_ w_compress_result* pCompressInfo)
{
	// set up properties
	CLzmaEncProps _props;
	LzmaEncProps_Init(&_props);

	if (pCompressInfo->size_in >= (1 << 20))
	{
		_props.dictSize = 1 << 20; //1mb dictionary
	}
	else
	{
		_props.dictSize = (UINT32)pCompressInfo->size_in; //smaller dictionary = faster!
	}
	_props.fb = 40;

	// prepare space for the encoded properties
	SizeT _props_size = 5;
	unsigned char _props_encoded[5];

	// allocate some space for the compression output
	// this is way more than necessary in most cases...
	// but better safe than sorry
	//   (a smarter implementation would use a growing buffer,
	//    but this requires a bunch of fuckery that is out of
	///   scope for this simple example)
	SizeT _output_size_64 = pCompressInfo->size_in * 2;
	if (_output_size_64 < 1024)
	{
		_output_size_64 = 1024;
	}

	uint8_t* _compress_mem = (uint8_t*)malloc(_output_size_64 * sizeof(uint8_t));
	int _lzma_status = LzmaEncode(
		_compress_mem,
		&_output_size_64,
		pSourceBuffer,
		pCompressInfo->size_in,
		&_props,
		_props_encoded,
		&_props_size,
		0,
		NULL,
		&_alloc_funcs,
		&_alloc_funcs);

	pCompressInfo->size_out = _output_size_64 + 13;
	if (_lzma_status == SZ_OK)
	{
		// tricky: we have to generate the LZMA header
		// 5 bytes properties + 8 byte uncompressed size
		pCompressInfo->data = (char*)malloc(pCompressInfo->size_out);
		if (pCompressInfo->data && pCompressInfo->size_out >= 5)
		{
			memcpy((unsigned char*)pCompressInfo->data, _props_encoded, 5);
			for (int i = 0; i < 8; i++)
			{
				pCompressInfo->data[5 + i] = (pCompressInfo->size_in >> (i * 8)) & 0xFF;
			}
			if (_compress_mem)
			{
				memcpy((unsigned char*)(pCompressInfo->data + 13), _compress_mem, _output_size_64);
			}
		}
		return 0;
	}
	return 1;
}

int w_decompress_lzma(_In_ const uint8_t* pCompressedBuffer, _Inout_ w_compress_result* pDeCompressInfo)
{
	pDeCompressInfo->size_out = 0;
	if (pDeCompressInfo->size_in < 13)
	{
		return 1; // invalid header!
	}

	// extract the size from the header
	UInt64 _size_from_header = 0;
	for (int i = 0; i < 8; i++)
	{
		_size_from_header |= (UInt64)(pCompressedBuffer[5 + i]) << ((UInt64)i * 8);
	}

	if (_size_from_header <= (256 * 1024 * 1024))
	{
		pDeCompressInfo->data = (char*)malloc(_size_from_header);

		ELzmaStatus _lzma_status;
		SizeT _proc_out_size = _size_from_header, _proc_in_size = pDeCompressInfo->size_in - 13;
		int status = LzmaDecode(
			(uint8_t*)pDeCompressInfo->data,
			&_proc_out_size,
			&pCompressedBuffer[13],
			&_proc_in_size,
			pCompressedBuffer,
			5,
			LZMA_FINISH_END,
			&_lzma_status,
			&_alloc_funcs);

		if (status == SZ_OK && _proc_out_size == _size_from_header)
		{
			pDeCompressInfo->data[_proc_out_size] = '\0';
			pDeCompressInfo->size_out = _proc_out_size;
			return 0;
		}
	}
	return 1;
}

#endif

W_RESULT w_compress_msgpack_init(
	_Inout_ w_mem_pool pMemPool,
	_Inout_ w_msgpack* pMsgPack)
{
	const char* _trace_info = "w_compress_msgpack_init";
	W_RESULT _ret = W_SUCCESS;
	w_msgpack _out_msgpack = NULL;
	msgpack* _private_msgpack = NULL;

	if (!pMemPool)
	{
		W_ASSERT_P(false, "memory pool is invalid. trace info: %s", _trace_info);
		_ret = W_BAD_ARG;
		goto exit;
	}

	_out_msgpack = (w_msgpack)w_malloc(pMemPool, sizeof(w_msgpack_t));
	if (!_out_msgpack)
	{
		W_ASSERT_P(false, "could not allocate memory for w_msgpack_t. trace info: %s", _trace_info);
		_ret = W_FAILURE;
		goto exit;
	}

	_private_msgpack = (msgpack*)w_malloc(pMemPool, sizeof(msgpack));
	if (!_private_msgpack)
	{
		W_ASSERT_P(false, "could not allocate memory for w_msgpack_t. trace info: %s", _trace_info);
		_ret = W_FAILURE;
		goto exit;
	}

	//msgpack::sbuffer is a simple buffer implementation.
	msgpack_sbuffer_init(&_private_msgpack->buf);

	// serialize values into the buffer using msgpack_sbuffer_write callback function.
	msgpack_packer_init(&_private_msgpack->pack, &_private_msgpack->buf, msgpack_sbuffer_write);

	_out_msgpack->msgpack_obj = (void*)_private_msgpack;
	*pMsgPack = _out_msgpack;
exit:
	return _ret;
}

W_RESULT w_compress_msgpack_append_boolean(_Inout_ w_msgpack pMsgPack, _In_ bool pValue)
{
	const char* _trace_info = "w_compress_msgpack_append_boolean";
	W_RESULT _ret = W_SUCCESS;

	if (!pMsgPack || !pMsgPack->msgpack_obj)
	{
		W_ASSERT_P(false, "memory pool is invalid. trace info: %s", _trace_info);
		return (_ret = W_BAD_ARG);
	}

	msgpack* _msgpack = (msgpack*)pMsgPack->msgpack_obj;
	if (_msgpack)
	{
		pValue ?
			msgpack_pack_true(&_msgpack->pack) :
			msgpack_pack_false(&_msgpack->pack);
	}
	else
	{
		_ret = W_FAILURE;
	}

	return _ret;
}

W_RESULT w_compress_msgpack_append_int(_Inout_ w_msgpack pMsgPack, _In_ int pValue)
{
	const char* _trace_info = "w_compress_msgpack_append_int";
	W_RESULT _ret = W_SUCCESS;

	if (!pMsgPack || !pMsgPack->msgpack_obj)
	{
		W_ASSERT_P(false, "memory pool is invalid. trace info: %s", _trace_info);
		return (_ret = W_BAD_ARG);
	}

	msgpack* _msgpack = (msgpack*)pMsgPack->msgpack_obj;
	if (_msgpack)
	{
		msgpack_pack_int(&_msgpack->pack, pValue);
	}
	else
	{
		_ret = W_FAILURE;
	}

	return _ret;
}

W_RESULT w_compress_msgpack_append_float(_Inout_ w_msgpack pMsgPack, _In_ float pValue)
{
	const char* _trace_info = "w_compress_msgpack_append_float";
	W_RESULT _ret = W_SUCCESS;

	if (!pMsgPack || !pMsgPack->msgpack_obj)
	{
		W_ASSERT_P(false, "memory pool is invalid. trace info: %s", _trace_info);
		return (_ret = W_BAD_ARG);
	}

	msgpack* _msgpack = (msgpack*)pMsgPack->msgpack_obj;
	if (_msgpack)
	{
		msgpack_pack_float(&_msgpack->pack, pValue);
	}
	else
	{
		_ret = W_FAILURE;
	}

	return _ret;
}

W_RESULT w_compress_msgpack_append_double(_Inout_ w_msgpack pMsgPack, _In_ double pValue)
{
	const char* _trace_info = "w_compress_msgpack_append_double";

	W_RESULT _ret = W_SUCCESS;

	if (!pMsgPack || !pMsgPack->msgpack_obj)
	{
		W_ASSERT_P(false, "memory pool is invalid. trace info: %s", _trace_info);
		return (_ret = W_BAD_ARG);
	}

	msgpack* _msgpack = (msgpack*)pMsgPack->msgpack_obj;
	if (_msgpack)
	{
		msgpack_pack_double(&_msgpack->pack, pValue);
	}
	else
	{
		_ret = W_FAILURE;
	}

	return _ret;
}

W_RESULT w_compress_msgpack_append_string(_Inout_ w_msgpack pMsgPack, _In_ const char* pString, _In_ size_t pStringLen)
{
	const char* _trace_info = "w_compress_msgpack_append_string";

	W_RESULT _ret = W_SUCCESS;

	if (!pMsgPack || !pMsgPack->msgpack_obj)
	{
		W_ASSERT_P(false, "memory pool is invalid. trace info: %s", _trace_info);
		return (_ret = W_BAD_ARG);
	}

	msgpack* _msgpack = (msgpack*)pMsgPack->msgpack_obj;
	if (_msgpack)
	{
		msgpack_pack_str(&_msgpack->pack, pStringLen);
		msgpack_pack_str_body(&_msgpack->pack, pString, pStringLen);
	}
	else
	{
		_ret = W_FAILURE;
	}

	return _ret;
}

W_RESULT w_compress_msgpack_append_array(
	_Inout_ w_msgpack pMsgPack, 
	_In_ w_array pArray, 
	_In_ w_std_types pArrayElementType)
{
	const char* _trace_info = "w_compress_msgpack_append_array";

	W_RESULT _ret = W_SUCCESS;

	if (!pMsgPack || !pMsgPack->msgpack_obj || !pArray)
	{
		W_ASSERT_P(false, "invalid parameters! trace info: %s", _trace_info);
		return (_ret = W_BAD_ARG);
	}
	
	msgpack* _msgpack = (msgpack*)pMsgPack->msgpack_obj;
	if (!_msgpack)
	{
		W_ASSERT_P(false, "missing msgpack_obj! trace info: %s", _trace_info);
		return (_ret = W_BAD_ARG);
	}

	size_t _size = w_array_get_size(pArray);
	if (!_size)
	{
		W_ASSERT_P(false, "array is empty! trace info: %s", _trace_info);
		return (_ret = W_BAD_ARG);
	}

	msgpack_pack_array(&_msgpack->pack, _size);
	switch (pArrayElementType)
	{
	default:
	{
		_ret = W_FAILURE;
		break;
	}
	case W_TYPE_BOOLEAN:
	{
		for (size_t i = 0; i < _size; ++i)
		{
			bool* _ptr = (bool*)w_array_get_element(pArray, (int)i);
			if (_ptr)
			{
				(*_ptr) ?
					msgpack_pack_true(&_msgpack->pack) :
					msgpack_pack_false(&_msgpack->pack);
			}
		}
		break;
	}
	case W_TYPE_POSITIVE_INT:
	case W_TYPE_NEGATIVE_INT:
	{
		for (size_t i = 0; i < _size; ++i)
		{
			int* _ptr = (int*)w_array_get_element(pArray, (int)i);
			if (_ptr)
			{
				msgpack_pack_int(&_msgpack->pack, *_ptr);
			}
		}
		break;
	}
	case W_TYPE_FLOAT:
	{
		for (size_t i = 0; i < _size; ++i)
		{
			float* _ptr = (float*)w_array_get_element(pArray, (int)i);
			if (_ptr)
			{
				msgpack_pack_float(&_msgpack->pack, *_ptr);
			}
		}
		break;
	}
	case W_TYPE_DOUBLE:
	{
		for (size_t i = 0; i < _size; ++i)
		{
			double* _ptr = (double*)w_array_get_element(pArray, (int)i);
			if (_ptr)
			{
				msgpack_pack_double(&_msgpack->pack, *_ptr);
			}
		}
		break;
	}
	case W_TYPE_STRING:
	{
		for (size_t i = 0; i < _size; ++i)
		{
			const char* _ptr = (const char*)w_array_get_element(pArray, (int)i);
			if (_ptr)
			{
				size_t _size = strlen(_ptr);
				msgpack_pack_str(&_msgpack->pack, _size);
				msgpack_pack_str_body(&_msgpack->pack, _ptr, _size);
			}
		}
		break;
	}
	}

	return _ret;
}

W_RESULT w_compress_msgpack_fini(
	_Inout_ w_mem_pool pMemPool,
	_Inout_ w_msgpack pMsgPack,
	_Inout_ w_buffer* pBuffer)
{
	const char* _trace_info = "w_compress_msgpack_fini";

	W_RESULT _ret = W_SUCCESS;
	*pBuffer = NULL;

	if (!pMemPool || !pMsgPack || !pMsgPack->msgpack_obj)
	{
		W_ASSERT_P(false, "invalid parameters! trace info: %s", _trace_info);
		return (_ret = W_BAD_ARG);
	}

	msgpack* _msgpack = (msgpack*)pMsgPack->msgpack_obj;
	if (!_msgpack)
	{
		W_ASSERT_P(false, "missing msgpack_obj! trace info: %s", _trace_info);
		return (_ret = W_BAD_ARG);
	}
	
	w_buffer _buffer = (w_buffer)w_malloc(pMemPool, sizeof(w_buffer_t));
	if (_buffer)
	{
		_buffer->len = _msgpack->buf.size;
		_buffer->data = w_malloc(pMemPool, _buffer->len);
		if (_buffer->data)
		{
#if defined(W_PLATFORM_UNIX)
            memcpy(_buffer->data, _msgpack->buf.data, _msgpack->buf.size);
#else
            memcpy_s(_buffer->data, _buffer->len, _msgpack->buf.data, _msgpack->buf.size);
#endif
			*pBuffer = _buffer;
		}
		else
		{
			_ret = W_FAILURE;
		}
	}
	else
	{
		_ret = W_FAILURE;
	}

	msgpack_sbuffer_destroy(&_msgpack->buf);

	return _ret;
}

static void s_send_msgpack_object(
	_Inout_ w_mem_pool pMemPool,
	_In_ msgpack_object* pObj,
	_In_ w_unpack_msg_fn pMsgUnPackFunction)
{
	switch (pObj->type)
	{
	case MSGPACK_OBJECT_NIL:
	{
		pMsgUnPackFunction(W_TYPE_NULL, NULL);
		break;
	}
	case MSGPACK_OBJECT_BOOLEAN:
	{
		bool _v = pObj->via.boolean;
		pMsgUnPackFunction(W_TYPE_BOOLEAN, (const void*)(&_v));
		break;
	}
	case MSGPACK_OBJECT_POSITIVE_INTEGER:
	case MSGPACK_OBJECT_NEGATIVE_INTEGER:
	{
		int _v = pObj->via.i64;
		pMsgUnPackFunction((w_std_types)pObj->type, (const void*)(&_v));
		break;
	}
	case MSGPACK_OBJECT_FLOAT32:
	{
		float _v = (float)pObj->via.f64;
		pMsgUnPackFunction(W_TYPE_FLOAT, (const void*)(&_v));
		break;
	}
	case MSGPACK_OBJECT_FLOAT64:
	{
		double _v = (double)pObj->via.f64;
		pMsgUnPackFunction(W_TYPE_DOUBLE, (const void*)(&_v));
		break;
	}
	case MSGPACK_OBJECT_STR:
	{
		w_string _str = (w_string)w_malloc(pMemPool, sizeof(w_string_t));
		if (_str)
		{
			_str->str_len = pObj->via.str.size;
			_str->reserved_size = _str->str_len + 1; //plus '\0'
			_str->data = (char*)w_malloc(pMemPool, _str->reserved_size);
			if (_str->data)
			{
#if defined(W_PLATFORM_UNIX)
                memcpy(
                    _str->data,
                    pObj->via.str.ptr,
                    pObj->via.str.size);
#else
				memcpy_s(
					_str->data,
					_str->str_len,
					pObj->via.str.ptr,
					pObj->via.str.size);
#endif
				_str->data[_str->str_len] = '\0';
				pMsgUnPackFunction(W_TYPE_STRING, _str);
				break;
			}
		}
		pMsgUnPackFunction(W_TYPE_STRING, NULL);
		break;
	}
	case MSGPACK_OBJECT_ARRAY:
	{
		pMsgUnPackFunction(W_TYPE_ARRAY_BEGIN, NULL);
		for (size_t i = 0; i < pObj->via.array.size; ++i)
		{
			s_send_msgpack_object(
				pMemPool,
				&(pObj->via.array.ptr[i]),
				pMsgUnPackFunction);
		}
		pMsgUnPackFunction(W_TYPE_ARRAY_END, NULL);
		break;
	}
	case MSGPACK_OBJECT_MAP:
	{
		//NOT IMPLEMENTED YET
		break;
	}
	}
}

W_RESULT w_decompress_msgpack(
	_Inout_ w_mem_pool pMemPool,
	_Inout_ w_buffer pBuffer,
	_In_ w_unpack_msg_fn pMsgUnPackFunction)
{
	msgpack_unpack_return _ret;
	size_t _unpack_pos = 0;

	msgpack_unpacked _unpack;
	msgpack_unpacked_init(&_unpack);

	for (;;)
	{
		_ret = msgpack_unpack_next(
			&_unpack,
			pBuffer->data,
			pBuffer->len,
			&_unpack_pos);
		if (_ret != MSGPACK_UNPACK_SUCCESS)
		{
			//send NULL for the last object
			pMsgUnPackFunction(W_TYPE_NULL, NULL);
			break;
		}
		s_send_msgpack_object(pMemPool, &_unpack.data, pMsgUnPackFunction);
	}

	msgpack_unpacked_destroy(&_unpack);

	return W_SUCCESS;
}

//W_RESULT w_msgunpack()
//{
//
//}

/*

		static W_RESULT decompress_lz4(
			_In_	const char* pCompressedBuffer,
			_Inout_	w_compress_result* pDecompressInfo)
		{
			if (!pDecompressInfo || !pCompressedBuffer) return W_RESULT::W_INVALIDARG;

			W_RESULT _result = W_PASSED;

			auto _err_log = (char*)malloc(256 * sizeof(char));
			if (decompress_buffer_c(
				pCompressedBuffer,
				pDecompressInfo,
				_err_log))
			{
				logger.error(_err_log);
				_result = W_FAILED;
			}
			free(_err_log);

			return _result;
		}

#ifdef __WIN32
		static W_RESULT compress_lzma(
			_In_ const uint8_t* pSrcBuffer,
			_Inout_ w_compress_result* pCompressResult)
		{
			if (!pCompressResult || !pSrcBuffer) return W_RESULT::W_INVALIDARG;

			W_RESULT _result = W_PASSED;

			if (w_compress_lzma::compress(
				pSrcBuffer,
				pCompressResult))
			{
				_result = W_FAILED;
			}

			return _result;
		}

		static W_RESULT decompress_lzma(
			_In_ const uint8_t* pCompressedBuffer,
			_Inout_ w_compress_result* pDecompressInfo)
		{
			if (!pDecompressInfo || !pCompressedBuffer) return W_RESULT::W_INVALIDARG;

			W_RESULT _result = W_PASSED;

			if (w_compress_lzma::decompress(
				pCompressedBuffer,
				pDecompressInfo))
			{
				_result = W_FAILED;
			}

			return _result;
		}
#endif

		static w_compress_result compress_file(_In_ FILE* pFileStreamIn, _Inout_ FILE* pCompressedFileOut)
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
		}
	};
}
*/
