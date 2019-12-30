/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_shared_memory.h
	Description		 : shared memory between processes
	Comment          :
*/

#pragma once

#ifdef BOOST

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

namespace wolf::system
{
	inline bool init_shared_memory(_In_ const char& pInitValue)
	{
		using namespace boost::interprocess;

		bool _result = true;
		try
		{
			shared_memory_object::remove(sSharedMemoryName);
			shared_memory_object _shared_mem{ create_only, sSharedMemoryName, read_write };
			//set size for 1 character
			_shared_mem.truncate(1);
			//map the whole shared memory in this process
			mapped_region _region(_shared_mem, read_write);
			//set all memory to pInitValue
			std::memset(_region.get_address(), pInitValue, _region.get_size());
		}
		catch (...)
		{
			_result = false;
		}
		return _result;
	}

	inline  bool write_to_shared_memory(_In_ const char& pInitValue)
	{
		using namespace boost::interprocess;

		bool _result = true;
		try
		{
			shared_memory_object _shared_mem{ open_or_create, sSharedMemoryName, read_write };

			//map the whole shared memory in this process
			mapped_region _region(_shared_mem, read_write);
			//set all memory to pInitValue
			std::memset(_region.get_address(), pInitValue, _region.get_size());
		}
		catch (...)
		{
			_result = false;
		}

		return _result;
	}

	inline  char read_shared_memory(_Inout_ bool& pError)
	{
		using namespace boost::interprocess;

		pError = false;
		try
		{
			shared_memory_object _shared_mem{ open_only, sSharedMemoryName, read_only };
			offset_t  _size = 0;
			if (_shared_mem.get_size(_size))
			{
				if (_size == 1)
				{
					//map the whole shared memory in this process
					mapped_region _region(_shared_mem, read_only);

					auto _mem = static_cast<char*>(_region.get_address());
					if (_mem)
					{
						return _mem[0];
					}
				}
			}
		}
		catch (...)
		{
			pError = true;
		}

		return -1;
	}
}

#endif
