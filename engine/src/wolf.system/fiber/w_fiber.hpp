/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
    Name			 : w_fiber.h
    Description		 : a fiber based on boost::fiber
    Comment          :
*/
#pragma once

#include <wolf.h>

typedef struct
{
    int*    fiber_ids;
    size_t  number_of_fibers;
} scheduler_info;

/**
 * initialize a scheduler
 * @param pNumberOfFibers number of fibers
 * @return scheduler ID
*/
W_RESULT w_fiber_scheduler_init(_In_z_ const char* pScheduleName, _In_ const int pNumberOfFibers);

/**
 * get scheduler information
 * @param pSchedulerInfo scheduler information ID
 * @return scheduler information
*/
scheduler_info*  w_fiber_get_scheduler_info(_In_ const int pSchedulerInfo);
