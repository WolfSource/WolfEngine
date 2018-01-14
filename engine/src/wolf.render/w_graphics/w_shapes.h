/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_shapes.h
	Description		 : A class which is responsible to render shapes
	Comment          :
*/

#ifndef __W_SHAPES_H__
#define __W_SHAPES_H__

#include "w_graphics_device_manager.h"
#include "w_render_pass.h"
#include "w_frame_buffer.h"
#include <w_bounding.h>
#include <w_time_span.h>

namespace wolf
{
	namespace graphics
	{
		class w_shapes_pimp;
		class w_shapes : public system::w_object
		{
		public:
			W_EXP w_shapes();
			W_EXP virtual ~w_shapes();

			//load shapes render
			W_EXP HRESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
				_In_ const wolf::graphics::w_render_pass& pRenderPass,
				_In_ const wolf::graphics::w_viewport& pViewport,
				_In_ const wolf::graphics::w_viewport_scissor& pViewportScissor);

			//add bounding box
            W_EXP HRESULT add_bounding_box(
				_In_ wolf::system::w_bounding_box& pBoundingBox,
                _In_ const w_color& pColor,
                _In_ const wolf::system::w_time_span& pLifeTime);

			W_EXP ULONG release();

		private:
			typedef	system::w_object                                _super;
			w_shapes_pimp*                                          _pimp;
		};
	}
}

#endif
