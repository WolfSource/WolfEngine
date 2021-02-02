/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_sigslot.h
	Description		 : signal and slot
	Comment          :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "wolf.h"

	typedef void* (*w_slot)(void*);
	typedef struct w_sigslot_t* w_sigslot;

	/**
	 * Create a signal.
     * @param pMemPool The pool for the signal associated storage
	 * @param pSignal The signal just created
     * @return result
	 */
	W_SYSTEM_EXPORT
        W_RESULT w_sigslot_init(_Inout_ w_mem_pool pMemPool, _Inout_ w_sigslot* pSignal);

	/**
	 * Raise the signal.
	 * @param pSignal The signal to be raised.
	 * @param pArgs The parameters to be passed to calling slot
	 * @return result
	 */
	W_SYSTEM_EXPORT
		W_RESULT w_sigslot_raise(_In_ w_sigslot pSignal, _In_ void* pArgs);

	/**
	 * Remove all slots from the signal
	 * @param pSignal The signal to be cleared from all its slots.
	 * @return result
	 */
	W_SYSTEM_EXPORT
		W_RESULT w_sigslot_remove_all(_Inout_ w_sigslot pSignal);

	/**
	 * Set the slot for the signal. Calling this function clears all the existing slots first and add the slot to the cleared signal.
	 * @param pSignal The signal that the slot should be added to.
	 * @param pSlot The slot should be added to the signal.
	 * @param pArgs The optional arguments passed to the calling slot
	 * @return result
	 */
	W_SYSTEM_EXPORT
		W_RESULT w_sigslot_set_slot(_Inout_ w_sigslot pSignal, _In_ w_slot pSlot, void* pArgs);

	/**
	 * Add the slot for the signal. Calling this function adds the slot to the signal (No changes in already added slots).
	 * @param pSignal The signal that the slot should be added to.
	 * @param pSlot The slot should be added to the signal.
	 * @param pArgs The optional arguments passed to the calling slot
	 * @return result
	 */
	W_SYSTEM_EXPORT
		W_RESULT w_sigslot_add_slot(_Inout_ w_sigslot pSignal, _In_ w_slot pSlot, void* pArgs);

	/**
	 * Remove the slot for the signal.
	 * @param pSignal The signal that the slot should be removed from.
	 * @param pSlot The slot should be removed from the signal.
	 * @return result
	 */
	W_SYSTEM_EXPORT
		W_RESULT w_sigslot_remove_slot(_Inout_ w_sigslot pSignal, _In_ w_slot pSlot);

	/**
	 * Destroy all resources.
	 * @param pSignal The signal to be destroyed.
     *   @return result
	 */
	W_SYSTEM_EXPORT
        W_RESULT w_sigslot_fini(_Inout_ w_sigslot* pSignal);

#ifdef __cplusplus
}
#endif

