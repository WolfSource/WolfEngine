/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_queue.h
	Description		 : graphice queue and index
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_QUEUE_H__
#define __W_QUEUE_H__

#include <w_graphics_headers.h>
#include <w_render_export.h>

namespace wolf
{
	namespace graphics
	{
        struct w_queue
        {
#ifdef __VULKAN__
            VkQueue        queue = 0;
#endif
            uint32_t       index = UINT32_MAX;
            
            W_EXP ULONG release();
        };
	}
}

#include "python_exporter/py_queue.h"

#endif
