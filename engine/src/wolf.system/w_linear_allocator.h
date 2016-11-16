/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_linear_allocator.h
	Description		 : Responsible for allocating memory linear. This code modified and optimized based on "LinearAllocator" class written by "Tiago Costa", Copyright(c) 2013
	Comment          :
*/

#ifndef __W_LINEAR_ALLOCATOR_H__
#define __W_LINEAR_ALLOCATOR_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "w_allocator.h"

namespace wolf
{
	namespace system
	{
		class w_linear_allocator : public w_allocator
		{
		public:
			w_linear_allocator(size_t pSize, void* pStart);
			~w_linear_allocator();

			void* allocate(size_t pSize, uint8_t pAlignment) override;

			void deallocate(void* pmemory) override;

			void clear();

		private:
			//Prevent copies because it might cause errors
			w_linear_allocator(const w_linear_allocator&);
			w_linear_allocator& operator=(const w_linear_allocator&);

			void* _current_pos;
		};

		namespace allocator
		{
			inline w_linear_allocator* newLinearAllocator(size_t pSize, w_allocator& pAllocator)
			{
				void* p = pAllocator.allocate(pSize + sizeof(w_linear_allocator), __alignof(w_linear_allocator));
				return new (p) w_linear_allocator(pSize, pointer_math::add(p, sizeof(w_linear_allocator)));
			}

			inline void delete_linear_allocator(w_linear_allocator& pLinearAllocator, w_allocator& pAllocator)
			{
				pLinearAllocator.~w_linear_allocator();

				pAllocator.deallocate(&pLinearAllocator);
			}
		};
	}
}

#endif //__W_LINEAR_ALLOCATOR_H__