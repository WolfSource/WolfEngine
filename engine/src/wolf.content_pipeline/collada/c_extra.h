/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : c_extra.h
	Description		 : This class responsible to bind python and c++ functions callbacks 
	Comment          :
*/

#ifndef __C_EXTRA_H__
#define __C_EXTRA_H__

#include <string>
#include "c_obj.h"

namespace wolf
{
	namespace content_pipeline
	{
		namespace collada
		{
			struct c_value_obj : public c_obj
			{
				std::string value = "";
			};

			struct c_xsi_extra
			{
				std::string timing = "Second";
				unsigned int start = 0;
				unsigned int end = 10;
				unsigned int frame_rate = 30;
			};
		}
	}
}

#endif