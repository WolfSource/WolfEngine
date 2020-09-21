/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : c_obj.h
	Description		 : The base object of collada 
	Comment          :
*/

#ifndef __C_OBJ_H__
#define __C_OBJ_H__

#include <string>

namespace wolf
{
	namespace content_pipeline
	{
		namespace collada
		{
			struct c_obj
			{
				std::string c_name = "";
				std::string c_id = "";
				std::string c_sid = "";
			};
		}
	}
}

#endif