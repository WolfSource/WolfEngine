/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : main.cpp
	Description		 : This sample shows how to interact lua as script engine of Wolf Engine
	Comment          : Read more information about this sample on http://wolfsource.io/gpunotes/wolfengine/
*/

#include "pch.h"

//namespaces
using namespace wolf::system;

int c_function_bind(lua_State* pState)
{
    logger.write("c function called from lua");
    return 0;
}

//Entry point of program 
WOLF_MAIN()
{
    const wchar_t* _name = L"01_system-06_lua";
    WOLF_INIT(_name);

	logger.write(L"starting Wolf");

    std::wstring _path = wolf::system::io::get_current_directoryW() + L"../../../../content/scripts/samples/" + _name + L".lua";
    if (w_lua::load_file(_path.c_str()) == S_OK)
    {
        //bind lua function named "bind" to c function, the script will call bind from action function 
        w_lua::bind_to_cfunction(c_function_bind, "bind");
        w_lua::run();

        //get value of global int variable from lua
        int _var1 = -1;
        w_lua::get_global_variable<int>("var1", _var1);

        //get value of global string variable from lua
        std::string _var2 = "";
        w_lua::get_global_variable<std::string>("var2", _var2);

        //this will be error because var3 is type of number not string
        w_lua::get_global_variable<std::string>("var3", _var2);

        //although the wolf could log errors of lua, however you can get last error of lua using w_lua::get_last_error
        logger.write(w_lua::get_last_error());

        //this will be ok
        float _var3 = -1.0f;
        w_lua::get_global_variable<float>("var3", _var3);

        char _msg[256];
        sprintf_s(_msg, "results:\r\nvar3:%f", _var3);
        logger.write(_msg);

        //set new value for var1 then execute func1 which uses var1
        w_lua::set_global_variable<int>("var1", 5);
        
        //execute func of lua and get the results
        int _r1;
        std::string _r2;
        float _r3;
        w_lua::prepare_function("func");
        w_lua::set_parameter_function<int>(17);
        w_lua::execute_function<int, std::string, float>(_r1, _r2, _r3);
    }
    else
    {
        logger.write(L"could not load lua script from following path: " + _path);
    }

    logger.write(L"shutting down Wolf");

    _path.clear();
    w_lua::release();
    logger.release();

	return EXIT_SUCCESS;
}