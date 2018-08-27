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
#include "w_viewport.h"
#include "w_render_pass.h"
#include <w_bounding.h>
#include <w_plane.h>
#include <w_game_time.h>
#include <w_time_span.h>
#include <glm_extension.h>

namespace wolf
{
	namespace render
	{
		namespace vulkan
		{
			class w_shapes_pimp;
			class w_shapes : public system::w_object
			{
			public:
				//create line shape 
				W_VK_EXP w_shapes(
					_In_ const glm::vec3& pA,
					_In_ const glm::vec3& pB,
					_In_ const w_color& pColor);

				//create triangle shape 
				W_VK_EXP w_shapes(
					_In_ const glm::vec3& pA,
					_In_ const glm::vec3& pB,
					_In_ const glm::vec3& pC,
					_In_ const w_color& pColor);

				//create circle shape 
				W_VK_EXP w_shapes(
					_In_ const glm::vec3& pCenter,
					_In_ const float& pRadius,
					_In_ const w_color& pColor,
					_In_ const w_plane& pPlane,
					_In_ const uint32_t& pResolution = 30);

				//create bounding box shape 
				W_VK_EXP w_shapes(
					_In_ const wolf::system::w_bounding_box& pBoundingBox,
					_In_ const w_color& pColor);

				//create bounding sphere shape 
				W_VK_EXP w_shapes(
					_In_ const wolf::system::w_bounding_sphere& pBoundingSphere,
					_In_ const w_color& pColor,
					_In_ const uint32_t& pResolution = 30);

				//create coordinate system axis
				W_VK_EXP w_shapes(_In_ const w_color& pColor);

				//destructor of w_shapes
				W_VK_EXP virtual ~w_shapes();

				//load shapes render
				W_VK_EXP W_RESULT load(
					_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
					_In_ const w_render_pass& pRenderPass,
					_In_ const w_viewport& pViewport,
					_In_ const w_viewport_scissor& pViewportScissor);

				//update uniform of shape
				W_VK_EXP W_RESULT update(_In_ const glm::mat4& pWorldViewProjection);

				//draw shape
				W_VK_EXP W_RESULT draw(_In_ const w_command_buffer& pCommandBuffer);

				W_VK_EXP ULONG release();


#ifdef __PYTHON__

				W_VK_EXP w_shapes()
				{
				}

				W_VK_EXP w_shapes(
					_In_ const glm::w_vec3& pA,
					_In_ const glm::w_vec3& pB,
					_In_ const w_color& pColor);

				W_VK_EXP w_shapes(
					_In_ const glm::w_vec3& pA,
					_In_ const glm::w_vec3& pB,
					_In_ const glm::w_vec3& pC,
					_In_ const w_color& pColor);

				W_VK_EXP w_shapes(
					_In_ const glm::w_vec3& pCenter,
					_In_ const float& pRadius,
					_In_ const w_color& pColor,
					_In_ const w_plane& pPlane,
					_In_ const uint32_t& pResolution);

				//load shapes render
				W_RESULT py_load(
					_In_ boost::shared_ptr<w_graphics_device>& pGDevice,
					_In_ const w_render_pass& pRenderPass,
					_In_ const wolf::graphics::w_viewport& pViewport,
					_In_ const wolf::graphics::w_viewport_scissor& pViewportScissor)
				{
					if (!pGDevice.get()) return W_FAILED;
					auto _gDevice = boost_shared_ptr_to_std_shared_ptr<w_graphics_device>(pGDevice);

					auto _hr = load(
						_gDevice,
						pRenderPass,
						pViewport,
						pViewportScissor);

					_gDevice.reset();

					return _hr;
				}

				//update uniform of shape
				W_RESULT py_update(_In_ glm::w_mat4x4 pWorldViewProjection)
				{
					return update(pWorldViewProjection.data());
				}

#endif

			private:
				typedef	system::w_object                                _super;
				w_shapes_pimp*                                          _pimp;
			};
		}
	}
}

#include "python_exporter/py_shapes.h"

#endif
