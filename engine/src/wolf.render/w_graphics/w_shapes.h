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
#include <w_plane.h>
#include <w_game_time.h>
#include <w_time_span.h>
#include <glm/mat4x4.hpp>

namespace wolf
{
	namespace graphics
	{
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
				_In_ const w_plane& pPlane,
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
				_In_ const w_render_pass& pRenderPass,
				_In_ const w_viewport& pViewport,
				_In_ const w_viewport_scissor& pViewportScissor);

			//update uniform of shape
			W_EXP W_RESULT update(_In_ const glm::mat4& pWorldViewProjection);

            //draw shape
            W_EXP W_RESULT draw(_In_ const w_command_buffer& pCommandBuffer);
            
			W_EXP ULONG release();


#ifdef __PYTHON__

			w_shapes()
			{
			}

			w_shapes(
				_In_ boost::python::list pA,
				_In_ boost::python::list pB,
				_In_ const w_color& pColor)
			{
				std::vector<float> _pa;
				if (!boost_list_to_std_vector(pA, _pa)) return;

				std::vector<float> _pb;
				if (!boost_list_to_std_vector(pB, _pb)) return;

				w_shapes(
					glm::vec3(_pa[0], _pa[1], _pa[2]),
					glm::vec3(_pb[0], _pb[1], _pb[2]), 
					pColor);

				_pa.clear();
				_pb.clear();
			}

			w_shapes(
				_In_ boost::python::list pA,
				_In_ boost::python::list pB,
				_In_ boost::python::list pC,
				_In_ const w_color& pColor)
			{
				std::vector<float> _pa;
				if (!boost_list_to_std_vector(pA, _pa)) return;

				std::vector<float> _pb;
				if (!boost_list_to_std_vector(pB, _pb)) return;

				std::vector<float> _pc;
				if (!boost_list_to_std_vector(pC, _pc)) return;

				w_shapes(
					glm::vec3(_pa[0], _pa[1], _pa[2]), 
					glm::vec3(_pb[0], _pb[1], _pb[2]), 
					glm::vec3(_pc[0], _pc[1], _pc[2]), 
					pColor);

				_pa.clear();
				_pb.clear();
				_pc.clear();
			}

			w_shapes(
				_In_ boost::python::list pCenter,
				_In_ const float& pRadius,
				_In_ const w_color& pColor,
				_In_ const w_plane& pPlane,
				_In_ const uint32_t& pResolution)
			{
				std::vector<float> _center;
				if (!boost_list_to_std_vector(pCenter, _center)) return;
				
				w_shapes(
					glm::vec3(_center[0], _center[1], _center[2]),
					pRadius,
					pColor,
					pPlane,
					pResolution);

				_center.clear();
			}

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

				pGDevice.reset();

				return _hr;
			}

			//update uniform of shape
			W_RESULT py_update(_In_ boost::python::list pWorldViewProjection)
			{
				std::vector<float> _mat_data;
				boost_list_to_std_vector(pWorldViewProjection, _mat_data);

				size_t _i = 0, _j = 0;
				glm::mat4 _mat;
				
				auto _size = _mat_data.size();
				if (_size > 16) _size = 16;

				for (size_t k = 0; k < _size; ++k)
				{
					_mat[_i][_j++] = _mat_data[k];
					if (_j == 4)
					{
						_j = 0;
						_i++;
					}
				}

				auto _hr = update(_mat);

				_mat_data.clear();

				return _hr;
			}

#endif

		private:
			typedef	system::w_object                                _super;
			w_shapes_pimp*                                          _pimp;
		};
	}
}

#include "python_exporter/py_shapes.h"

#endif
