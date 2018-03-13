#include "w_render_pch.h"
#include "w_shapes.h"
#include "w_mesh.h"
#include "w_pipeline.h"
#include "w_shader.h"
#include "w_uniform.h"

namespace wolf
{
    namespace graphics
    {
		class w_shapes_pimp
		{
		public:
			//create line shape 
			w_shapes_pimp(
				_In_ const glm::vec3& pA, 
				_In_ const glm::vec3& pB, 
				_In_ const w_color& pColor):
				_shape_type(shape_type::LINE),
				_color(pColor),
				_bounding_box(nullptr),
				_bounding_sphere(nullptr),
				_name("w_shapes"),
				_gDevice(nullptr)
			{
				this->_points.push_back(pA);
				this->_points.push_back(pB);
			}

			//create triangle shape
			w_shapes_pimp(
				_In_ const glm::vec3& pA, 
				_In_ const glm::vec3& pB, 
				_In_ const glm::vec3& pC, 
				_In_ const w_color& pColor) :
				_shape_type(shape_type::TRIANGLE),
				_color(pColor),
				_bounding_box(nullptr),
				_bounding_sphere(nullptr),
				_name("w_shapes"),
				_gDevice(nullptr)
			{
				this->_points.push_back(pA);
				this->_points.push_back(pB);
				this->_points.push_back(pC);
			}

			//create circle shape
			w_shapes_pimp(
				_In_ const glm::vec3& pCenter, 
				_In_ const float& pRadius, 
				_In_ const w_color& pColor, 
				_In_ const w_plan& pPlan, 
				_In_ const uint32_t& pResolution) :
				_shape_type(shape_type::CIRCLE),
				_color(pColor),
				_bounding_box(nullptr),
				_bounding_sphere(nullptr),
				_circle_resolution(pResolution),
				_circle_center(pCenter),
				_circle_radius(pRadius),
				_circle_plan(pPlan),
				_name("w_shapes"),
				_gDevice(nullptr)
			{
			}

			//create box shape
			w_shapes_pimp(
				_In_ const wolf::system::w_bounding_box& pBoundingBox, 
				_In_ const w_color& pColor) :
				_shape_type(shape_type::BOX),
				_color(pColor),
				_bounding_sphere(nullptr),
				_name("w_shapes"),
				_gDevice(nullptr)
			{
				this->_bounding_box = new (std::nothrow) wolf::system::w_bounding_box();
				if (!this->_bounding_box)
				{
					V(W_FAILED, "allocating memory for w_bounding_box in w_shapes", this->_name, 3, false);
					return;
				}
				std::memcpy(&this->_bounding_box->min[0], &pBoundingBox.min[0], 3 * sizeof(float));
				std::memcpy(&this->_bounding_box->max[0], &pBoundingBox.max[0], 3 * sizeof(float));
			}

			//create sphere shape
			w_shapes_pimp(
				_In_ const wolf::system::w_bounding_sphere& pBoundingSphere, 
				_In_ const w_color& pColor, 
				_In_ const uint32_t& pResolution) :
				_shape_type(shape_type::SPHERE),
				_color(pColor),
				_bounding_box(nullptr),
				_sphere_resolution(pResolution),
				_name("w_shapes"),
				_gDevice(nullptr)
			{
				this->_bounding_sphere = new (std::nothrow) wolf::system::w_bounding_sphere();
				if (!this->_bounding_sphere)
				{
					V(W_FAILED, "allocating memory for _bounding_sphere in w_shapes", this->_name, 3, false);
					return;
				}
				std::memcpy(&this->_bounding_sphere->center[0], &pBoundingSphere.center[0], 3 * sizeof(float));
				this->_bounding_sphere->radius = pBoundingSphere.radius;
			}

			//create axis
			w_shapes_pimp(_In_ const w_color& pColor) :
				_shape_type(shape_type::AXIS),
				_color(pColor),
				_bounding_box(nullptr),
				_bounding_sphere(nullptr),
				_name("w_shapes"),
				_gDevice(nullptr)
			{
			}

			W_RESULT load(
				_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
				_In_ const wolf::graphics::w_render_pass& pRenderPass,
				_In_ const wolf::graphics::w_viewport& pViewport,
				_In_ const wolf::graphics::w_viewport_scissor& pViewportScissor)
			{
				const std::string _trace_info = this->_name + "::load";

				this->_gDevice = pGDevice;

				std::vector<float> _vertices;
				switch (_shape_type)
				{
				case shape_type::LINE:
					_generate_line_vertices(_vertices);
					break;
				case shape_type::TRIANGLE:
					_generate_triangle_vertices(_vertices);
					break;
				case shape_type::CIRCLE:
					_generate_circle_vertices(_vertices);
					break;
				case shape_type::BOX:
					_generate_bounding_box_vertices(_vertices);
					break;
				case shape_type::SPHERE:
					_generate_bounding_sphere_vertices(_vertices);
					break;
				case shape_type::AXIS:
					_generate_axis_vertices(_vertices);
					break;
				};

				this->_shape_drawer.set_vertex_binding_attributes(w_vertex_declaration::VERTEX_POSITION);
				auto _hr = this->_shape_drawer.load(
					this->_gDevice,
					_vertices.data(),
					static_cast<uint32_t>(_vertices.size() * sizeof(float)),
					static_cast<uint32_t>(_vertices.size()),
					nullptr,
					0);

				_vertices.clear();

				if (_hr == W_FAILED)
				{
					release();
					V(W_FAILED, "loading mesh", _trace_info, 3, true);
					return W_FAILED;
				}

				//loading vertex shaders
				_hr = this->_shader.load(_gDevice,
					content_path + L"shaders/shape.vert.spv",
					w_shader_stage_flag_bits::VERTEX_SHADER);
				if (_hr == W_FAILED)
				{
					release();
					V(W_FAILED, "loading vertex shader", _trace_info, 3, true);
					return W_FAILED;
				}

				//loading fragment shader
				_hr = this->_shader.load(_gDevice,
					content_path + L"shaders/shape.frag.spv",
					w_shader_stage_flag_bits::FRAGMENT_SHADER);
				if (_hr == W_FAILED)
				{
					release();
					V(W_FAILED, "loading fragment shader", _trace_info, 3, true);
					return W_FAILED;
				}

				//load vertex shader uniform
				_hr = this->_u0.load(_gDevice);
				if (_hr == W_FAILED)
				{
					release();
					V(W_FAILED, "loading WorldViewProjection uniform", _trace_info, 3, true);
				}
				
				_hr = this->_u1.load(_gDevice);
				if (_hr == W_FAILED)
				{
					release();
					V(W_FAILED, "loading color uniform", _trace_info, 3, true);
				}

				std::vector<w_shader_binding_param> _shader_params;

				w_shader_binding_param _shader_param;
				_shader_param.index = 0;
				_shader_param.type = w_shader_binding_type::UNIFORM;
				_shader_param.stage = w_shader_stage_flag_bits::VERTEX_SHADER;
				_shader_param.buffer_info = this->_u0.get_descriptor_info();
				_shader_params.push_back(_shader_param);

				_shader_param.index = 1;
				_shader_param.type = w_shader_binding_type::UNIFORM;
				_shader_param.stage = w_shader_stage_flag_bits::FRAGMENT_SHADER;
				_shader_param.buffer_info = this->_u1.get_descriptor_info();
				_shader_params.push_back(_shader_param);

				_hr = this->_shader.set_shader_binding_params(_shader_params);
				if (_hr == W_FAILED)
				{
					release();
					V(W_FAILED, "setting shader binding param", _trace_info, 3, true);
				}

				//loading pipeline cache
				std::string _pipeline_cache_name = "shape_pipeline_cache";
				if (w_pipeline::create_pipeline_cache(_gDevice, _pipeline_cache_name) == W_FAILED)
				{
					logger.error("could not create pipeline cache for w_shapes: line_pipeline_cache");
					_pipeline_cache_name.clear();
				}

				auto _descriptor_set_layout_binding = this->_shader.get_descriptor_set_layout();
				_hr = this->_pipeline.load(_gDevice,
					this->_shape_drawer.get_vertex_binding_attributes(),
					w_primitive_topology::LINE_LIST,
					&pRenderPass,
					&this->_shader,
					{ pViewport },
					{ pViewportScissor });
				if (_hr == W_FAILED)
				{
					release();
					V(W_FAILED, "creating solid pipeline", _trace_info, 3, true);
					return W_FAILED;
				}

				return set_color(this->_color);
			}

			W_RESULT update(_In_ const glm::mat4& pWorldViewProjection)
			{
				const std::string _trace_info = this->_name + "::update";

				//we must update uniform
				this->_u0.data.wvp = pWorldViewProjection;
				auto _hr = this->_u0.update();
				if (_hr == W_FAILED)
				{
					V(W_FAILED, "updating uniform WorldViewProjection", _trace_info, 3, false);
					return W_FAILED;
				}
				return W_PASSED;
			}

			W_RESULT set_color(_In_ w_color pColor)
			{
				const std::string _trace_info = this->_name + "::set_color";

				this->_color = pColor;

				//we must update uniform
				this->_u1.data.color.r = this->_color.r / 255.0f;
				this->_u1.data.color.g = this->_color.g / 255.0f;
				this->_u1.data.color.b = this->_color.b / 255.0f;
				this->_u1.data.color.a = this->_color.a / 255.0f;

				auto _hr = this->_u1.update();
				if (_hr == W_FAILED)
				{
					V(W_FAILED, "updating uniform color", _trace_info, 3, false);
					return W_FAILED;
				}
				return W_PASSED;
			}

			W_RESULT draw(_In_ const w_command_buffer& pCommandBuffer)
			{
				const std::string _trace_info = this->_name + "::draw";

				this->_pipeline.bind(pCommandBuffer);

				if (this->_shape_drawer.draw(pCommandBuffer, nullptr, 0, false, 0) == W_FAILED)
				{
					V(W_FAILED, "drawing shape", _trace_info, 3, false);
					return W_FAILED;
				}
				return W_PASSED;;
			}
            
			void release()
			{
				this->_name.clear();
                
				this->_shape_drawer.release();
				this->_shader.release();
				this->_pipeline.release();
				this->_u0.release();

				_points.clear();
				SAFE_DELETE(this->_bounding_box);
				SAFE_DELETE(this->_bounding_sphere);

				this->_gDevice = nullptr;
			}

        private:
			void _generate_line_vertices(_Inout_ std::vector<float>& pVertices)
			{
				if (this->_points.size() < 2) return;

				uint32_t _index = 0;
				const size_t _offset = 3;//3 floats for pos
				pVertices.resize(2 * _offset);//2 vertices

#pragma region fill in the vertices for the line

				std::memcpy(&pVertices[_index], &this->_points[0], _offset * sizeof(float));
				_index += _offset;

				std::memcpy(&pVertices[_index], &this->_points[1], _offset * sizeof(float));
				
#pragma endregion

			}

			void _generate_triangle_vertices(_Inout_ std::vector<float>& pVertices)
			{
				if (this->_points.size() < 3) return;

				uint32_t _index = 0;
				const size_t _offset = 3;//3 floats for pos
				pVertices.resize(6 * _offset);//6 vertices 
				
#pragma region fill in the vertices for the triangle

				std::memcpy(&pVertices[_index], &this->_points[0], _offset * sizeof(float));
				_index += _offset;

				std::memcpy(&pVertices[_index], &this->_points[1], _offset * sizeof(float));
				_index += _offset;

				std::memcpy(&pVertices[_index], &this->_points[1], _offset * sizeof(float));
				_index += _offset;

				std::memcpy(&pVertices[_index], &this->_points[2], _offset * sizeof(float));
				_index += _offset;

				std::memcpy(&pVertices[_index], &this->_points[2], _offset * sizeof(float));
				_index += _offset;

				std::memcpy(&pVertices[_index], &this->_points[0], _offset * sizeof(float));

#pragma endregion
			}

			void _generate_bounding_box_vertices(_Inout_ std::vector<float>& pVertices)
			{
				if (!this->_bounding_box) return;

				std::array<glm::vec3, 8> _corners;
				this->_bounding_box->get_corners(_corners);

				uint32_t _index = 0;
				const size_t _offset = 3;//3 floats for pos
				pVertices.resize(24 * _offset);//24 vertices 
				
#pragma region fill in the vertices for the bottom of the box

				std::memcpy(&pVertices[_index], &_corners[0], _offset * sizeof(float));
				_index += _offset;
				
				std::memcpy(&pVertices[_index], &_corners[1], _offset * sizeof(float));
				_index += _offset;

				std::memcpy(&pVertices[_index], &_corners[1], _offset * sizeof(float));
				_index += _offset;

				std::memcpy(&pVertices[_index], &_corners[2], _offset * sizeof(float));
				_index += _offset;

				std::memcpy(&pVertices[_index], &_corners[2], _offset * sizeof(float));
				_index += _offset;

				std::memcpy(&pVertices[_index], &_corners[3], _offset * sizeof(float));
				_index += _offset;

				std::memcpy(&pVertices[_index], &_corners[3], _offset * sizeof(float));
				_index += _offset;

				std::memcpy(&pVertices[_index], &_corners[0], _offset * sizeof(float));
				_index += _offset;

#pragma endregion

#pragma region fill in the vertices for the top of the box

				std::memcpy(&pVertices[_index], &_corners[4], _offset * sizeof(float));
				_index += _offset;
				
				std::memcpy(&pVertices[_index], &_corners[5], _offset * sizeof(float));
				_index += _offset;
				
				std::memcpy(&pVertices[_index], &_corners[5], _offset * sizeof(float));
				_index += _offset;
				
				std::memcpy(&pVertices[_index], &_corners[6], _offset * sizeof(float));
				_index += _offset;
				
				std::memcpy(&pVertices[_index], &_corners[6], _offset * sizeof(float));
				_index += _offset;
				
				std::memcpy(&pVertices[_index], &_corners[7], _offset * sizeof(float));
				_index += _offset;
				
				std::memcpy(&pVertices[_index], &_corners[7], _offset * sizeof(float));
				_index += _offset;
				
				std::memcpy(&pVertices[_index], &_corners[4], _offset * sizeof(float));
				_index += _offset;

#pragma endregion

#pragma region fill in the vertices for the sides of the box

				std::memcpy(&pVertices[_index], &_corners[0], _offset * sizeof(float));
				_index += _offset;
				
				std::memcpy(&pVertices[_index], &_corners[4], _offset * sizeof(float));
				_index += _offset;

				std::memcpy(&pVertices[_index], &_corners[1], _offset * sizeof(float));
				_index += _offset;

				std::memcpy(&pVertices[_index], &_corners[5], _offset * sizeof(float));
				_index += _offset;

				std::memcpy(&pVertices[_index], &_corners[2], _offset * sizeof(float));
				_index += _offset;

				std::memcpy(&pVertices[_index], &_corners[6], _offset * sizeof(float));
				_index += _offset;

				std::memcpy(&pVertices[_index], &_corners[3], _offset * sizeof(float));
				_index += _offset;

				std::memcpy(&pVertices[_index], &_corners[7], _offset * sizeof(float));

#pragma endregion

			}
            
			void _generate_bounding_sphere_vertices(_Inout_ std::vector<float>& pVertices)
			{
				if (!this->_bounding_sphere) return;

				const size_t _offset = 3;//3 floats for pos
				auto _sphere_line_count = (this->_sphere_resolution + 1) * 3;
				pVertices.resize(_sphere_line_count * 2 * _offset);

				const auto _two_pi = glm::two_pi<float>();
				//compute our step around each circle
				auto _step = _two_pi / (float)this->_sphere_resolution;

				//used to track the index into our vertex array
				size_t _index = 0;

				//create the loop on the XY plane
				for (float i = 0.0f; i < _two_pi; i += _step)
				{
					pVertices[_index] = std::cos(i)	* this->_bounding_sphere->radius + this->_bounding_sphere->center[0];
					pVertices[_index + 1] = std::sin(i) * this->_bounding_sphere->radius + this->_bounding_sphere->center[1];
					pVertices[_index + 2] = 0.0f * this->_bounding_sphere->radius + this->_bounding_sphere->center[2];
					_index += _offset;

					pVertices[_index] = std::cos(i + _step) * this->_bounding_sphere->radius + this->_bounding_sphere->center[0];
					pVertices[_index + 1] = std::sin(i + _step) * this->_bounding_sphere->radius + this->_bounding_sphere->center[1];
					pVertices[_index + 2] = 0.0f * this->_bounding_sphere->radius + this->_bounding_sphere->center[2];
					_index += _offset;
				}

				//create the loop on the XZ plane
				for (float i = 0.0f; i < _two_pi; i += _step)
				{
					pVertices[_index] = std::cos(i) * this->_bounding_sphere->radius + this->_bounding_sphere->center[0];
					pVertices[_index + 1] = 0.0f * this->_bounding_sphere->radius + this->_bounding_sphere->center[1];
					pVertices[_index + 2] = std::sin(i) * this->_bounding_sphere->radius + this->_bounding_sphere->center[2];
					_index += _offset;

					pVertices[_index] = std::cos(i + _step) * this->_bounding_sphere->radius + this->_bounding_sphere->center[0];
					pVertices[_index + 1] = 0.0f * this->_bounding_sphere->radius + this->_bounding_sphere->center[1];
					pVertices[_index + 2] = std::sin(i + _step) * this->_bounding_sphere->radius + this->_bounding_sphere->center[2];
					_index += _offset;
				}

				//create the loop on the YZ plane
				for (float i = 0.0f; i < _two_pi; i += _step)
				{
					pVertices[_index] = 0.0f * this->_bounding_sphere->radius + this->_bounding_sphere->center[0];
					pVertices[_index + 1] = std::cos(i) * this->_bounding_sphere->radius + this->_bounding_sphere->center[1];
					pVertices[_index + 2] = std::sin(i) * this->_bounding_sphere->radius + this->_bounding_sphere->center[2];
					_index += _offset;

					pVertices[_index] = 0.0f * this->_bounding_sphere->radius + this->_bounding_sphere->center[0];
					pVertices[_index + 1] = std::cos(i + _step) * this->_bounding_sphere->radius + this->_bounding_sphere->center[1];
					pVertices[_index + 2] = std::sin(i + _step) * this->_bounding_sphere->radius + this->_bounding_sphere->center[2];
					_index += _offset;
				}
			}

			void _generate_circle_vertices(_Inout_ std::vector<float>& pVertices)
			{
				const size_t _offset = 3;//3 floats for pos
				auto _circle_line_count = this->_circle_resolution + 1;
				pVertices.resize(_circle_line_count * 2 * _offset);

				const auto _two_pi = glm::two_pi<float>();
				//compute our step around each circle
				auto _step = _two_pi / (float)this->_circle_resolution;

				//used to track the index into our vertex array
				size_t _index = 0;

				switch (this->_circle_plan)
				{
				case wolf::graphics::XY:
					for (float i = 0.0f; i < _two_pi; i += _step)
					{
						pVertices[_index] = std::cos(i)	* this->_circle_radius + this->_circle_center[0];
						pVertices[_index + 1] = std::sin(i) * this->_circle_radius + this->_circle_center[1];
						pVertices[_index + 2] = 0.0f * this->_circle_radius + this->_circle_center[2];
						_index += _offset;

						pVertices[_index] = std::cos(i + _step) * this->_circle_radius + this->_circle_center[0];
						pVertices[_index + 1] = std::sin(i + _step) * this->_circle_radius + this->_circle_center[1];
						pVertices[_index + 2] = 0.0f * this->_circle_radius + this->_circle_center[2];
						_index += _offset;
					}
					break;
				case wolf::graphics::XZ:
					for (float i = 0.0f; i < _two_pi; i += _step)
					{
						pVertices[_index] = std::cos(i) * this->_circle_radius + this->_circle_center[0];
						pVertices[_index + 1] = 0.0f * this->_circle_radius + this->_circle_center[1];
						pVertices[_index + 2] = std::sin(i) * this->_circle_radius + this->_circle_center[2];
						_index += _offset;

						pVertices[_index] = std::cos(i + _step) * this->_circle_radius + this->_circle_center[0];
						pVertices[_index + 1] = 0.0f * this->_circle_radius + this->_circle_center[1];
						pVertices[_index + 2] = std::sin(i + _step) * this->_circle_radius + this->_circle_center[2];
						_index += _offset;
					}
					break;
				case wolf::graphics::YZ:
					for (float i = 0.0f; i < _two_pi; i += _step)
					{
						pVertices[_index] = 0.0f * this->_circle_radius + this->_circle_center[0];
						pVertices[_index + 1] = std::cos(i) * this->_circle_radius + this->_circle_center[1];
						pVertices[_index + 2] = std::sin(i) * this->_circle_radius + this->_circle_center[2];
						_index += _offset;

						pVertices[_index] = 0.0f * this->_circle_radius + this->_circle_center[0];
						pVertices[_index + 1] = std::cos(i + _step) * this->_circle_radius + this->_circle_center[1];
						pVertices[_index + 2] = std::sin(i + _step) * this->_circle_radius + this->_circle_center[2];
						_index += _offset;
					}
					break;
				}
			}

			void _generate_axis_vertices(_Inout_ std::vector<float>& pVertices)
			{
				uint32_t _index = 0;
				const size_t _offset = 3;//3 floats for pos
				pVertices.resize(34 * _offset);//28 vertices 

#pragma region fill in the vertices for X axis
				
				glm::vec3 _point(0);
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				_point.x = 5.0f;
				_point.y = 0.0f;
				_point.z = 0.0f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				//again Right(1,0,0) * 5
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				_point.x = 4.5f;
				_point.y = 0.5f;
				_point.z = 0.0f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				_point.x = 5.0f;
				_point.y = 0.0f;
				_point.z = 0.0f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				_point.x = 4.5f;
				_point.y = -0.5f;
				_point.z = 0.0f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				//create X
				_point.x = 5.5f;
				_point.y = -0.5f;
				_point.z = 0.0f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				_point.x = 6.0f;
				_point.y = 0.5f;
				_point.z = 0.0f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				_point.x = 6.0f;
				_point.y = -0.5f;
				_point.z = 0.0f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				_point.x = 5.5f;
				_point.y = 0.5f;
				_point.z = 0.0f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

#pragma endregion

#pragma region fill in the vertices for Y axis
				
				_point.x = 0.0f;
				_point.y = 0.0f;
				_point.z = 0.0f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				_point.x = 0.0f;
				_point.y = 5.0f;
				_point.z = 0.0f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				//again Up(0,1,0) * 5
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				_point.x = 0.5f;
				_point.y = 4.5f;
				_point.z = 0.0f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				_point.x = 0.0f;
				_point.y = 5.0f;
				_point.z = 0.0f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				_point.x = -0.5f;
				_point.y = 4.5f;
				_point.z = 0.0f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				//create Y
				_point.x = 0.0f;
				_point.y = 5.9f;
				_point.z = 0.0f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				_point.x = -0.3f;
				_point.y = 6.3f;
				_point.z = 0.0f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				_point.x = 0.0f;
				_point.y = 5.9f;
				_point.z = 0.0f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				_point.x = 0.3f;
				_point.y = 6.3f;
				_point.z = 0.0f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				_point.x = 0.0f;
				_point.y = 5.9f;
				_point.z = 0.0f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				_point.x = 0.0f;
				_point.y = 5.5f;
				_point.z = 0.0f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;
#pragma endregion

#pragma region fill in the vertices for Z axis

				_point.x = 0.0f;
				_point.y = 0.0f;
				_point.z = 0.0f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				_point.x = 0.0f;
				_point.y = 0.0f;
				_point.z = 5.0f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				//again (0,0,1) * 5
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				_point.x = 0.0f;
				_point.y = 0.5f;
				_point.z = 4.5f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				_point.x = 0.0f;
				_point.y = 0.0f;
				_point.z = 5.0f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				_point.x = 0.0f;
				_point.y = -0.5f;
				_point.z = 4.5f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				//create Z
				_point.x = 0.0f;
				_point.y = 0.5f;
				_point.z = 5.5f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				_point.x = 0.0f;
				_point.y = 0.5f;
				_point.z = 6.0f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				_point.x = 0.0f;
				_point.y = -0.5f;
				_point.z = 5.5f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				_point.x = 0.0f;
				_point.y = -0.5f;
				_point.z = 6.0f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				_point.x = 0.0f;
				_point.y = 0.5f;
				_point.z = 6.0f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

				_point.x = 0.0f;
				_point.y = -0.5f;
				_point.z = 5.5f;
				std::memcpy(&pVertices[_index], &_point[0], _offset * sizeof(float));
				_index += _offset;

#pragma endregion

			}

            enum shape_type
            {
				LINE, TRIANGLE, CIRCLE, BOX, SPHERE, AXIS
            } _shape_type;

			std::string                                             _name;
			std::shared_ptr<w_graphics_device>                      _gDevice;
			w_mesh													_shape_drawer;
			wolf::graphics::w_pipeline                              _pipeline;		
			wolf::graphics::w_shader                                _shader;

			struct U0
			{
				glm::mat4	wvp;
			};
			wolf::graphics::w_uniform<U0>                           _u0;

			struct U1
			{
				glm::vec4	color;
			};
			wolf::graphics::w_uniform<U1>                           _u1;

            
            wolf::system::w_bounding_box*                           _bounding_box;
			wolf::system::w_bounding_sphere*                        _bounding_sphere;
			uint32_t												_sphere_resolution;
			std::vector<glm::vec3>									_points;
			w_color													_color;

			uint32_t												_circle_resolution;
			glm::vec3												_circle_center;
			float													_circle_radius;
			w_plan													_circle_plan;
        };
    }
}

using namespace wolf::system;
using namespace wolf::graphics;

//create line shape 
w_shapes::w_shapes(
	_In_ const glm::vec3& pA, 
	_In_ const glm::vec3& pB, 
	_In_ const w_color& pColor) :
	_pimp(new w_shapes_pimp(pA, pB, pColor))
{
	_super::set_class_name("w_shapes");
}

w_shapes::w_shapes(
	_In_ const glm::vec3& pA,
	_In_ const glm::vec3& pB,
	_In_ const glm::vec3& pC,
	_In_ const w_color& pColor) :
	_pimp(new w_shapes_pimp(pA, pB, pC, pColor))
{
	_super::set_class_name("w_shapes");
}

w_shapes::w_shapes(_In_ const glm::vec3& pCenter,
	_In_ const float& pRadius,
	_In_ const w_color& pColor,
	_In_ const w_plan& pPlan,
	_In_ const uint32_t& pResolution) :
	_pimp(new w_shapes_pimp(pCenter, pRadius, pColor, pPlan, pResolution))
{
	_super::set_class_name("w_shapes");
}

w_shapes::w_shapes(
	_In_ const w_bounding_box& pBoundingBox, 
	_In_ const w_color& pColor) :
    _pimp(new w_shapes_pimp(pBoundingBox, pColor))
{
    _super::set_class_name("w_shapes");
}

//create bounding sphere shape 
w_shapes::w_shapes(
	_In_ const w_bounding_sphere& pBoundingSphere, 
	_In_ const w_color& pColor, 
	_In_ const uint32_t& pResolution) :
	_pimp(new w_shapes_pimp(pBoundingSphere, pColor, pResolution))
{
	_super::set_class_name("w_shapes");
}

w_shapes::w_shapes(_In_ const w_color& pColor) :
	_pimp(new w_shapes_pimp(pColor))
{
	_super::set_class_name("w_shapes");
}

w_shapes::~w_shapes()
{
    release();
}

W_RESULT w_shapes::load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
	_In_ const w_render_pass& pRenderPass,
	_In_ const w_viewport& pViewport,
	_In_ const w_viewport_scissor& pViewportScissor)
{
	return (!this->_pimp) ? W_FAILED : this->_pimp->load(pGDevice, pRenderPass, pViewport, pViewportScissor);
}

W_RESULT w_shapes::update(_In_ const glm::mat4& pWorldViewProjection)
{
	return (!this->_pimp) ? W_FAILED : this->_pimp->update(pWorldViewProjection);
}

W_RESULT w_shapes::draw(_In_ const w_command_buffer& pCommandBuffer)
{
	if (!this->_pimp) return W_FAILED;
	return this->_pimp->draw(pCommandBuffer);
}

ULONG w_shapes::release()
{
    if (_super::get_is_released()) return 1;
 
    //release the private implementation
    SAFE_RELEASE(this->_pimp);
    
    return _super::release();
}
