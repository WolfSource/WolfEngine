/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
    Name			 : w_log_config.h
    Description		 : structures of logger's configuration
    Comment          :
*/

#pragma once

typedef enum w_log_type
{
    W_INFO = 0,
    W_WARNING,
    W_ERROR
} w_log_type;

typedef struct w_log_config
{
	char*	            app_name = nullptr;
	char*	            log_path = nullptr;
	enum w_log_type	    flush_level = w_log_type::W_INFO;
	bool		        log_to_std_out = true;
} w_log_config;


