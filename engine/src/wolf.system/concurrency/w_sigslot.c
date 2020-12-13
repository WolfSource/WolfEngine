#include "w_sigslot.h"
#include <log/w_log.h>
#include <stdio.h>
#include <memory/w_array.h>
#include <memory/w_hash.h>
#include <stdarg.h>

// this is used to cache lengths in apr_pwstrcat
#define MAX_SAVED_LENGTHS   6
#define INITIAL_MAX			100  

typedef struct w_sigslot_t
{
	w_hash hash_table;
	w_array key;
    w_mem_pool pool;
}w_sigslot_t;

typedef struct w_slot_args
{
	w_slot slot;
	void* args;
}w_slot_args;

W_RESULT w_sigslot_init(_Inout_ w_mem_pool pMemPool, _Inout_ w_sigslot* pSignal)
{
	const char* _trace_info = "w_sigslot_init";

    *pSignal = NULL;
	if (!pMemPool)
	{
        W_ASSERT_P(false,
            "bad args. trace info: %s",
            _trace_info);

        return W_BAD_ARG;
	}

	w_sigslot _signal = (w_sigslot)w_malloc(pMemPool, sizeof(struct w_sigslot_t));
    
    if(!_signal)
    {
        W_ASSERT_P(false,
            "could not allocate memory for signal. trace info: %s",
            _trace_info);

        return W_FAILURE;
    }
    
	_signal->hash_table = (w_hash_init(pMemPool));
	_signal->key = w_array_init(pMemPool, INITIAL_MAX, sizeof(const char*));
    _signal->pool = pMemPool;
    
    *pSignal = _signal;

	return W_SUCCESS;
}

W_RESULT w_sigslot_raise(_In_ w_sigslot pSignal, _In_ void* pArgs)
{
	const char* _trace_info = "w_sigslot_rise";

	if (!pSignal)
	{
		W_ASSERT_P(false,
			"bad args. trace info: %s",
			_trace_info);

		return W_BAD_ARG;
	}

	size_t _array_size = w_array_get_size(pSignal->key);

	for (uint32_t i = 0; i < _array_size; i++)
	{
		const void* tmp = w_array_get_element(pSignal->key, (int)i);
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

		return W_BAD_ARG;
	}

	w_hash_clear(pSignal->hash_table);
	w_array_clear(pSignal->key);
	return W_SUCCESS;
}

W_RESULT w_sigslot_set_slot(_Inout_ w_sigslot pSignal, _In_ w_slot pSlot, void* pArgs)
{
	const char* _trace_info = "w_sigslot_set_slot";

	if (!pSlot || !pSignal)
	{
		W_ASSERT_P(false,
			"bad args. trace info: %s",
			_trace_info);

		return W_BAD_ARG;
	}

	w_sigslot_remove_all(pSignal);
	int _index = w_hash_size(pSignal->hash_table);

	char* _key = NULL;

	if (w_array_get_element(pSignal->key, _index) == NULL)
	{
		_key = (char*)w_malloc(pSignal->pool, sizeof(const char*));
        
        if(!_key)
        {
            W_ASSERT_P(false,
                "could not allocate memory to add slot. trace info: %s",
                _trace_info);

            return W_FAILURE;
        }

        snprintf(_key, sizeof(const char*), "%d", _index);
		w_array_append(pSignal->key, _key);
	}
	else
	{
		_key = (char*)w_array_get_element(pSignal->key, _index);
        snprintf(_key, sizeof(const char*), "%d", _index);
	}

	w_slot_args* _slot_args = (w_slot_args*)w_malloc(pSignal->pool, sizeof(w_slot_args));
    if(!_slot_args)
    {
        W_ASSERT_P(false,
            "could not allocate memory to add arguments. trace info: %s",
            _trace_info);

        return W_FAILURE;
    }
    
	_slot_args->args = pArgs;
	_slot_args->slot = pSlot;

	w_hash_set(pSignal->hash_table, _key, 1, _slot_args);
	return W_SUCCESS;
}

W_RESULT w_sigslot_add_slot(_Inout_ w_sigslot pSignal, _In_ w_slot pSlot, void* pArgs)
{
	const char* _trace_info = "w_sigslot_add_slot";

	if (!pSlot || !pSignal)
	{
		W_ASSERT_P(false,
			"bad args. trace info: %s",
			_trace_info);

		return W_BAD_ARG;
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

	if (w_array_get_element(pSignal->key, _index) == NULL)
	{
		_key = (char*)w_malloc((w_mem_pool)&_pool, sizeof(const char*));
        
        if(!_key)
        {
            W_ASSERT_P(false,
                "could not allocate memory to add slot. trace info: %s",
                _trace_info);

            return W_FAILURE;
        }

        snprintf(_key, sizeof(const char*), "%d", _index);
		w_array_append(pSignal->key, _key);
	}
	else
	{
		_key = (char*)w_array_get_element(pSignal->key, _index);
        snprintf(_key, sizeof(const char*), "%d", _index);
	}

	w_slot_args* _slot_args = (w_slot_args*)w_malloc((w_mem_pool)&_pool, sizeof(w_slot_args));
    if(!_slot_args)
    {
        W_ASSERT_P(false,
            "could not allocate memory to add arguments. trace info: %s",
            _trace_info);

        return W_FAILURE;
    }
    
	_slot_args->args = pArgs;
	_slot_args->slot = pSlot;

	w_hash_set(pSignal->hash_table, _key, 1, _slot_args);
	return W_SUCCESS;
}

W_RESULT w_sigslot_remove_slot(_Inout_ w_sigslot pSignal, _In_ w_slot pSlot)
{
	const char* _trace_info = "w_sigslot_remove_slot";

	if (!pSlot || !pSignal)
	{
		W_ASSERT_P(false,
			"bad args. trace info: %s",
			_trace_info);

		return W_BAD_ARG;
	}

	for (uint32_t i = 0; i < w_hash_size(pSignal->hash_table); i++)
	{
		const void* tmp = w_array_get_element(pSignal->key, (int)i);
		w_slot_args* _slot = (w_slot_args*)w_hash_get(pSignal->hash_table, tmp, 1);

		if (_slot->slot == pSlot)
		{
			w_hash_set(pSignal->hash_table, tmp, 1, NULL);
		}
	}

	return W_SUCCESS;
}

W_RESULT w_sigslot_fini(_Inout_ w_sigslot* pSignal)
{
    const char* _trace_info = "w_sigslot_fini";

    if (!pSignal || !(*pSignal))
    {
        W_ASSERT_P(false,
            "bad args. trace info: %s",
            _trace_info);

        return W_BAD_ARG;
    }
    
	w_sigslot_remove_all(*pSignal);
	*pSignal = NULL;
    
    return W_SUCCESS;
}
