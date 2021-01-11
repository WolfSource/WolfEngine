/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
    Source           : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
    Website          : https://WolfEngine.App
    Name             : amq_connection_type.h
    Description      : activeMQ connection type
    Comment          :

*/

#pragma once

typedef void (*w_amq_consumer_callback_fn)(const char* /*pMessage*/);

#ifdef __cplusplus

enum class amq_connection_type
{
	AMQ_QUEUE = 0,
	AMQ_TOPIC
};

enum class amq_delivery_mode 
{
    AMQ_PERSISTENT = 0,
    AMQ_NON_PERSISTENT = 1
};

#else

typedef enum amq_connection_type
{
    AMQ_QUEUE = 0,
    AMQ_TOPIC
} amq_connection_type;

typedef enum amq_delivery_mode
{
    AMQ_PERSISTENT = 0,
    AMQ_NON_PERSISTENT = 1
} amq_delivery_mode;

#endif