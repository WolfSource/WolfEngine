#pragma once

#include "../w_test/base.hpp"

#include "w_compress.h"
#include "w_memory/w_string.h"

TEST_CASE("w_compression")
{
	w_mem_pool _mem_pool = w_init();

	const char* _buffer = "xxxxxxxxxxxxxxxxxxxxthis_is_playpod_xyz_123_789_XXXYZxxxxxxxxxxxxxxxxxxxx";
	size_t _buffer_size = strlen(_buffer);

	w_compress_result _compress_lz4 = { 0 };
	_compress_lz4.size_in = _buffer_size;
	W_RESULT _ret = w_compress_lz4(_buffer, W_FAST, 1, &_compress_lz4);
	REQUIRE(_ret == W_SUCCESS);

	w_compress_result _decompress_lz4 = { 0 };
	_decompress_lz4.size_in = _compress_lz4.size_out;
	if (_compress_lz4.data)
	{
		_ret = w_decompress_lz4(_compress_lz4.data, &_decompress_lz4);
		REQUIRE(_ret == W_SUCCESS);
	}

	REQUIRE(_compress_lz4.size_in == _decompress_lz4.size_out);

#ifdef W_PLATFORM_WIN

	w_compress_result _compress_lzma = { 0 };
	_compress_lzma.size_in = _buffer_size;
	_ret = w_compress_lzma((const uint8_t*)_buffer, &_compress_lzma);
	REQUIRE(_ret == W_SUCCESS);

	w_compress_result _decompress_lzma = { 0 };
	_decompress_lzma.size_in = _compress_lzma.size_out;
	if (_compress_lzma.data)
	{
		_ret = w_decompress_lzma((const uint8_t*)_compress_lzma.data, &_decompress_lzma);
		REQUIRE(_ret == W_SUCCESS);
	}

	REQUIRE(_compress_lzma.size_in == _decompress_lzma.size_out);

#endif

	//command
	w_buffer _msg_buffer = nullptr;
	w_msgpack _msg_pack = nullptr;
	_ret = w_compress_msgpack_init(_mem_pool, &_msg_pack);
	REQUIRE(_ret == W_SUCCESS);

	//pack boolean
	_ret = w_compress_msgpack_append_boolean(_msg_pack, true);
	REQUIRE(_ret == W_SUCCESS);

	//pack boolean
	_ret = w_compress_msgpack_append_boolean(_msg_pack, false);
	REQUIRE(_ret == W_SUCCESS);

	//pack int
	_ret = w_compress_msgpack_append_int(_msg_pack, 7);
	REQUIRE(_ret == W_SUCCESS);

	//pack float
	_ret = w_compress_msgpack_append_float(_msg_pack, (float)17.0);
	REQUIRE(_ret == W_SUCCESS);

	//pack double
	_ret = w_compress_msgpack_append_double(_msg_pack, 77.0);
	REQUIRE(_ret == W_SUCCESS);

	//pack string
	const char* _str = "this is test";
	_ret = w_compress_msgpack_append_string(_msg_pack, _str, strlen(_str));
	REQUIRE(_ret == W_SUCCESS);

	//pack array
	int _a[3] = { 1,2,3 };
	w_array _array = w_array_init(_mem_pool, 3, sizeof(const int*));
	w_array_append(_array, &_a[0]);
	w_array_append(_array, &_a[1]);
	w_array_append(_array, &_a[2]);

	_ret = w_compress_msgpack_append_array(
		_msg_pack,
		_array,
		w_std_types::W_TYPE_POSITIVE_INT);
	REQUIRE(_ret == W_SUCCESS);

	//the result will be availabe in _msg_buffer
	_ret = w_compress_msgpack_fini(_mem_pool, _msg_pack, &_msg_buffer);
	REQUIRE(_ret == W_SUCCESS);

	//let's decompress _msg_buffer
	_ret = w_decompress_msgpack(_mem_pool, _msg_buffer, 
		[](w_std_types pDataType, const void* pValue)
		{
			switch (pDataType)
			{
			default:
			{
				std::cout << "undefined data type" << std::endl;
				break;
			}
			case w_std_types::W_TYPE_NULL:
			{
				std::cout << "all data have been unpacked" << std::endl;
				break;
			}
			case w_std_types::W_TYPE_BOOLEAN:
			{
				auto _value = (const bool*)pValue;
				if (*_value)
				{
					std::cout << "type is W_TYPE_BOOLEAN and value is TRUE" << std::endl;
				}
				else
				{
					std::cout << "type is W_TYPE_BOOLEAN and value is FALSE" << std::endl;
				}
				break;
			}
			case w_std_types::W_TYPE_POSITIVE_INT:
			case w_std_types::W_TYPE_NEGATIVE_INT:
			{
				auto _value = (const int*)pValue;
				if (*_value)
				{
					if (*_value >= 0)
					{
						std::cout <<
							"type is W_TYPE_POSITIVE_INT and value is " <<
							*_value <<
							std::endl;
					}
					else
					{
						std::cout <<
							"type is W_TYPE_NEGATIVE_INT and value is " <<
							*_value <<
							std::endl;
					}
				}
				break;
			}
			case w_std_types::W_TYPE_FLOAT:
			{
				auto _value = (const float*)pValue;
				if (*_value)
				{
					std::cout <<
						"type is W_TYPE_FLOAT and value is " <<
						*_value <<
						std::endl;
				}
				break;
			}
			case w_std_types::W_TYPE_DOUBLE:
			{
				auto _value = (const double*)pValue;
				if (*_value)
				{
					std::cout <<
						"type is W_TYPE_DOUBLE and value is " <<
						*_value <<
						std::endl;
				}
				break;
			}
			case w_std_types::W_TYPE_STRING:
			{
				auto _value = (w_string)pValue;
				if (_value)
				{
					std::cout <<
						"type is W_TYPE_STRING and value is " <<
						_value->data <<
						std::endl;
				}
				break;
			}
			case w_std_types::W_TYPE_ARRAY_BEGIN:
			{
				std::cout << "Begin of ARRAY " << std::endl;
				break;
			}
			case w_std_types::W_TYPE_ARRAY_END:
			{
				std::cout << "End of ARRAY " << std::endl;
				break;
			}
			}
		});
	REQUIRE(_ret == W_SUCCESS);

	w_fini(&_mem_pool);
}
