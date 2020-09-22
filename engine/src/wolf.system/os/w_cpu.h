/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : w_cpu.h
    Description      : Show the information of CPU
    Comment          :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "wolf.h"

#ifdef W_PLATFORM_WIN
#pragma comment(lib, "pdh.lib")
#endif

//forward declaration
struct w_cpu_pv;
typedef struct
{
    W_RESULT            error_code;
    unsigned long       last_sample_time;
    float               cpu_usage;
    struct w_cpu_pv*    cpu_pv;
} w_cpu_imp;
typedef w_cpu_imp* w_cpu;

/**
 * initialize an object which is responsible to query the cpu information
 * @return result code
*/
w_cpu w_cpu_init(void);

/**
 * Update query and get the percentage of CPU usage
 * @param pCPU a pointer to cpu information object
 * @return result code
*/
W_RESULT w_cpu_get_info(_Inout_ w_cpu pCPU);

/**
 * free CPU information object
 * @param pCPU a pointer to cpu information object
 * @return result code
*/
W_RESULT w_cpu_free(_Inout_ w_cpu pCPU);

#ifdef __cplusplus
}
#endif
