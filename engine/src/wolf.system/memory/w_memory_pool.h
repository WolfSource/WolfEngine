/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_memory_pool based intel tbb fixed memory pool
	Description		 : Memory pool manager
	Comment          :
*/

#pragma once

#include "w_system_export.h"
#include "w_std.h"
#include <w_aligned_malloc.h>

#define __1KB__ 1024
#define __1MB__ 1024 * __1KB__
#define __1GB__ 1024 * __1MB__
#define __1TB__ 1024 * __1GB__
#define __1PB__ 1024 * __1TB__

namespace wolf::system
{
	struct w_memory_pool
	{
	public:
		w_memory_pool()
		{
		}

		~w_memory_pool()
		{
			release();
		}

		//Allocate block of memory (in bytes)
		void* alloc(_In_ size_t pSizeInBytes, _In_ size_t pAlignment = 16)
		{
			this->_size_in_bytes = pSizeInBytes;
			this->_alignment = pAlignment;

#if defined(__WIN32) || defined(_MSC_VER) 
			this->_ptr = _aligned_malloc(pSizeInBytes, pAlignment);
#else
			this->_ptr = aligned_malloc(pSizeInBytes, pAlignment);
#endif
			return this->_ptr;
		}

		//Re-allocate block of memory (in bytes)
		void* re_alloc(_In_ size_t pSizeInBytes, _In_ size_t pAlignment = 16)
		{
			this->_size_in_bytes = pSizeInBytes;
			this->_alignment = pAlignment;

#if defined(__WIN32) || defined(_MSC_VER) 
			this->_ptr = _aligned_realloc(this->_ptr, pSizeInBytes, pAlignment);
#else
			aligned_free(this->_ptr);
			this->_ptr = aligned_malloc(pSizeInBytes, pAlignment);
#endif
			return this->_ptr;
		}

		//Discard all allocated memory
		ULONG release()
		{
			if (this->_is_released) return 1;

#if defined(__WIN32) || defined(_MSC_VER) 
			_aligned_free(this->_ptr);
#else
			aligned_free(this->_ptr);
#endif
			this->_ptr = nullptr;
			this->_size_in_bytes = 0;
			this->_alignment = 0;

			this->_is_released = true;
			return 0;
		}

		//Get pointer to start of memory
		void* get_start_ptr()
		{
			return this->_ptr;
		}

		//Get total size of memory in bytes
		const size_t get_size_in_bytes() const
		{
			return this->_size_in_bytes;
		}

		//Get alignment of memory
		const size_t get_alignment() const
		{
			return this->_alignment;
		}

		size_t											write_index = 0;
		size_t											read_index = 0;
	private:
		//Prevent copying
		w_memory_pool(w_memory_pool const&);
		w_memory_pool& operator= (w_memory_pool const&);

		void* _ptr = nullptr;
		size_t                                          _size_in_bytes = 0;
		size_t                                          _alignment = 0;
		bool                                            _is_released = false;
	};
}
