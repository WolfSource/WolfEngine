#include "w_system_pch.h"

#ifdef __WIN32

#include "w_linear_allocator.h"
#include <string>

using namespace wolf::system;

w_linear_allocator::w_linear_allocator(size_t pSize, void* pStart) : w_allocator(pSize, pStart), _current_pos(pStart)
{
	assert(pSize > 0);
}

w_linear_allocator::~w_linear_allocator()
{
	this->_current_pos   = nullptr;
}

void* w_linear_allocator::allocate(size_t pSize, uint8_t pAlignment)
{
	assert(pSize != 0);

	uint8_t _adjustment =  pointer_math::align_forward_adjustment(this->_current_pos, pAlignment);

	if (this->_used_memory + _adjustment + pSize > this->_size)
	{
		//OutputDebugString(L"/////////////////////////ERROR\n");
		//OutputDebugString(std::to_wstring(_used_memory).c_str());
		//OutputDebugString(L"\n");
		//OutputDebugString(std::to_wstring(adjustment).c_str());
		//OutputDebugString(L"\n");
		//OutputDebugString(std::to_wstring(size).c_str());
		//OutputDebugString(L"\n");
		//OutputDebugString(std::to_wstring(_size).c_str());
		//OutputDebugString(L"\n");
		//OutputDebugString(L"/////////////////////////ERROR\n");

		return nullptr;
	}

	uintptr_t _aligned_address = (uintptr_t)this->_current_pos + _adjustment;

	this->_current_pos = (void*)(_aligned_address + pSize);

	this->_used_memory += pSize + _adjustment;
	this->_num_allocations++;

	return (void*)_aligned_address;
}

void w_linear_allocator::deallocate(void* pMemory)
{
	assert( false && "Use clear() instead" );
}

void w_linear_allocator::clear()
{
	this->_num_allocations = 0;
	this->_used_memory = 0;
	this->_current_pos = _start;
}

#endif // __WIN32