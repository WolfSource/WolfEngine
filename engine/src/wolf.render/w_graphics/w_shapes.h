/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_shapes.h
	Description		 : A class which is responsible to render shapes
	Comment          : 
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_SHAPES_H__
#define __W_SHAPES_H__

#include "w_graphics_device_manager.h"
#include "w_render_pass.h"
#include <w_bounding.h>
#include <w_game_time.h>
#include <w_time_span.h>
#include <glm/mat4x4.hpp>

namespace wolf
{
	namespace graphics
	{
		enum w_plan { XY, XZ, YZ };

		class w_shapes_pimp;
		class w_shapes : public system::w_object
		{
		public:
			//create line shape 
			W_EXP w_shapes(
				_In_ const glm::vec3& pA, 
				_In_ const glm::vec3& pB, 
				_In_ const w_color& pColor);
			
			//create triangle shape 
			W_EXP w_shapes(
				_In_ const glm::vec3& pA, 
				_In_ const glm::vec3& pB, 
				_In_ const glm::vec3& pC, 
				_In_ const w_color& pColor);
			
			//create circle shape 
			W_EXP w_shapes(
				_In_ const glm::vec3& pCenter,
				_In_ const float& pRadius,
				_In_ const w_color& pColor,
				_In_ const w_plan& pPlan,
				_In_ const uint32_t& pResolution = 30);

			//create bounding box shape 
			W_EXP w_shapes(
				_In_ const wolf::system::w_bounding_box& pBoundingBox, 
				_In_ const w_color& pColor);
			
			//create bounding sphere shape 
			W_EXP w_shapes(
				_In_ const wolf::system::w_bounding_sphere& pBoundingSphere, 
				_In_ const w_color& pColor, 
				_In_ const uint32_t& pResolution = 30);

			//create coordinate system axis
			W_EXP w_shapes(_In_ const w_color& pColor);

			//destructor of w_shapes
			W_EXP virtual ~w_shapes();

			//load shapes render
			W_EXP W_RESULT load(
				_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
				_In_ const wolf::graphics::w_render_pass& pRenderPass,
				_In_ const wolf::graphics::w_viewport& pViewport,
				_In_ const wolf::graphics::w_viewport_scissor& pViewportScissor);

			//update uniform of shape
			W_EXP W_RESULT update(_In_ const glm::mat4& pWorldViewProjection);

            //draw shape
            W_EXP W_RESULT draw(_In_ const w_command_buffers* pCommandBuffer);
            
			W_EXP ULONG release();

		private:
			typedef	system::w_object                                _super;
			w_shapes_pimp*                                          _pimp;
		};
	}
}

#endif
