#include "w_mem_pool.h"
#include <stdlib.h>
#include <apr-1/apr_pools.h>
#include <apr-1/apr_atomic.h>

static  volatile apr_uint64_t				s_number_apr_pool_ref_counts = 0;

typedef struct w_mem_pool_t
{
	apr_pool_t*				apr;
} w_mem_pool_t;

W_RESULT w_mem_pool_init_from_parent(
	_Inout_ w_mem_pool* pMemPool,
	_Inout_opt_ w_mem_pool* pParentMemPool)
{

	if (pMemPool && *pMemPool)
	{
		w_mem_pool_fini(pMemPool);
	}

	W_RESULT _ret = 1;
	*pMemPool = (w_mem_pool_t*)malloc(sizeof(struct w_mem_pool_t));
	if (!(*pMemPool))
	{
		return _ret;
	}

	_ret = 0;

	apr_pool_t* _parent = NULL;
	if (pParentMemPool && *pParentMemPool)
	{
		_parent = (*pParentMemPool)->apr;
	}

	apr_pool_create(&((*pMemPool)->apr), _parent);
	apr_allocator_t* _allocator = apr_pool_allocator_get((*pMemPool)->apr);
	if (_allocator)
	{
		apr_allocator_max_free_set(_allocator, 32);
	}

	//increase number of ref counts of apr pools
#if defined (_WIN64) || defined (W_PLATFORM_ANDROID) || defined (W_PLATFORM_OSX) || defined (W_PLATFORM_IOS) || defined (W_PLATFORM_LINUX)
	apr_atomic_inc64(&s_number_apr_pool_ref_counts);
#else
	apr_atomic_inc32(&s_number_apr_pool_ref_counts);
#endif

	return _ret;
}

W_RESULT w_mem_pool_init(_Inout_ w_mem_pool* pMemPool)
{
	return w_mem_pool_init_from_parent(pMemPool, NULL);
}

void* w_malloc(
	_Inout_ w_mem_pool pMemPool,
	_In_ size_t pMemSize)
{
	if (pMemSize && pMemPool && pMemPool->apr)
	{
		return apr_palloc(pMemPool->apr, pMemSize);
	}
	return NULL;
}

void* w_calloc(
	_Inout_ w_mem_pool pMemPool,
	_In_ size_t pMemSize)
{
	if (pMemSize && pMemPool && pMemPool->apr)
	{
		return apr_pcalloc(pMemPool->apr, pMemSize);
	}
	return NULL;
}

void w_mem_pool_fini(_Inout_ w_mem_pool* pMemPool)
{
	if (!pMemPool || !(*pMemPool))
	{
		return;
	}

	apr_pool_destroy((*pMemPool)->apr);
	//decrease number of ref counts of apr pools

#if defined (_WIN64) || defined (W_PLATFORM_ANDROID) || defined (W_PLATFORM_OSX) || defined (W_PLATFORM_IOS) || defined (W_PLATFORM_LINUX)
	apr_atomic_dec64(&s_number_apr_pool_ref_counts);
#else
	apr_atomic_dec32(&s_number_apr_pool_ref_counts);
#endif

	free((*pMemPool));
	*pMemPool = NULL;
}

void w_mem_pool_clear(_Inout_ w_mem_pool pMemPool)
{
	if (pMemPool && pMemPool->apr)
	{
		apr_pool_clear(pMemPool->apr);
	}
}

w_apr_pool w_mem_pool_get_apr_pool(_Inout_ w_mem_pool pMemPool)
{
	if (pMemPool)
	{
		return pMemPool->apr;
	}
	return NULL;
}

size_t w_mem_pool_get_ref_counts()
{
#if defined (_WIN64) || defined (W_PLATFORM_ANDROID) || defined (W_PLATFORM_OSX) || defined (W_PLATFORM_IOS) || defined (W_PLATFORM_LINUX)
	apr_atomic_read64(&s_number_apr_pool_ref_counts);
#else
	apr_atomic_read32(&s_number_apr_pool_ref_counts);
#endif

	return (size_t)s_number_apr_pool_ref_counts;
}

#pragma region old mem pool

////Minimum size of memory pool
//#define MIN_MEMORY_POOL_CHUNKS_SIZE			64Ui64//64 Bytes
//
////dump memory in to the file
//#define INITIALIZE_SIGN						0xCC
//#define ALLOCATE_SIGN						0xAA
//#define FREE_SIGN							0xFF

//struct w_mem_chunk_t
//{
//	//used for checking wheather this chunk is free
//	uint8_t						is_free;
//	//the actual size of data
//	size_t						data_size;
//	//next memory chunk
//	struct w_mem_chunk_t* next;
//	//previous memory chunk
//	struct w_mem_chunk_t* prev;
//};
//typedef struct w_mem_chunk_t w_mem_chunk;
//
//struct w_mem_pool_chunks_t
//{
//	//memory
//	char* pool;
//	//free pool size
//	size_t				free_pool_size;
//	//max pool size
//	size_t				max_pool_size;
//	//debug dump
//	uint8_t				debug_dump;
//	//mutex for thread safety
//	apr_thread_mutex_t* mutex;
//	//apr memory pool for thread mutex
//	apr_pool_t* mutex_mem_pool;
//};
//typedef struct w_mem_pool_chunks_t* w_mem_pool_chunks;
//
//
//w_mem_pool_chunks _mem_pool_chunks_init(_In_ size_t pMaxSizeInBytes, _In_ uint8_t pEnableDebugDump)
//{
//	const char* _trace_info = "w_mem_chunk_init";
//	if (pMaxSizeInBytes < MIN_MEMORY_POOL_CHUNKS_SIZE)
//	{
//		W_ASSERT_P(false, "minimum size of chunk should be 64 bytes. trace info: %s", _trace_info);
//		return NULL;
//	}
//
//	//create a memory pool
//	w_mem_pool_chunks _mem_pool = (w_mem_pool_chunks)malloc(sizeof(struct w_mem_pool_chunks_t));
//	if (!_mem_pool)
//	{
//		W_ASSERT_P(false, "could not allocate memory pool chunks. trace info: %s", _trace_info);
//		return NULL;
//	}
//	//store max pool size
//	_mem_pool->max_pool_size = pMaxSizeInBytes;
//	_mem_pool->pool = (char*)malloc(pMaxSizeInBytes);
//	if (!_mem_pool->pool)
//	{
//		W_ASSERT_P(false,
//			"could not allocate %llu bytes for memory pool chunks. trace info: %s",
//			pMaxSizeInBytes,
//			_trace_info);
//		return NULL;
//	}
//
//	//create mutex
//	int _mutex_created = 0;
//	apr_pool_create(&_mem_pool->mutex_mem_pool, NULL);
//	if (_mem_pool->mutex_mem_pool)
//	{
//		if (apr_thread_mutex_create(&_mem_pool->mutex, 0x0, _mem_pool->mutex_mem_pool) == APR_SUCCESS)
//		{
//			_mutex_created = 1;
//		}
//		else
//		{
//			//terminate apr memory pool of mutex
//			apr_pool_destroy(_mem_pool->mutex_mem_pool);
//		}
//	}
//	if (!_mutex_created)
//	{
//		W_ASSERT_P(false,
//			"could not create thread mutex for memory pool chunks. trace info: %s",
//			_trace_info);
//		return NULL;
//	}
//
//	_mem_pool->debug_dump = pEnableDebugDump;
//	//initialize it for debugging
//	if (_mem_pool->debug_dump)
//	{
//		memset(_mem_pool->pool, INITIALIZE_SIGN, pMaxSizeInBytes);
//	}
//
//	//get size of each chunk
//	const size_t _chunk_size = sizeof(w_mem_chunk);
//
//	//find the free pool size
//	size_t _free_pool_size = pMaxSizeInBytes - _chunk_size;
//	_mem_pool->free_pool_size = _free_pool_size;
//
//	//create the block of memory and copy to the pool
//	w_mem_chunk _free_chunk;
//	_free_chunk.data_size = _free_pool_size;
//	memcpy(_mem_pool->pool, &_free_chunk, _chunk_size);
//
//	return _mem_pool;
//}
//
//void _mem_pool_chunks_clear(_Inout_ w_mem_pool_chunks pMemPoolChunks)
//{
//	//get size of each chunk
//	const size_t _chunk_size = sizeof(w_mem_chunk);
//
//	//find the free pool size
//	size_t _free_pool_size = pMemPoolChunks->max_pool_size - _chunk_size;
//	pMemPoolChunks->free_pool_size = _free_pool_size;
//
//	//create the block of memory and copy to the pool
//	w_mem_chunk _free_chunk;
//	_free_chunk.data_size = _free_pool_size;
//	memcpy(pMemPoolChunks->pool, &_free_chunk, _chunk_size);
//}
//
//void* _mem_pool_chunks_malloc(
//	_Inout_ w_mem_pool_chunks pMemPoolChunks,
//	_In_ size_t pMemSize,
//	_In_ uint8_t pZeroMemory)
//{
//	const char* _trace_info = "_mem_pool_chunks_malloc";
//	if (!pMemPoolChunks || !pMemSize)
//	{
//		W_ASSERT_P(false, "missing parameters. trace info: %s", _trace_info);
//		return NULL;
//	}
//
//	const size_t _size_of_mem_chunk = sizeof(w_mem_chunk);
//	//actually the require size is sum of allocated memory plus the size of memory chunk
//	size_t _req_size = pMemSize + _size_of_mem_chunk;
//
//	//cast memory pool to memory chunks
//	w_mem_chunk* _pool_chuck = (w_mem_chunk*)(pMemPoolChunks);
//	//try to find a free chunk by iterating chunks
//	while (_pool_chuck)
//	{
//		if (_pool_chuck->is_free && _pool_chuck->data_size >= _req_size)
//		{
//			//fine, we've got a free chuck
//			break;
//		}
//		_pool_chuck = _pool_chuck->next;
//	}
//
//	//return NULL if we could't find any free chunk
//	if (!_pool_chuck) return NULL;
//
//	//cast chunk to char pointer
//	char* _chuck_data_start_ptr = (char*)_pool_chuck;
//
//	//the free size of this chunk should be greater than minimum size of each memory chunks
//	size_t _free_size = _pool_chuck->data_size - _req_size;
//	if (_free_size > MIN_MEMORY_POOL_CHUNKS_SIZE)
//	{
//		//create free chunk
//		w_mem_chunk _free_chuck;
//		_free_chuck.is_free = true;
//		_free_chuck.data_size = _free_size;
//		_free_chuck.next = _pool_chuck->next;
//		_free_chuck.prev = _pool_chuck;
//
//		// write this chuck from this position
//		//[ chuck_data_start_position + the required size of data in bytes + sizeof(MemoryChunk) ]
//		memcpy(_chuck_data_start_ptr + _req_size, &_free_chuck, _size_of_mem_chunk);
//
//		// make it as a linked list of chucks
//		if (_free_chuck.next)
//		{
//			_free_chuck.next->prev = (w_mem_chunk*)(_chuck_data_start_ptr + _req_size);
//		}
//		_pool_chuck->next = (w_mem_chunk*)(_chuck_data_start_ptr + _req_size);
//
//		//store size of data
//		_pool_chuck->data_size = pMemSize;
//	}
//
//	//make sure to update the pool size
//	pMemPoolChunks->free_pool_size -= _pool_chuck->data_size;
//
//	//this memory is not free any more
//	_pool_chuck->is_free = false;
//
//	if (pMemPoolChunks->debug_dump || pZeroMemory)
//	{
//		//mark it with specific sign just for dump memory to file
//		memset(
//			_chuck_data_start_ptr + _size_of_mem_chunk,
//			INITIALIZE_SIGN,
//			_pool_chuck->data_size);
//	}
//
//	return (_chuck_data_start_ptr + _size_of_mem_chunk);
//}
//
//void _mem_pool_chunks_free(
//	_Inout_ w_mem_pool_chunks pMemPoolChunks,
//	_In_ const void* pMemoryAddress)
//{
//	const char* _trace_info = "_mem_pool_chunks_free";
//	if (!pMemPoolChunks || !pMemoryAddress)
//	{
//		W_ASSERT_P(false, "missing parameters. trace info: %s", _trace_info);
//		return;
//	}
//
//	const size_t _chunk_size = sizeof(w_mem_chunk);
//
//	//get chuck struture
//	w_mem_chunk* _chunk = (w_mem_chunk*)((char*)pMemoryAddress - _chunk_size);
//
//	//return if the chuck is free
//	if (_chunk->is_free) return;
//
//	//get actual chunk size
//	size_t _actual_chunk_size = _chunk->data_size + _chunk_size;
//	//return size of data to the free pool size
//	pMemPoolChunks->free_pool_size += _chunk->data_size;
//
//	w_mem_chunk* _head = _chunk;
//	w_mem_chunk* _prev = _chunk->prev;
//	w_mem_chunk* _next = _chunk->next;
//
//	//merge the previous free chunk with this chunk
//	if (_chunk->prev && _chunk->prev->is_free)
//	{
//		/*
//		 merging
//		 this is our situation before free:
//
//			[previous free chunk] -> [the chunk that should be free] -> [next chunk]
//
//		 and the memory should be like this after merging:
//
//			... -> [previous free chunk + the chunk that should be free] -> [next chunk]
//
//		*/
//
//		_head = _chunk->prev;
//		_prev = _chunk->prev->prev;
//		_next = _chunk->next;
//
//		_actual_chunk_size += _chunk->prev->data_size + _chunk_size;
//		if (_chunk->next)
//		{
//			//now, _head is merged chunk
//			_chunk->next->prev = _head;
//
//			//merge the next chunk with our current chunk if it is free
//			if (_chunk->next->is_free)
//			{
//				_next = _chunk->next->next;
//				//make sure to specify which chunk is the head now
//				if (_chunk->next->next)
//				{
//					_chunk->next->next->prev = _head;
//				}
//				_actual_chunk_size += _chunk->next->data_size + _chunk_size;
//			}
//		}
//	}
//	else
//	{
//		//merge the nex free chunk with this chunk
//		if (_chunk->next && _chunk->next->is_free)
//		{
//			/*
//			 merging
//			 this is our situation before free:
//
//			 [previous chunk] -> [the chunk that should be free] -> [next free chunk]
//
//			 and the memory should be like this after merging:
//
//			 [previous chunk] -> [the chunk that should be free + next free chunk] -> ...
//
//			 */
//
//			_head = _chunk;
//			_prev = _chunk->prev;
//			_next = _chunk->next->next;
//
//			_actual_chunk_size += _chunk->next->data_size + _chunk_size;
//		}
//	}
//
//	//create the free chunk
//	char* _free_chunk_start_position = (char*)_head;
//	w_mem_chunk _free_chunk;
//	_free_chunk.is_free = true;
//	_free_chunk.data_size = (_actual_chunk_size - _chunk_size);
//	_free_chunk.prev = _prev;
//	_free_chunk.next = _next;
//
//	if (pMemPoolChunks->debug_dump)
//	{
//		//mark it with specific sign just for dump memory to file
//		memset(
//			_free_chunk_start_position,
//			FREE_SIGN,
//			_free_chunk.data_size);
//	}
//
//	memcpy(
//		_free_chunk_start_position,
//		&_free_chunk,
//		_chunk_size);
//}

#pragma endregion