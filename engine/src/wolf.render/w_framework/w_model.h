/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_model.h
	Description		 : Render wolf::content_pipeline::w_cpipeline_model using this class
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_MODEL_H__
#define __W_MODEL_H__

#include <w_graphics_device_manager.h>
#include <w_graphics/w_mesh.h>
#include <w_graphics/w_shader.h>
#include <w_graphics/w_buffer.h>
#include <w_graphics/w_command_buffers.h>
#include <w_cpipeline_model.h>
#include <w_vertex_declaration.h>

#ifdef __GNUC__
#pragma GCC visibility push(default)
#endif

namespace wolf
{
    namespace framework
    {
        template<typename T = wolf::content_pipeline::vertex_declaration_structs::vertex_position_uv>
        class w_model : public wolf::graphics::w_mesh
        {
        public:

            w_model(wolf::content_pipeline::w_cpipeline_model* pModelData = nullptr,
                _In_ bool pZup = false,
                _In_ bool pFlipU = false,
                _In_ bool pFlipV = true) :
                _instances_buffer(nullptr),
                _pipeline_model(nullptr),
                _z_up(pZup),
                _flip_u(pFlipU),
                _flip_v(pFlipV)
            {
                using namespace wolf::content_pipeline;

                if (pModelData)
                {
                    this->_pipeline_model = new w_cpipeline_model();
                    //copy memory
                    std::memcpy((void*)this->_pipeline_model, pModelData, sizeof(w_cpipeline_model));
                }
            }

            virtual ~w_model()
            {
                release();
            }

            HRESULT load(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
                _In_ wolf::graphics::w_shader* pShader,
                _In_ const wolf::graphics::w_render_pass* pRenderPass,
                _In_ const std::string& pPipelineCacheName)
            {
                if (this->_pipeline_model == nullptr || pShader == nullptr) return S_FALSE;

                this->_gDevice = pGDevice;

                //load meshes
                std::vector<content_pipeline::w_cpipeline_model::w_mesh*> _model_meshes;
                this->_pipeline_model->get_meshes(_model_meshes);

                std::vector<content_pipeline::w_cpipeline_model::w_instance_info> _model_instances;
                this->_pipeline_model->get_instances(_model_instances);
                if (_model_instances.size())
                {
                    update_instance_buffer(_model_instances.data(),
                        static_cast<UINT>(_model_instances.size() * sizeof(content_pipeline::w_cpipeline_model::w_instance_info)),
                        w_vertex_declaration::VERTEX_POSITION_UV_INSTANCE_VEC7_INT);
                }

                HRESULT _hr;
                uint8_t _error = 0;
                for (size_t i = 0; i < _model_meshes.size(); ++i)
                {
                    auto _model_mesh = _model_meshes[i];
                    auto _mesh = new wolf::graphics::w_mesh();

                    //prepare vertices
                    std::vector<float> _v_data;

                    if (std::is_same<T, wolf::content_pipeline::vertex_declaration_structs::vertex_position>::value)
                    {
                        //for (size_t j = 0; j < _model_mesh->vertices.size(); ++j)
                        //{
                        //    auto _pos = _model_mesh->vertices[j].position;
                        //    auto _uv = _model_mesh->vertices[j].uv;

                        //    //position
                        //    _v_data.push_back(_pos[0]);
                        //    _v_data.push_back(_pos[1]);
                        //    _v_data.push_back(_pos[2]);
                        //}
                        //_mesh->set_vertex_declaration_struct(this->_instances_count ?
                        //    w_mesh::w_vertex_declaration::INSTANCE_POSITION : 
                        //    w_mesh::w_vertex_declaration::POSITION);
                    }
                    else if (std::is_same<T, wolf::content_pipeline::vertex_declaration_structs::vertex_position_color>::value)
                    {
                        ////prepare vertex data
                        //for (size_t j = 0; j < _model_mesh->vertices.size(); ++j)
                        //{
                        //    auto _pos = _model_mesh->vertices[j].position;
                        //    auto _color = _model_mesh->vertices[j].color;

                        //    //position
                        //    _v_data.push_back(_pos[0]);
                        //    _v_data.push_back(_pos[1]);
                        //    _v_data.push_back(_pos[2]);

                        //    //color
                        //    _v_data.push_back(_color[0]);
                        //    _v_data.push_back(_color[1]);
                        //    _v_data.push_back(_color[2]);
                        //    _v_data.push_back(_color[3]);
                        //}
                        //_mesh->set_vertex_declaration_struct(this->_instances_count ?
                        //    w_mesh::w_vertex_declaration::INSTANCE_POSITION_COLOR :
                        //    w_mesh::w_vertex_declaration::POSITION_COLOR);
                    }
                    else if (std::is_same<T, wolf::content_pipeline::vertex_declaration_structs::vertex_position_uv>::value)
                    {
                        for (size_t j = 0; j < _model_mesh->vertices.size(); ++j)
                        {
                            auto _pos = _model_mesh->vertices[j].position;
                            auto _uv = _model_mesh->vertices[j].uv;

                            //position
                            _v_data.push_back(_pos[0]);
                            _v_data.push_back(_pos[1]);
                            _v_data.push_back(_pos[2]);

                            //uv
                            _v_data.push_back(this->_flip_u ? 1 - _uv[0] : _uv[0]);
                            _v_data.push_back(this->_flip_v ? 1 - _uv[1] : _uv[1]);
                        }
                        this->_vertex_declaration = w_vertex_declaration::VERTEX_POSITION_UV;
                    }
                    else if (std::is_same<T, wolf::content_pipeline::vertex_declaration_structs::vertex_position_uv_color>::value)
                    {
                        //for (size_t j = 0; j < _model_mesh->vertices.size(); ++j)
                        //{
                        //    auto _pos = _model_mesh->vertices[j].position;
                        //    auto _color = _model_mesh->vertices[j].color;
                        //    auto _uv = _model_mesh->vertices[j].uv;

                        //    //position
                        //    _v_data.push_back(_pos[0]);
                        //    _v_data.push_back(_pos[1]);
                        //    _v_data.push_back(_pos[2]);

                        //    //color
                        //    _v_data.push_back(_color[0]);
                        //    _v_data.push_back(_color[1]);
                        //    _v_data.push_back(_color[2]);
                        //    _v_data.push_back(_color[3]);

                        //    //uv
                        //    _v_data.push_back(this->_flip_u ? 1 - _uv[0] : _uv[0]);
                        //    _v_data.push_back(this->_flip_v ? 1 - _uv[1] : _uv[1]);
                        //}
                        //_mesh->set_vertex_declaration_struct(this->_instances_count ?
                        //    w_mesh::w_vertex_declaration::INSTANCE_POSITION_COLOR :
                        //    w_mesh::w_vertex_declaration::POSITION_COLOR);
                    }
                    else
                    {
                        _error += 1;
                        logger.error("Unsupported vertex layout of mesh #" + std::to_string(i) +
                            " for model: " + this->_pipeline_model->get_name());
                        continue;
                    }

                    //load mesh
                    _mesh->set_vertex_declaration_struct(this->_instances_count ? this->_instance_declaration:
                        this->_vertex_declaration);

                    auto _count = static_cast<UINT>(_v_data.size());
                    _hr = _mesh->load(this->_gDevice,
                        _v_data.data(),
                        _count * sizeof(float),
                        _count,
                        _model_mesh->indices.data(),
                        static_cast<UINT>(_model_mesh->indices.size()),
                        pShader,
                        pRenderPass,
                        pPipelineCacheName,
                        _z_up);
                    _v_data.clear();
                    if (_hr == S_FALSE)
                    {
                        _error += 1;
                        logger.error("Could not create mesh #" + std::to_string(i) +
                            " for model: " + this->_pipeline_model->get_name());
                        continue;
                    }
                    this->_meshes.push_back(_mesh);
                }

                return _error > 0 ? S_FALSE : S_OK;
            }

            HRESULT update_instance_buffer(_In_ const void* const pInstancedData,
                _In_ const UINT pInstancedSize, _In_ const w_vertex_declaration pInstanceVertexDeclaration)
            {
                using namespace wolf::graphics;

                if (!pInstancedData || pInstancedSize == 0) return S_FALSE;

                this->_instances_count = pInstancedSize;
                this->_instance_declaration = pInstanceVertexDeclaration;

                HRESULT _hr;

#pragma region staging buffer in DRAM
                w_buffer _staging_buffer;
                _hr = _staging_buffer.load_as_staging(this->_gDevice, pInstancedSize);
                if (_hr == S_FALSE)
                {
                    logger.error("Could not create staging instance buffer of model: " +
                        this->_pipeline_model->get_name());
                    return _hr;
                }

                _hr = _staging_buffer.set_data(pInstancedData);
                if (_hr == S_FALSE)
                {
                    logger.error("Setting staging instance buffer of model: " +
                        this->_pipeline_model->get_name());
                    return _hr;
                }

                _hr = _staging_buffer.bind();
                if (_hr == S_FALSE)
                {
                    logger.error("Could not bind to staging instance buffer of model: " +
                        this->_pipeline_model->get_name());
                    return _hr;
                }
#pragma endregion

#pragma region create VRAM buffer

                if (_instances_buffer == nullptr)
                {
                    this->_instances_buffer = new (std::nothrow) w_buffer();
                    if (!this->_instances_buffer)
                    {
                        logger.error("Error on creating instance buffer.");
                        return S_FALSE;
                    }

                    _hr = this->_instances_buffer->load(this->_gDevice,
                        pInstancedSize,
                        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

                    if (_hr == S_FALSE)
                    {
                        logger.error("Could not create instance buffer of model: " +
                            this->_pipeline_model->get_name());
                        return _hr;
                    }
                }
#pragma endregion

                //bind instance buffer
                _hr = this->_instances_buffer->bind();
                if (_hr == S_FALSE)
                {
                    logger.error("Could not bind to instance buffer of model: " +
                        this->_pipeline_model->get_name());
                    return _hr;
                }

                //create one command buffer
                auto _copy_command_buffer = new w_command_buffers();
                _copy_command_buffer->load(this->_gDevice, 1);

                _copy_command_buffer->begin(0);
                {
                    auto _copy_cmd = _copy_command_buffer->get_command_at(0);

                    VkBufferCopy _copy_region = {};

                    _copy_region.size = pInstancedSize;
                    vkCmdCopyBuffer(_copy_cmd,
                        _staging_buffer.get_handle(),
                        this->_instances_buffer->get_handle(),
                        1,
                        &_copy_region);
                }
                _copy_command_buffer->flush(0);

                SAFE_DELETE(_copy_command_buffer);

                _staging_buffer.release();

                return S_OK;
            }

            void render(_In_ const VkCommandBuffer& pCommandBuffer)
            {
                auto _instance_count = this->_instances_count;
                auto _instance_handle = this->_instances_buffer ? this->_instances_buffer->get_handle() : nullptr;

                std::for_each(this->_meshes.begin(), this->_meshes.end(),
                    [&pCommandBuffer, &_instance_handle, &_instance_count](_In_ w_mesh* pMesh)
                {
                    pMesh->render(pCommandBuffer, _instance_handle, _instance_count);
                });
            }

            virtual ULONG release() override
            {
                if (_super::get_is_released()) return 0;

                SAFE_RELEASE(this->_pipeline_model);
                for (auto _iter : this->_meshes)
                {
                    SAFE_RELEASE(_iter);
                }
                SAFE_RELEASE(this->_instances_buffer);
                this->_gDevice = nullptr;

                return _super::release();
            }

#pragma region Getters

            UINT get_instances_count() const
            {
                return this->_instances_count;
            }

            wolf::content_pipeline::w_transform_info get_transform() const
            {
                return this->_pipeline_model ? this->_pipeline_model->get_transform() :
                    wolf::content_pipeline::w_transform_info();
            }

#pragma endregion

#pragma region Setters
            void set_z_up(_In_ bool pZUp)
            {
                this->_z_up = pZUp;
            }
#pragma endregion

        private:
            typedef	wolf::system::w_object                              _super;

            std::shared_ptr<wolf::graphics::w_graphics_device>			_gDevice;
            wolf::content_pipeline::w_cpipeline_model*					_pipeline_model;
            std::vector<wolf::graphics::w_mesh*>                        _meshes;
            wolf::graphics::w_buffer*                                   _instances_buffer;
            UINT                                                        _instances_count;
            w_vertex_declaration                                        _vertex_declaration;
            w_vertex_declaration                                        _instance_declaration;
            bool                                                        _flip_u, _flip_v;
            bool                                                        _z_up;
        };
    }
}

#ifdef __GNUC__
#pragma GCC visibility pop
#endif

#endif
