/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_viewport.h
	Description		 : viewport
	Comment          :
*/

#ifndef __W_VIEWPORT_H__
#define __W_VIEWPORT_H__

#include <w_graphics_device_manager.h>

namespace wolf
{
	namespace render
	{
		namespace vulkan
		{
			struct w_viewport :
				public VkViewport
			{
				w_viewport()
				{
					this->x = 0.0f;
					this->y = 0.0f;
					this->width = 800.0f;
					this->height = 600.0f;
					this->minDepth = 0.0f;
					this->maxDepth = 1.0f;
				}

				void set(_In_ const w_command_buffer& pCommandBuffer)
				{
					vkCmdSetViewport(pCommandBuffer.handle, 0, 1, this);
				}
			};

			struct w_viewport_scissor :
				public VkRect2D
			{
				w_viewport_scissor()
				{
					this->offset.x = 0;
					this->offset.y = 0;
					this->extent.width = 800;
					this->extent.height = 600;
				}

				void set(_In_ const w_command_buffer& pCommandBuffer)
				{
					vkCmdSetScissor(pCommandBuffer.handle, 0, 1, this);
				}

#ifdef __PYTHON__
				w_point py_get_offset() const
				{
					auto _point = w_point();
					_point.x = this->offset.x;
					_point.y = this->offset.y;
					return _point;
				}
				void py_set_offset(_In_ const int32_t& pX, _In_ const int32_t& pY)
				{
					this->offset.x = pX;
					this->offset.y = pY;
				}

				w_point_t py_get_extent() const
				{
					auto _point = w_point_t();
					_point.x = this->extent.width;
					_point.y = this->extent.height;
					return _point;
				}
				void py_set_extent(_In_ const uint32_t& pWidth, _In_ const uint32_t& pHeight)
				{
					this->extent.width = pWidth;
					this->extent.height = pHeight;
				}
#endif
			};

		}
	}
}

#include "python_exporter/py_uniform.h"

#endif
