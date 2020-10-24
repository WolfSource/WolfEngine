#include "w_sigslot.h"
#include <log/w_log.h>
#include <memory/w_array.h>
#include <memory/w_hash.h>
#include <stdarg.h>

// this is used to cache lengths in apr_pwstrcat
#define MAX_SAVED_LENGTHS   6
#define INITIAL_MAX			100  

typedef struct w_sigslot_t
{
	w_hash hash_table;
	w_array _key;
}w_sigslot_t;

typedef struct w_slot_args
{
	w_slot slot;
	void* args;
}w_slot_args;

static w_mem_pool mem_pool = NULL;

w_sigslot w_sigslot_init()
{
	const char* _trace_info = "w_sigslot_init";

	if (!mem_pool)
	{
		if (w_mem_pool_init(&mem_pool) == W_FAILURE)
		{
			W_ASSERT_P(false,
				"could not create memory. trace info: %s",
				_trace_info);

			return W_FAILURE;
		}
	}

	w_sigslot _signal = (w_sigslot)w_malloc(mem_pool, sizeof(struct w_sigslot_t));
	_signal->hash_table = (w_hash_init(mem_pool));

	_signal->_key = w_array_init(mem_pool, INITIAL_MAX, sizeof(const char*));

	return _signal;
}

W_RESULT w_sigslot_raise(_In_ w_sigslot pSignal, _In_ void* pArgs)
{
	const char* _trace_info = "w_sigslot_rise";

	if (!pSignal)
	{
		W_ASSERT_P(false,
			"bad args. trace info: %s",
			_trace_info);

		return W_FAILURE;
	}

	size_t _array_size = w_array_get_size(pSignal->_key);

	for (uint32_t i = 0; i < _array_size; i++)
	{
		const void* tmp = w_array_get_element(pSignal->_key, (int)i);
		w_slot_args* _slot = (w_slot_args*)w_hash_get(pSignal->hash_table, tmp, 1);
		if (_slot)
		{
			if (pArgs)
			{
				_slot->slot(pArgs);
			}
			_slot->slot(_slot->args);
		}
	}

	return W_SUCCESS;
}

W_RESULT w_sigslot_remove_all(_Inout_ w_sigslot pSignal)
{
	const char* _trace_info = "w_sigslot_remove_all";

	if (!pSignal)
	{
		W_ASSERT_P(false,
			"bad args. trace info: %s",
			_trace_info);

		return W_FAILURE;
	}

	w_hash_clear(pSignal->hash_table);
	w_array_clear(pSignal->_key);
	return W_SUCCESS;
}

W_RESULT w_sigslot_set_slot(_Inout_ w_sigslot pSignal, _In_ w_slot pSlot, void* pArgs)
{
	const char* _trace_info = "w_sigslot_set_slot";

	if (!pSlot)
	{
		W_ASSERT_P(false,
			"bad args. trace info: %s",
			_trace_info);

		return W_FAILURE;
	}

	w_sigslot_remove_all(pSignal);
	int _index = w_hash_size(pSignal->hash_table);

	char* _key = NULL;

	if (w_array_get_element(pSignal->_key, _index) == NULL)
	{
		_key = (char*)w_malloc(mem_pool, sizeof(const char*));

        snprintf(_key, sizeof(_key), "%d", _index);
		w_array_append(pSignal->_key, _key);
	}
	else
	{
		_key = w_array_get_element(pSignal->_key, _index);
        snprintf(_key, sizeof(_key), "%d", _index);
	}

	w_slot_args* _slot_args = (w_slot_args*)w_malloc(mem_pool, sizeof(w_slot_args));
	_slot_args->args = pArgs;
	_slot_args->slot = pSlot;

	w_hash_set(pSignal->hash_table, _key, 1, _slot_args);
	return W_SUCCESS;
}

W_RESULT w_sigslot_add_slot(_Inout_ w_sigslot pSignal, _In_ w_slot pSlot, void* pArgs)
{
	const char* _trace_info = "w_sigslot_add_slot";

	if (!pSlot)
	{
		W_ASSERT_P(false,
			"bad args. trace info: %s",
			_trace_info);

		return W_FAILURE;
	}

	int _index = w_hash_size(pSignal->hash_table);

	if (_index >= INITIAL_MAX)
	{
		W_ASSERT_P(false,
			"Maximum threshold exceeds. trace info: %s",
			_trace_info);

		return W_FAILURE;
	}

	w_apr_pool _pool = w_hash_get_mem_pool(pSignal->hash_table);
	char* _key = NULL;
	void* t1 = NULL;

	if (w_array_get_element(pSignal->_key, _index) == NULL)
	{
		_key = (char*)w_malloc((w_mem_pool)&_pool, sizeof(const char*));

        snprintf(_key, sizeof(_key), "%d", _index);
		t1 = w_array_append(pSignal->_key, _key);
	}
	else
	{
		_key = w_array_get_element(pSignal->_key, _index);
        snprintf(_key, sizeof(_key), "%d", _index);
	}

	w_slot_args* _slot_args = (w_slot_args*)w_malloc((w_mem_pool)&_pool, sizeof(w_slot_args));
	_slot_args->args = pArgs;
	_slot_args->slot = pSlot;

	w_hash_set(pSignal->hash_table, _key, 1, _slot_args);
	return W_SUCCESS;
}

W_RESULT w_sigslot_remove_slot(_Inout_ w_sigslot pSignal, _In_ w_slot pSlot)
{
	const char* _trace_info = "w_sigslot_remove_slot";

	if (!pSlot)
	{
		W_ASSERT_P(false,
			"bad args. trace info: %s",
			_trace_info);

		return W_FAILURE;
	}

	for (uint32_t i = 0; i < w_hash_size(pSignal->hash_table); i++)
	{
		const void* tmp = w_array_get_element(pSignal->_key, (int)i);
		w_slot_args* _slot = (w_slot_args*)w_hash_get(pSignal->hash_table, tmp, 1);

		if (_slot->slot == pSlot)
		{
			w_hash_set(pSignal->hash_table, tmp, 1, NULL);
		}
	}

	return W_SUCCESS;
}

void w_sigslot_fini(_Inout_ w_sigslot pSignal)
{
	w_sigslot_remove_all(pSignal);

	if (mem_pool)
	{
		w_mem_pool_fini(&mem_pool);
	}
}
