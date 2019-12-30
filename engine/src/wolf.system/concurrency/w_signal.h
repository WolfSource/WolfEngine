/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : w_signal.h
    Description      : signal handling
    Comment          :
*/


#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "wolf.h"

typedef void w_signal_handler(int);

/**
 * create w_signal
 * @return signal number
*/
int w_signal_init(void);

/**
 * get description of signal
 * @param pSigno signal number
 * @return description
*/
const char* w_signal_get_desc(_In_ int pSigno);

/**
 * Run the specified function one time, regardless of how many threads call it.
 * @param pSigno signal number
 * @param pHandler signal handler
 * @return the function to get called
*/
w_signal_handler* w_signal_set_handler(_In_ int pSigno, _In_ w_signal_handler* pHandler);

/**
 * Block the delivery of a particular signal
 * @param pSigno signal number info
 * @return result
*/
W_RESULT w_signal_block(_In_ int pSigno);

/**
 * Enable the delivery of a particular signal
 * @param pSigno signal number info
 * @return result
*/
W_RESULT w_signal_unblock(_In_ int pSigno);


#ifdef __cplusplus
}
#endif
