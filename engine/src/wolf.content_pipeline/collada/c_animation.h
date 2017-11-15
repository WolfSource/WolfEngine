/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : c_animation.h
	Description		 : Declaration of collada's animation
	Comment          :
*/

#ifndef __C_ANIMATION_H__
#define __C_ANIMATION_H__

#include <string>
#include <vector>
#include <array>
#include "c_obj.h"
#include "c_extra.h"
#include <glm/matrix.hpp>
#include <w_logger.h>

namespace wolf
{
	namespace content_pipeline
	{
		namespace collada
		{
			enum DIMENTION_TYPE { X, Y, Z};
			enum TRANSFORM_TYPE { TRANSLATION, ROTATION, SCALE };
			enum INTERPOLATION_TYPE { LINEAR, BEZIER };

			struct c_semantic
			{
				std::string semantic = "";
				std::string source = "";
				int offset = 0;
			};

			struct c_sampler : c_obj
			{
				std::vector<c_semantic> semantics;

				ULONG release()
				{
					if (_is_released) return 1;
					this->_is_released = true;

					this->semantics.clear();

					return 0;
				}

			private:
				bool _is_released = false;
			};

			struct c_channel : c_obj
			{
				std::string source = "";
				std::string target = "";
			};

			struct c_technique_common
			{
				std::string accessor_source = "";
				int stride = 0;
				std::string param_name = "";
				std::string param_type = "";
			};

			struct c_source : c_obj
			{
				std::vector<float> float_array;
				std::vector<std::string> name_array;
				int stride = 0;
				c_technique_common technique_common;
				
				ULONG release()
				{
					if (_is_released) return 1;

					this->_is_released = true;

					this->float_array.clear();
					this->name_array.clear();

					return 0;
				}

			private:
				bool _is_released = false;
			};

			struct c_triangles
			{
				std::vector<UINT> indices;
				std::vector<c_semantic*> semantics;
				std::string material_name;

				ULONG release()
				{
					if (_is_released) return 1;
					this->_is_released = true;

					this->semantics.clear();

					return 0;
				}

			private:
				bool _is_released = false;
			};

			struct c_vertices
			{
				std::string id;
				std::vector<c_semantic*> semantics;

				ULONG release()
				{
					if (_is_released) return 1;
					this->_is_released = true;

					this->semantics.clear();

					return 0;
				}

			private:
				bool _is_released = false;

			};

			struct c_geometry
			{
				std::string id;
				std::string name;
				std::vector<c_source*> sources;
				std::vector<c_triangles*> triangles;
				c_vertices* vertices;

				ULONG release()
				{
					if (_is_released) return 1;
					
					std::for_each(this->sources.begin(), this->sources.end(), [](c_source* pValue)
					{
						SAFE_RELEASE(pValue);
					});
					std::for_each(this->triangles.begin(), this->triangles.end(), [](c_triangles* pValue)
					{
						SAFE_RELEASE(pValue);
					});
					
					this->sources.clear();
					this->triangles.clear();
					SAFE_RELEASE(this->vertices);

					_is_released = true;
					return 0;
				}

			private:
				bool _is_released = false;
			};

			struct c_collada_animation : c_obj
			{
				std::vector<c_source*> sources;
				c_channel* channel;
				c_sampler* sampler;

				ULONG release()
				{
					if (_is_released) return 1;

					std::for_each(this->sources.begin(), this->sources.end(), [](c_source* pValue)
					{
						SAFE_RELEASE(pValue);
					});
					this->sources.clear();
					SAFE_RELEASE(sampler);
					_is_released = true;
					return 0;
				}

			private:
				bool _is_released = false;
			};

			struct c_bezier_matrix
			{
				std::array<float, 2> get_control_point(int pStartIndex)
				{
					if (pStartIndex > lenght)
                    {
                        logger.error("pStartIndex out of index range");
                        return std::array<float, 2>() = { 0, 0 };
                    }
					std::array<float, 2> _points;
					_points[0] = data[pStartIndex];
					_points[1] = data[pStartIndex + 1];
					return  _points;
				}

				std::array<float, 2> get_c1()
				{
					std::array<float, 2> _points;
					_points[0] = data[2];
					_points[1] = data[3];
					return  _points;
				}

				static glm::mat4x4 get_bezier_coefficients_matrix()
				{
					glm::mat4x4 _mat;

					_mat[0][0] = -1; _mat[0][1] =  3; _mat[0][2] = -3; _mat[0][3] = 1;
					_mat[1][0] =  3; _mat[1][1] = -6; _mat[1][2] =  3; _mat[1][3] = 0;
					_mat[2][0] = -3; _mat[2][1] =  3; _mat[2][2] =  0; _mat[2][3] = 0;
					_mat[3][0] =  1; _mat[3][1] =  0; _mat[3][2] =  0; _mat[3][3] = 0;

					return _mat;
				}

				static const int lenght = 16;// 4*4
				float data[lenght];
				int stride = 0;
			};

			struct c_animation : c_obj
			{
			public:
				DIMENTION_TYPE dimension_type;
				TRANSFORM_TYPE transform_type;
				INTERPOLATION_TYPE interpolation_type;
				std::vector<float> times;
				std::vector<float> keys;
				c_bezier_matrix out_tangent;
				c_bezier_matrix in_tangent;

				float get_key(float pTime, _Out_ bool& pContainsAnimation)
				{
					pContainsAnimation = true;
					auto _key = 0.0f;

					switch (this->interpolation_type)
					{
					case INTERPOLATION_TYPE::LINEAR:
						_key = get_linear_key(pTime);
						break;
					case INTERPOLATION_TYPE::BEZIER:
						_key = get_bezier_key(pTime, pContainsAnimation);
						break;
					}
					return _key;
				}

                void release()
                {
                    this->times.clear();
                    this->keys.clear();
                }

			private:
				unsigned int get_time_index(float pTime)
				{
					unsigned int _index = 0;
					auto _size = this->times.size();
					for (size_t i = 0; i < _size; ++i)
					{
						if (i + 1 < _size)
						{
							if (pTime >= this->times[i] && pTime < this->times[i + 1])
							{
								_index = (unsigned int)i;
								break;
							}
						}
					}
					return _index;
				}

				float get_linear_key(float pTime)
				{
					auto _time_index = get_time_index(pTime);

					//get times and keys
					auto _time_1 = this->times[_time_index];
					auto _time_2 = this->times[_time_index + 1];
					auto _key_1 = this->keys[_time_index];
					auto _key_2 = this->keys[_time_index + 1];

					auto _delta_time = std::abs(pTime - _time_1) / _time_2;
					return _key_1 * (1 - _delta_time) + _key_2 * (_delta_time);
				}

                float get_bezier_key(float pTime, _Out_ bool& pContainsAnimation)
                {
                    pContainsAnimation = true;

                    auto _time_index = get_time_index(pTime);

                    auto _time_1 = this->times[_time_index];
                    auto _time_2 = this->times[_time_index + 1];

                    auto _delta_time = std::abs(pTime - _time_1) / (_time_2 - _time_1);
                    auto s = glm::vec4(
                        std::powf(_delta_time, 3.0f),
                        std::powf(_delta_time, 2),
                        std::powf(_delta_time, 1),
                        1);

                    auto _p0 = this->keys[_time_index];
                    auto _p1 = this->keys[_time_index + 1];

                    auto _c0 = this->out_tangent.data[_time_index * 2 + 1];
                    auto _c1 = this->in_tangent.data[_time_index * 2 + 3];

                    //tranform vect4 by matrix4x4
                    auto _coff = c_bezier_matrix::get_bezier_coefficients_matrix() * s;
                    auto _key = glm::dot(_coff, glm::vec4(_p0, _c0, _c1, _p1));

                    if (std::abs(_p0 - _c0) < 0.0001f && std::abs(_p0 - _c1) < 0.0001f && std::abs(_p0 - _p1) < 0.0001f && this->keys.size() <= 2)
                    {
                        pContainsAnimation = false;
                    }
                    return _key;
                }
			};

			struct c_animation_container
			{
				std::vector<c_animation> animations;
				c_xsi_extra xsi_extra;

                void release()
                {
                    for (auto& _iter : this->animations)
                    {
                        _iter.release();
                    }
                    this->animations.clear();
                }
			};
		}
	}
}

#endif
