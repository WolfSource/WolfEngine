#include "w_system_pch.h"

#ifdef __WIN32

#include "w_memory.h"
#include <memory>

using namespace wolf::system;

w_memory::w_memory() : _writeAddress(0), _isReleased(false)
{

}

w_memory::~w_memory()
{
	free();
}

bool w_memory::malloc(size_t pSize)
{
	//First free it, if we allocated before
	if (this->_mem) scalable_free(this->_mem);
	if (this->_linearAllocator)
	{
		this->_linearAllocator->clear();
		delete this->_linearAllocator;
	}

	//Allocate memory
	this->_mem = scalable_malloc(pSize);
	if (!this->_mem) return false;
	
	this->_linearAllocator = new w_linear_allocator(pSize, this->_mem);
	if (!this->_linearAllocator) return false;

	this->_writeAddress = 0;

	return true;
}

bool w_memory::allocate(size_t pSize, uint8_t pAlignment)
{
	auto _memory = this->_linearAllocator->allocate(pSize, pAlignment);
	if (!_memory || this->_writeAddress >= MAX_PTR_ALLOCS)
	{
		if (this->_writeAddress >= MAX_PTR_ALLOCS)
		{
			logger.error(L"Memory overflow");
		}
		return false;
	}
	this->_ptrAllocs[this->_writeAddress++] = _memory;
	
	return true;
}

void* w_memory::read(size_t pMemoryAddress)
{
	return this->_ptrAllocs[pMemoryAddress];
}

void w_memory::write(size_t pMemoryAddress, void* pValue)
{
	this->_ptrAllocs[pMemoryAddress] = pValue;
}

size_t w_memory::get_address() const
{
	return this->_writeAddress;
}

void w_memory::clear()
{
	this->_writeAddress = 0;
	if (this->_linearAllocator)
	{
		this->_linearAllocator->clear();
	}
}

void w_memory::free()
{
	if (this->_isReleased) return;

	if (this->_mem) scalable_free(this->_mem);
	if (this->_linearAllocator)
	{
		this->_linearAllocator->clear();
		delete this->_linearAllocator;
	}
	this->_isReleased = true;
}

#endif