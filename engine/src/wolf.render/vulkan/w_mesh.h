/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_mesh.h
	Description		 : Render a mesh
	Comment          : 
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_MESH_H__
#define __W_MESH_H__

#include "w_graphics_device_manager.h"
#include "w_texture.h"
#include "w_render_pass.h"
#include <glm/mat4x4.hpp>
#include <w_vertex_declaration.h>

namespace wolf
{
	namespace render
	{
		namespace vulkan
		{
			typedef enum w_vertex_declaration
			{
				NOT_DEFINED,
				//User defined
				USER_DEFINED,
				//Position(Float3)
				VERTEX_POSITION,
				//Position(Float3) + Color(Float4)
				VERTEX_POSITION_COLOR,
				//Position(Float3) + UV(Float2)
				VERTEX_POSITION_UV,
				//Position(Float3) + Float3
				VERTEX_POSITION_UV_INDEX,
				//Position(Float3) + Float2 + Float4
				VERTEX_POSITION_UV_COLOR,
				//Position(Float3) + Float3 + Float4
				VERTEX_POSITION_UV_INDEX_COLOR,
				//Position(Float3) + Float3 + Float4
				VERTEX_POSITION_NORMAL_COLOR,
				//Position(Float3) + Float3 + Float2
				VERTEX_POSITION_NORMAL_UV,
				//Position(Float3) + Float3 + Float3
				VERTEX_POSITION_NORMAL_UV_INDEX,
				//Position(Float3) + Float3 + Float2 + Float3 + Float3
				VERTEX_POSITION_NORMAL_UV_TANGENT_BINORMAL,
				//Position(Float3) + Float3 + Float3 + Float3 + Float3
				VERTEX_POSITION_NORMAL_UV_INDEX_TANGENT_BINORMAL,
				//Position(Float3) + Float3 + Float2 + Float3 + Float3 + Float3 + Float3
				VERTEX_POSITION_NORMAL_UV_TANGENT_BINORMAL_BLEND_WEIGHT_BLEND_INDICES,
				//Position(Float3) + Float3 + Float3 + Float3 + Float3 + Float3 + Float3
				VERTEX_POSITION_NORMAL_UV_INDEX_TANGENT_BINORMAL_BLEND_WEIGHT_BLEND_INDICES
			} w_vertex_declaration;

			typedef enum w_vertex_attribute : uint32_t
			{
				W_FLOAT,
				W_TEXTURE_INDEX,
				W_SCALE,
				W_VEC2,
				W_UV,
				W_VEC3,
				W_POS,
				W_ROT,
				W_NORM,
				W_TANGENT,
				W_BINORMAL,
				W_VEC4,
				W_COLOR,
				W_BLEND_WEIGHT,
				W_BLEND_INDICES
			} w_vertex_attribute;

			struct w_vertex_binding_attributes
			{
				w_vertex_declaration                                    declaration = w_vertex_declaration::NOT_DEFINED;
				std::map<uint32_t, std::vector<w_vertex_attribute>>     binding_attributes;

				w_vertex_binding_attributes() {}
				w_vertex_binding_attributes(_In_ const w_vertex_declaration& pDeclaration)
				{
					this->declaration = pDeclaration;

					std::vector<w_vertex_attribute> _attr;
					switch (this->declaration)
					{
					case w_vertex_declaration::NOT_DEFINED:
						return;
					case w_vertex_declaration::USER_DEFINED:
						break;
					case w_vertex_declaration::VERTEX_POSITION:
						_attr.push_back(w_vertex_attribute::W_VEC3);//position
						break;
					case w_vertex_declaration::VERTEX_POSITION_COLOR:
						_attr.push_back(w_vertex_attribute::W_VEC3);//position
						_attr.push_back(w_vertex_attribute::W_VEC4);//color
						break;
					case w_vertex_declaration::VERTEX_POSITION_NORMAL_COLOR:
						_attr.push_back(w_vertex_attribute::W_VEC3);//position
						_attr.push_back(w_vertex_attribute::W_VEC3);//normal
						_attr.push_back(w_vertex_attribute::W_VEC4);//color
						break;
					case w_vertex_declaration::VERTEX_POSITION_NORMAL_UV:
						_attr.push_back(w_vertex_attribute::W_VEC3);//position
						_attr.push_back(w_vertex_attribute::W_VEC3);//normal
						_attr.push_back(w_vertex_attribute::W_VEC2);//texture coordinate
						break;
					case w_vertex_declaration::VERTEX_POSITION_NORMAL_UV_INDEX:
						_attr.push_back(w_vertex_attribute::W_VEC3);//position
						_attr.push_back(w_vertex_attribute::W_VEC3);//normal
						_attr.push_back(w_vertex_attribute::W_VEC3);//texture coordinate with index of texture
						break;
					case w_vertex_declaration::VERTEX_POSITION_NORMAL_UV_TANGENT_BINORMAL:
						_attr.push_back(w_vertex_attribute::W_VEC3);//position
						_attr.push_back(w_vertex_attribute::W_VEC3);//normal
						_attr.push_back(w_vertex_attribute::W_VEC2);//texture coordinate
						_attr.push_back(w_vertex_attribute::W_VEC3);//tangent
						_attr.push_back(w_vertex_attribute::W_VEC3);//binormal
						break;
					case w_vertex_declaration::VERTEX_POSITION_NORMAL_UV_INDEX_TANGENT_BINORMAL:
						_attr.push_back(w_vertex_attribute::W_VEC3);//position
						_attr.push_back(w_vertex_attribute::W_VEC3);//normal
						_attr.push_back(w_vertex_attribute::W_VEC3);//texture coordinate with index of texture
						_attr.push_back(w_vertex_attribute::W_VEC3);//tangent
						_attr.push_back(w_vertex_attribute::W_VEC3);//binormal
						break;
					case w_vertex_declaration::VERTEX_POSITION_NORMAL_UV_TANGENT_BINORMAL_BLEND_WEIGHT_BLEND_INDICES:
						//TODO: Skinned
						/*_v.attributes.push_back(w_mesh::w_vertex_attribute::Vec3);
						_v.attributes.push_back(w_mesh::w_vertex_attribute::Vec3);
						_v.attributes.push_back(w_mesh::w_vertex_attribute::Vec2);
						_v.attributes.push_back(w_mesh::w_vertex_attribute::Vec3);
						_v.attributes.push_back(w_mesh::w_vertex_attribute::Vec3);*/
						break;
					case w_vertex_declaration::VERTEX_POSITION_NORMAL_UV_INDEX_TANGENT_BINORMAL_BLEND_WEIGHT_BLEND_INDICES:
						//TODO: Skinned
						/*_v.attributes.push_back(w_mesh::w_vertex_attribute::Vec3);
						 _v.attributes.push_back(w_mesh::w_vertex_attribute::Vec3);
						 _v.attributes.push_back(w_mesh::w_vertex_attribute::Vec2);
						 _v.attributes.push_back(w_mesh::w_vertex_attribute::Vec3);
						 _v.attributes.push_back(w_mesh::w_vertex_attribute::Vec3);*/
						break;
					case w_vertex_declaration::VERTEX_POSITION_UV:
						_attr.push_back(w_vertex_attribute::W_VEC3);//position
						_attr.push_back(w_vertex_attribute::W_VEC2);//texture coordinate
						break;
					case w_vertex_declaration::VERTEX_POSITION_UV_INDEX:
						_attr.push_back(w_vertex_attribute::W_VEC3);//position
						_attr.push_back(w_vertex_attribute::W_VEC3);//texture coordinate with index of texture
						break;
					case w_vertex_declaration::VERTEX_POSITION_UV_COLOR:
						_attr.push_back(w_vertex_attribute::W_VEC3);//position
						_attr.push_back(w_vertex_attribute::W_VEC2);//texture coordinate
						_attr.push_back(w_vertex_attribute::W_VEC4);//color
						break;
					case w_vertex_declaration::VERTEX_POSITION_UV_INDEX_COLOR:
						_attr.push_back(w_vertex_attribute::W_VEC3);//position
						_attr.push_back(w_vertex_attribute::W_VEC3);//texture coordinate with index of texture
						_attr.push_back(w_vertex_attribute::W_VEC4);//color
						break;
					}

					//assign new binding attributes
					for (auto& _iter : this->binding_attributes)
					{
						_iter.second.clear();
					}
					this->binding_attributes.clear();
					this->binding_attributes[0] = _attr;
				}
				w_vertex_binding_attributes(_In_ const  std::map<uint32_t, std::vector<w_vertex_attribute>>& pDeclaration)
				{
					this->binding_attributes = pDeclaration;
					this->declaration = w_vertex_declaration::USER_DEFINED;
				}

#ifdef __PYTHON__
				w_vertex_binding_attributes(_In_ boost::python::dict pDeclaration)
				{
					py_set_binding_attributes(pDeclaration);
				}

				boost::python::dict py_get_binding_attributes()
				{
					boost::python::dict _dictionary;
					for (auto _iter = binding_attributes.begin(); _iter != binding_attributes.end(); ++_iter)
					{
						boost::python::list _list;
						for (auto& _v : _iter->second)
						{
							_list.append(_v);
						}
						_dictionary[_iter->first] = _list;
					}
					return _dictionary;
				}

				void py_set_binding_attributes(_In_ boost::python::dict& pDic)
				{
					binding_attributes.clear();
					boost::python::list keys = pDic.keys();
					for (int i = 0; i < len(keys); ++i)
					{
						boost::python::extract<uint32_t> _extracted_key(keys[i]);
						if (!_extracted_key.check())
						{
							logger.error("invalid key for py_get_binding_attributes. type was not uint32_t");
							continue;
						}

						auto _key = _extracted_key();
						boost::python::extract<boost::python::list> _extracted_val(pDic[_key]);
						if (!_extracted_val.check())
						{
							logger.error("invalid key for py_get_binding_attributes. type was not list");
							continue;
						}

						auto _list = _extracted_val();
						std::vector<w_vertex_attribute> _values;
						for (size_t j = 0; j < len(_list); ++j)
						{
							boost::python::extract<w_vertex_attribute> _v(_list[j]);
							if (!_v.check())
							{
								logger.error("Value of list is not type of \"w_vertex_attribute\" for py_get_binding_attributes");
								continue;
							}
							_values.push_back(_v());
						}
						binding_attributes[_key] = _values;
					}
				}
#endif
			};

			class w_mesh_pimp;
			//Represents a 3D model mesh composed of multiple meshpart objects.
			class w_mesh : public system::w_object
			{
			public:
				W_EXP w_mesh();
				W_EXP virtual ~w_mesh();

				//load mesh
				W_EXP W_RESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
					_In_ const void* const pVerticesData,
					_In_ const uint32_t&  pVerticesSizeInBytes,
					_In_ const uint32_t& pVerticesCount,
					_In_ const uint32_t* const pIndicesData,
					_In_ const uint32_t& pIndicesCount,
					_In_ const bool& pUseDynamicBuffer = false);

				//update data of vertices and indices
				W_EXP W_RESULT update_dynamic_buffer(
					_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
					_In_ const void* const pVerticesData,
					_In_ const uint32_t& pVerticesSize,
					_In_ const uint32_t& pVerticesCount,
					_In_ const uint32_t* const pIndicesData,
					_In_ const uint32_t& pIndicesCount);

				/*
					Draw vertices and indices
					@param pCommandBuffer, The command buffer
					@param pInstanceHandle, The memory handle of instnace buffer
					@param pInstancesCount, Number of instnaces
					@param pFirstInstance, the index of first instance
					@param pIndirectDrawCommands, the indirect draw commands buffer
					@param pVertexOffset, The vertex offset
					@param pIndexCount, The count of indices for drawing, the default value (-1) will send whole count of index buffer for drawing with indexed buffer
					@param pFirstIndex, The first index in index buffer for drawing with indexed buffer
					@param pVertexCount, The count of vertices for drawing without indexed buffer
					@param pFirstVertex, The first vertex of vertex buffer for drawing without indexed buffer
				*/
				W_EXP W_RESULT draw(
					_In_ const w_command_buffer& pCommandBuffer,
					_In_ const w_buffer_handle* pInstanceHandle,
					_In_ const uint32_t& pInstancesCount,
					_In_ const uint32_t& pFirstInstance = 0,
					_In_ const w_indirect_draws_command_buffer* pIndirectDrawCommands = nullptr,
					_In_ const uint32_t& pVertexOffset = 0,
					_In_ const int& pIndexCount = -1,
					_In_ const uint32_t& pFirstIndex = 0,
					_In_ const int& pVertexCount = -1,
					_In_ const uint32_t& pFirstVertex = 0);

				//release all resources
				W_EXP virtual ULONG release() override;

#pragma region Getters

				W_EXP w_buffer_handle                                           get_vertex_buffer_handle() const;
				W_EXP w_buffer_handle                                           get_index_buffer_handle() const;
				W_EXP const uint32_t                                            get_vertices_count() const;
				W_EXP const uint32_t                                            get_indices_count() const;
				W_EXP w_texture*                                                get_texture() const;
				W_EXP const w_vertex_binding_attributes                         get_vertex_binding_attributes() const;

#pragma endregion

#pragma region Setters

				W_EXP void set_texture(_In_ w_texture* pTexture);
				W_EXP void set_vertex_binding_attributes(_In_ const w_vertex_binding_attributes& pVertexBindingAttributes);

#pragma endregion	

#ifdef __PYTHON__

				W_RESULT py_load(
					_In_ boost::shared_ptr<w_graphics_device>& pGDevice,
					_In_ boost::python::list pVerticesData,
					_In_ boost::python::list pIndicesData,
					_In_ bool pUseDynamicBuffer = false)
				{
					if (!pGDevice.get()) return W_FAILED;
					auto _gDevice = boost_shared_ptr_to_std_shared_ptr<w_graphics_device>(pGDevice);

					//get vertices data
					std::vector<float> _vertices;
					for (size_t i = 0; i < len(pVerticesData); ++i)
					{
						boost::python::extract<float> _v(pVerticesData[i]);
						if (_v.check())
						{
							_vertices.push_back(_v());
						}
					}

					//get indices data
					std::vector<uint32_t> _indices;
					for (size_t i = 0; i < len(pIndicesData); ++i)
					{
						boost::python::extract<uint32_t> _i(pIndicesData[i]);
						if (_i.check())
						{
							_indices.push_back(_i());
						}
					}

					const void* const _v_data = _vertices.data();
					auto _hr = load(_gDevice,
						_v_data,
						static_cast<uint32_t>(_vertices.size() * sizeof(float)),
						static_cast<uint32_t>(_vertices.size()),
						_indices.data(),
						static_cast<uint32_t>(_indices.size()),
						pUseDynamicBuffer);

					_gDevice.reset();
					return _hr;
				}

				W_RESULT py_update_dynamic_buffer(
					_In_ boost::shared_ptr<w_graphics_device>& pGDevice,
					_In_ boost::python::list pVerticesData,
					_In_ const uint32_t& pVerticesSize,
					_In_ const uint32_t& pVerticesCount,
					_In_ boost::python::list pIndicesData,
					_In_ const uint32_t& pIndicesCount)
				{
					if (!pGDevice.get()) return W_FAILED;
					auto _gDevice = boost_shared_ptr_to_std_shared_ptr<w_graphics_device>(pGDevice);

					//get vertices data
					std::vector<float> _vertices;
					for (size_t i = 0; i < len(pVerticesData); ++i)
					{
						boost::python::extract<float> _v(pVerticesData[i]);
						if (_v.check())
						{
							_vertices.push_back(_v());
						}
					}

					//get indices data
					std::vector<uint32_t> _indices;
					for (size_t i = 0; i < len(pIndicesData); ++i)
					{
						boost::python::extract<uint32_t> _i(pIndicesData[i]);
						if (_i.check())
						{
							_indices.push_back(_i());
						}
					}

					const void* const _v_data = _vertices.data();
					auto _hr = update_dynamic_buffer(
						_gDevice,
						_v_data,
						static_cast<uint32_t>(_vertices.size() * sizeof(float)),
						static_cast<uint32_t>(_vertices.size()),
						_indices.data(),
						static_cast<uint32_t>(_indices.size()));

					_gDevice.reset();
					return _hr;
				}

				W_RESULT py_draw(
					_In_ const w_command_buffer& pCommandBuffer,
					_In_ const w_buffer_handle* pInstanceHandle,
					_In_ const uint32_t& pInstancesCount,
					/////////_In_ const bool& pIndirectDraw,
					_In_ const uint32_t& pVertexOffset = 0)
				{
					return  draw(
						pCommandBuffer,
						pInstanceHandle,
						pInstancesCount,
						pIndirectDraw,
						pVertexOffset);
				}

				void py_set_texture(_In_ w_texture& pTexture)
				{
					set_texture(&pTexture);
				}
#endif

			private:
				typedef		system::w_object                        _super;
				w_mesh_pimp*                                        _pimp;
			};

		}
	}
}

#include "python_exporter/py_mesh.h"

#endif
