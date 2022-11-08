/*
        Project: Wolf Engine. Copyright © 2014-2022 Pooya Eimandar
        https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <wolf.h>

	/**
	  * compress stream using lzma algorithm
	  * @param p_dst is the destination buffer
	  * @param p_dst_len is length of the destination buffer
	  * @param p_src is the source buffer
	  * @param p_src_len is length of the source buffer
	  * @param p_type type of LZMA algorithm (0 = LZMA1 and 1 = LZMA2)
	  * @param p_level 0 <= level <= 9
	  * @return compressed memory
	 */
	//W_API 
	//	W_RESULT compress(
	//		uint8_t** p_dst,
	//		size_t* p_dst_len,
	//		const uint8_t* p_src,
	//		const size_t p_src_len,
	//		uint8_t p_type,
	//		uint32_t p_level);

#ifdef __cplusplus
}
#endif