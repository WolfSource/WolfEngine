/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_allocator.h
	Description		 : Responsible for allocating memory. this code modified and optimized based on "allocator" class written by "Tiago Costa", Copyright(c) 2013
	Comment          :
*/

#ifndef __W_ALLOCATOR_H__
#define __W_ALLOCATOR_H__

#ifdef __WIN32

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>
#include <new>
#include <assert.h>

namespace pointer_math
{
	void*       align_forward(void* pAddress, uint8_t pAlignment);
	const void* align_forward(const void* pAddress, uint8_t pAlignment);

	void*       align_backward(void* pAddress, uint8_t pAlignment);
	const void* align_backward(const void* pAddress, uint8_t pAlignment);

	uint8_t     align_forward_adjustment(const void* pAddress, uint8_t pAlignment);
	uint8_t     align_forward_adjustment_with_header(const void* pAddress, uint8_t pAlignment, uint8_t pHeaderSize);

	uint8_t     align_backward_adjustment(const void* pAddress, uint8_t pAlignment);

	void*       add(void* pP, size_t pX);
	const void* add(const void* pP, size_t pX);

	void*       subtract(void* pP, size_t pX);
	const void* subtract(const void* pP, size_t pX);
}

namespace wolf
{
	namespace system
	{
		class w_allocator
		{
		public:
			w_allocator(size_t pSize, void* pStart)
			{
				this->_start = pStart;
				this->_size = pSize;

				this->_used_memory = 0;
				this->_num_allocations = 0;
			}

			virtual ~w_allocator()
			{
				assert(this->_num_allocations == 0 && this->_used_memory == 0);

				this->_start = nullptr;
				this->_size = 0;
			}

			virtual void* allocate(size_t pSize, uint8_t pAlignment = 4) = 0;

			virtual void deallocate(void* pMemory) = 0;

			void* get_start() const
			{
				return _start;
			}

			size_t get_size() const
			{
				return _size;
			}

			size_t get_used_memory() const
			{
				return _used_memory;
			}

			size_t get_num_allocations() const
			{
				return _num_allocations;
			}

		protected:
			void*         _start;
			size_t        _size;

			size_t        _used_memory;
			size_t        _num_allocations;
		};

		namespace allocator
		{
			template <class T> T* allocate_new(w_allocator& pAllocator)
			{
				return new (pAllocator.allocate(sizeof(T), __alignof(T))) T;
			}

			template <class T> T* allocate_new(w_allocator& pAllocator, const T& pT)
			{
				return new (pAllocator.allocate(sizeof(T), __alignof(T))) T(pT);
			}

			template<class T> void deallocate_delete(w_allocator& pAllocator, T& pObject)
			{
				pObject.~T();
				pAllocator.deallocate(&pObject);
			}

			template<class T> T* allocate_array(w_allocator& pAllocator, size_t pLength)
			{
				ASSERT(pLength != 0);

				u8 headerSize = sizeof(size_t) / sizeof(T);

				if (sizeof(size_t) % sizeof(T) > 0)
					headerSize += 1;

				//Allocate extra space to store array length in the bytes before the array
				T* p = ((T*)allocator.allocate(sizeof(T)*(length + headerSize), __alignof(T))) + headerSize;

				*(((size_t*)p) - 1) = length;

				for (size_t i = 0; i < length; i++)
					new (&p[i]) T;

				return p;
			}

			template<class T> void deallocate_array(w_allocator& pAllocator, T* pArray)
			{
				ASSERT(array != nullptr);

				size_t length = *(((size_t*)array) - 1);

				for (size_t i = 0; i < length; i++)
					array[i].~T();

				//Calculate how much extra memory was allocated to store the length before the array
				u8 headerSize = sizeof(size_t) / sizeof(T);

				if (sizeof(size_t) % sizeof(T) > 0)
					headerSize += 1;

				allocator.deallocate(array - headerSize);
			}
		};

		//Inline functions definitions

		namespace pointer_math
		{
			inline void* align_forward(void* pAddress, uint8_t pAlignment)
			{
				return (void*)((reinterpret_cast<uintptr_t>(pAddress) + static_cast<uintptr_t>(pAlignment - 1)) & static_cast<uintptr_t>(~(pAlignment - 1)));
			}

			inline const void* align_forward(const void* pAddress, uint8_t pAlignment)
			{
				return (void*)((reinterpret_cast<uintptr_t>(pAddress) + static_cast<uintptr_t>(pAlignment - 1)) & static_cast<uintptr_t>(~(pAlignment - 1)));
			}

			inline void* align_backward(void* pAddress, uint8_t pAlignment)
			{
				return (void*)(reinterpret_cast<uintptr_t>(pAddress) & static_cast<uintptr_t>(~(pAlignment - 1)));
			}

			inline const void* align_backward(const void* pAddress, uint8_t pAlignment)
			{
				return (void*)(reinterpret_cast<uintptr_t>(pAddress) & static_cast<uintptr_t>(~(pAlignment - 1)));
			}

			inline uint8_t align_forward_adjustment(const void* pAddress, uint8_t pAlignment)
			{
				uint8_t adjustment = pAlignment - (reinterpret_cast<uintptr_t>(pAddress) & static_cast<uintptr_t>(pAlignment - 1));

				if (adjustment == pAlignment) return 0; //already aligned

				return adjustment;
			}

			inline uint8_t align_forward_adjustment_with_header(const void* pAddress, uint8_t pAlignment, uint8_t pHeaderSize)
			{
				uint8_t _adjustment = align_forward_adjustment(pAddress, pAlignment);

				uint8_t _neededSpace = pHeaderSize;

				if (_adjustment < _neededSpace)
				{
					_neededSpace -= _adjustment;

					//Increase adjustment to fit header
					_adjustment += pAlignment * (_neededSpace / pAlignment);

					if (_neededSpace % pAlignment > 0)
					{
						_adjustment += pAlignment;
					}
				}

				return _adjustment;
			}

			inline uint8_t align_backward_adjustment(const void* pAddress, uint8_t pAlignment)
			{
				uint8_t _adjustment = reinterpret_cast<uintptr_t>(pAddress) & static_cast<uintptr_t>(pAlignment - 1);

				if (_adjustment == pAlignment) return 0; //already aligned

				return _adjustment;
			}

			inline void* add(void* pP, size_t pX)
			{
				return (void*)(reinterpret_cast<uintptr_t>(pP) + pX);
			}

			inline const void* add(const void* pP, size_t pX)
			{
				return (const void*)(reinterpret_cast<uintptr_t>(pP) + pX);
			}

			inline void* subtract(void* pP, size_t pX)
			{
				return (void*)(reinterpret_cast<uintptr_t>(pP) - pX);
			}

			inline const void* subtract(const void* pP, size_t pX)
			{
				return (const void*)(reinterpret_cast<uintptr_t>(pP) - pX);
			}
		};
	}
}

#endif //__WIN32

#endif //__W_ALLOCATOR_H__