/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_occlusion_query.h
	Description		 : hardware occlusion query
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_OCCLUSION_QUERY_H__
#define __W_OCCLUSION_QUERY_H__

#include <w_graphics_device_manager.h>

namespace wolf
{
	namespace graphics
	{
        class w_occlusion_query_pimp;
        class w_occlusion_query : public system::w_object
        {
		public:
            //initialize occlusion query
            W_EXP W_RESULT initialize(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice);
            //release occlusion query
            W_EXP ULONG release() override;
        private:
            typedef system::w_object                        _super;
            w_occlusion_query_pimp*                         _pimp;
        };
	}
}

#endif
