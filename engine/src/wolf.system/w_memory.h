/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_memory.h
	Description		 : Linear memory manager
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_MEMORY_H__
#define __W_MEMORY_H__

#ifdef __WIN32

#include "w_system_export.h"
#include "w_linear_allocator.h"

#define MAX_PTR_ALLOCS 2048

namespace wolf
{
	namespace system
	{
		class w_memory
		{
		public:
			WSYS_EXP w_memory();
			WSYS_EXP ~w_memory();

			WSYS_EXP bool	Malloc(size_t pSize);
			WSYS_EXP bool	Allocate(size_t pSize, uint8_t pAlignment);
			WSYS_EXP void*	Read(size_t p_memory_address);
			WSYS_EXP void	Write(size_t p_memory_address, void* pValue);
			WSYS_EXP size_t	Get_address() const;
			WSYS_EXP void	Clear();
			WSYS_EXP void	Free();

			// _1KB = 10424 B, make sure change size of MAX_PTR_ALLOCS
			static const size_t _1KB() { return 1024; }
			// _1MB = 1048576 B, make sure change size of MAX_PTR_ALLOCS
			static const size_t _1MB() { return 1048576; }
			// _1GB = 1073741824 B, make sure change size of MAX_PTR_ALLOCS
			static const size_t _1GB() { return 1073741824; }

		private:
			//Prevent copying
			w_memory(w_memory const&);
			w_memory& operator= (w_memory const&);

			bool				_isReleased;
			void*				_mem;
			void*				_ptrAllocs[MAX_PTR_ALLOCS];
			w_linear_allocator*	_linearAllocator;
			size_t				_writeAddress;
		};
	}
}

#endif // __WIN32

#endif //__W_MEMORY_H__
