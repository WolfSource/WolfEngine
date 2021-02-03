/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
    Name			 : w_fiber.h
    Description		 : a fiber based on boost::fiber
    Comment          :
*/
#pragma once

#ifdef WOLF_ENABLE_FIBER

#include <wolf.h>
#include <initializer_list>

typedef struct scheduler_info
{
    char*       name = nullptr;
    size_t*     fiber_ids = nullptr;
    size_t      number_of_fibers = 0;
} scheduler_info;

/**
 * initialize a scheduler
 * @param pSchedulerName name of fibers scheduler
 * @param pFiberTasks tasks of fibers
 * @return result as W_RESULT
*/
template<typename T>
W_RESULT w_fiber_scheduler_init(
    _Inout_ w_mem_pool pMemPool,
    _In_z_ const char* pSchedulerName,
    _In_ std::initializer_list<const T&> pFiberTasks);

/**
 * get scheduler information
 * @param pSchedulerName name of fibers scheduler
 * @return scheduler information
*/
scheduler_info*  w_fiber_get_scheduler_info(
    _Inout_ w_mem_pool pMemPool,
    _In_z_ const char* pSchedulerName);

/**
 * get fiber is joinable
 * @param pSchedulerName name of the scheduler
 * @param pFiberIndex index of fiber in scheduler
 * @return result as W_RESULT
*/
W_RESULT w_fiber_is_joinable(
    _In_z_ const char* pSchedulerName,
    _In_ const size_t pFiberIndex);

/**
 * join a fiber from scheduler
 * @param pSchedulerName name of the scheduler
 * @param pFiberIndex index of fiber in scheduler
 * @return result as W_RESULT
*/
W_RESULT w_fiber_join(
    _In_z_ const char* pSchedulerName,
    _In_ const size_t pFiberIndex);

/**
 * detach a fiber from scheduler
 * @param pSchedulerName name of the scheduler
 * @param pFiberIndex index of fiber in scheduler
 * @return result as W_RESULT
*/
W_RESULT w_fiber_detach(
    _In_z_ const char* pSchedulerName,
    _In_ const size_t pFiberIndex);

/**
 * swap two fibers from same scheduler
 * @param pSchedulerName name of the scheduler
 * @param pFiberIndex index of fiber in scheduler
 * @return result as W_RESULT
*/
W_RESULT w_fiber_swap(
    _In_z_ const char* pSchedulerName,
    _In_   const size_t pFiberIndex,
    _In_  const size_t pAnotherFiberIndex);

/**
 * swap two fibers from different scheduler
 * @param pSchedulerName name of the scheduler
 * @param pFiberIndex index of fiber in scheduler
 * @param pAnotherSchedulerName name of the another scheduler
 * @param pAnotherFiberIndex index of another fiber in another scheduler
 * @return result as W_RESULT
*/
W_RESULT w_fiber_swap(
    _In_z_ const char* pSchedulerName,
    _In_ const size_t pFiberIndex,
    _In_z_ const char* pAnotherSchedulerName,
    _In_ const size_t pAnotherFiberIndex);

#endif
