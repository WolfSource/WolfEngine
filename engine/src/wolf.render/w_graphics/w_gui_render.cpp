#include "w_render_pch.h"
#include "w_gui_render.h"
#include "w_mesh.h"
#include "w_uniform.h"

#include <w_camera.h>

wolf::content_pipeline::w_camera camera;
wolf::graphics::w_uniform<glm::mat4> view_projection;

namespace wolf
{
    namespace graphics
    {
        class w_gui_render_pimp
        {
        public:
            w_gui_render_pimp() :
                _name("w_gui_render"),
                _texture(nullptr),
                _shader(nullptr),
                _copy_command_buffer(nullptr)
            {
                
            }

            ~w_gui_render_pimp()
            {
                release();
            }

            HRESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice)
            {
                view_projection.load(pGDevice);

                auto _hr = w_texture::load_to_shared_textures(pGDevice, content_path + L"textures/gui.png", &this->_texture);
                if (_hr == S_FALSE || !this->_texture)
                {
                    logger.error("Could not load texture for w_gui_render");
                }

                std::vector<w_shader_binding_param> _shader_params;

                w_shader_binding_param _shader_param;
                _shader_param.index = 0;
                _shader_param.uniform_info = view_projection.get_descriptor_info();
                _shader_param.stage = w_shader_stage::VERTEX_SHADER;
                _shader_param.type = w_shader_binding_type::UNIFORM;
                _shader_params.push_back(_shader_param);

                _shader_param.index = 1;
                _shader_param.sampler_info = this->_texture->get_descriptor_info();
                _shader_param.stage = w_shader_stage::FRAGMENT_SHADER;
                _shader_param.type = w_shader_binding_type::SAMPLER;
                _shader_params.push_back(_shader_param);


                _hr = w_shader::load_to_shared_shaders(pGDevice,
                    "gui",
                    content_path + L"shaders/gui.vert.spv",
                    content_path + L"shaders/gui.frag.spv",
                    _shader_params,
                    &this->_shader);
                if (_hr == S_FALSE || !this->_shader)
                {
                    V(S_FALSE, "loading shader", this->_name, 3);
                    return S_FALSE;
                }

                //create one command buffer
                this->_copy_command_buffer = new (std::nothrow) w_command_buffers();
                if (!this->_copy_command_buffer)
                {
                    V(S_FALSE, "allocating copy command buffer", this->_name, 3);
                    return S_FALSE;
                }
                _hr = this->_copy_command_buffer->load(pGDevice, 1);
                if (_hr == S_FALSE)
                {
                    V(S_FALSE, "loading copy command buffer", this->_name, 3);
                    return S_FALSE;
                }

                return S_OK;
            }

            HRESULT render(
                _In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                _In_ const w_render_pass* pRenderPass,
                _In_ const std::string& pPipelineCacheName,
                _In_ const VkCommandBuffer& pCommandBuffer,
                _In_ std::map<std::string, std::vector<wolf::gui::w_gui_vertex>>& pWidgetsVertices)
            {
                if (pRenderPass == nullptr)
                {
                    V(S_FALSE, "Render Pass can not be nullptr.", this->_name, 3);
                    return S_FALSE;
                }

                camera.set_translate(0, 0, 1.36f);
                camera.update_view();
                camera.update_projection();
                view_projection.data = camera.get_projection() * camera.get_view();
                view_projection.update();

                //load mesh
                HRESULT _hr = S_OK;
                for (auto _iter : pWidgetsVertices)
                {
                    auto _widget_name = _iter.first;
                    auto _widget_vertices = _iter.second;

                    if (!_widget_vertices.size()) continue;
                    
                    const std::vector<UINT> _indices = { 0, 1, 2, 0, 3, 1 };
                    std::vector<wolf::gui::w_gui_vertex> _vertices(4);
                    //std::vector<wolf::gui::w_gui_instance_vertex> _instance_vertices;

                    const float _left = -1.0f;
                    const float _top = -1.0f;
                    const float _right = 1.0f;
                    const float _down = 1.0f;

                    //create full screen quad, we will instnace from this one
                    _vertices[0].position[0] = _right;  _vertices[0].position[1] = _down;                  
                    _vertices[1].position[0] = _left;   _vertices[1].position[1] = _top; 
                    _vertices[2].position[0] = _left;   _vertices[2].position[1] = _down;
                    _vertices[3].position[0] = _right;  _vertices[3].position[1] = _top; 

                    //widget and child controls are instances
                    //for (size_t i = 0; i < _widget_vertices.size(); ++i)
                    //{
                       // wolf::gui::w_gui_instance_vertex _instance_vertex;
                        
                       // _instance_vertex.position[0] = _widget_vertices[i].position[0];
                        //_instance_vertex.position[1] = _widget_vertices[i].position[1];
                        //_instance_vertex.position[2] = _widget_vertices[i].position[2];
                        
                        //_instance_vertex.rotation[0] = _widget_vertices[i].rotation[0];
                        //_instance_vertex.rotation[1] = _widget_vertices[i].rotation[1];
                        //_instance_vertex.rotation[2] = _widget_vertices[i].rotation[2];
                        
                        //_instance_vertex.scale[0] = _widget_vertices[i].scale[0];
                        //_instance_vertex.scale[1] = _widget_vertices[i].scale[1];
                        
                        //_instance_vertex.left_top_color[0] = _widget_vertices[i].left_top_color[0];
                        //_instance_vertex.left_top_color[1] = _widget_vertices[i].left_top_color[1];
                        //_instance_vertex.left_top_color[2] = _widget_vertices[i].left_top_color[2];
                        //_instance_vertex.left_top_color[3] = _widget_vertices[i].left_top_color[3];

                        //_instance_vertex.left_down_color[0] = _widget_vertices[i].left_down_color[0];
                        //_instance_vertex.left_down_color[1] = _widget_vertices[i].left_down_color[1];
                        //_instance_vertex.left_down_color[2] = _widget_vertices[i].left_down_color[2];
                        //_instance_vertex.left_down_color[3] = _widget_vertices[i].left_down_color[3];

                        //_instance_vertex.right_top_color[0] = _widget_vertices[i].right_top_color[0];
                        //_instance_vertex.right_top_color[1] = _widget_vertices[i].right_top_color[1];
                        //_instance_vertex.right_top_color[2] = _widget_vertices[i].right_top_color[2];
                        //_instance_vertex.right_top_color[3] = _widget_vertices[i].right_top_color[3];

                        //_instance_vertex.right_down_color[0] = _widget_vertices[i].right_down_color[0];
                        //_instance_vertex.right_down_color[1] = _widget_vertices[i].right_down_color[1];
                        //_instance_vertex.right_down_color[2] = _widget_vertices[i].right_down_color[2];
                        //_instance_vertex.right_down_color[3] = _widget_vertices[i].right_down_color[3];

                        
                        //_instance_vertex.left_top_uv[0] = _widget_vertices[i].left_top_uv[0];
                        //_instance_vertex.left_top_uv[1] = _widget_vertices[i].left_top_uv[1];
                        
                        //_instance_vertex.left_down_uv[0] = _widget_vertices[i].left_down_uv[0];
                        //_instance_vertex.left_down_uv[1] = _widget_vertices[i].left_down_uv[1];
                        
                        //_instance_vertex.right_top_uv[0] = _widget_vertices[i].right_top_uv[0];
                        //_instance_vertex.right_top_uv[1] = _widget_vertices[i].right_top_uv[1];
                        
                        //_instance_vertex.right_down_uv[0] = _widget_vertices[i].right_down_uv[0];
                        //_instance_vertex.right_down_uv[1] = _widget_vertices[i].right_down_uv[1];
                        
                        //_instance_vertex.index = static_cast<UINT>(0);

                        //_instance_vertices.push_back(_instance_vertex);
                    //}

                    auto _iter_find = this->_widgets.find(_widget_name);
                    if (_iter_find == this->_widgets.end())
                    {
                        //there is no mesh and instance for this widget, so we need to create one
                        auto _mesh = new (std::nothrow) w_mesh();
                        if (!_mesh)
                        {
                            _hr = S_FALSE;
                            V(_hr, "allocating mesh for widget", this->_name, 3);
                            break;
                        }

                       // _mesh->set_vertex_declaration_struct(w_mesh::w_vertex_declaration::VERTEX_GUI_2D);
                        auto _count = static_cast<UINT>(_vertices.size());
                        auto _hr = _mesh->load(pGDevice,
                            _vertices.data(),
                            _count * sizeof(wolf::gui::w_gui_vertex),
                            _count,
                            _indices.data(),
                            static_cast<UINT>(_indices.size()),
                            this->_shader,
                            pRenderPass,
                            pPipelineCacheName,
                            false,
                            true);
                        if (_hr == S_FALSE)
                        {
                            _hr = S_FALSE;
                            V(_hr, "loading mesh", this->_name, 3);
                            break;
                        }

                        widget_render _widget_render;
                        _widget_render._widget_mesh = _mesh;

                        //if we have instances
                        //_widget_render._widget_childs_instances_size = static_cast<UINT>(_instance_vertices.size() *
                          //  sizeof(wolf::gui::w_gui_instance_vertex));
                        if (_widget_render._widget_childs_instances_size)
                        {
                          //  if (update_instance_buffer(pGDevice,
                           //     _instance_vertices.data(),
                           //     _widget_render._widget_childs_instances_size,
                           //     &_widget_render._widget_childs_instances) == S_FALSE)
                            //{
                              //  _widget_render._widget_childs_instances_size = 0;
                              //  SAFE_RELEASE(_widget_render._widget_childs_instances);
                              //  V(S_FALSE, "Error creating instance buffer for widget " + _widget_name, this->_name, 3);
                            //}
                        }
                        this->_widgets[_widget_name] = _widget_render;
                    }
                    else
                    {
                        //we found the widget, update instance vertices
                        auto _widget_render = _iter_find->second;
                        //if (_widget_render._widget_childs_instances_size)
                        //{
                            //if (update_instance_buffer(pGDevice,
                              //  _instance_vertices.data(),
                              //  _widget_render._widget_childs_instances_size,
                               // &_widget_render._widget_childs_instances) == S_FALSE)
                            //{
                                //release widget and remove it from widgets

                              //  _widget_render._widget_childs_instances_size = 0;
                              //  SAFE_RELEASE(_widget_render._widget_childs_instances);
                              //  V(S_FALSE, "Error creating instance buffer for widget " + _widget_name, this->_name, 3);
                                
                               // this->_widgets.erase(_widget_name);
                            //}
                        //}
                    }

                    _vertices.clear();
                    //_instance_vertices.clear();
                }

                if (_hr != S_OK) return _hr;

                //render each mesh of widgets
                if (!this->_widgets.size()) return S_OK;

                std::vector<std::string> _to_be_removed;
                std::for_each(this->_widgets.cbegin(), this->_widgets.cend(), 
                    [pCommandBuffer, pWidgetsVertices, &_to_be_removed](std::pair<std::string, widget_render> pIter)
                {
                    auto _widget_name = pIter.first;
                    auto _widget = pIter.second;

                    if (_widget._widget_mesh)
                    {
                        auto _iter = pWidgetsVertices.find(_widget_name);
                        if (_iter == pWidgetsVertices.end())
                        {
                            //mesh is no needed, it means the widget removed, so we can dispose it
                            _to_be_removed.push_back(_widget_name);
                        }
                        else
                        {
                            //render the mesh
                            auto _instance_handle = _widget._widget_childs_instances &&  _widget._widget_childs_instances_size ? 
                                _widget._widget_childs_instances->get_handle() : nullptr;

                            _widget._widget_mesh->render(
                                pCommandBuffer, 
                                _instance_handle, 
                                _widget._widget_childs_instances_size);
                        }
                    }
                });

                //remove unused meshes
                for (auto _name : _to_be_removed)
                {
                    auto _widget = this->_widgets.at(_name);
                    _widget.release();
                    this->_widgets.erase(_name);
                }
                _to_be_removed.clear();

                return S_OK;
            }

            ULONG release()
            {
                //release sampler
                SAFE_DELETE(this->_copy_command_buffer);
                this->_shader = nullptr;
                this->_texture = nullptr;
                for (auto _iter : this->_widgets)
                {
                    _iter.second.release();
                }
                this->_widgets.clear();

                return 1;
            }

        private:

            HRESULT update_instance_buffer(
                _In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                _In_ const void* const pInstancedData,
                _In_ const UINT pInstancedSize,
                _In_ w_buffer** pInstanceBuffer)
            {
                using namespace wolf::graphics;

                if (!pInstancedData || pInstancedSize == 0) return S_FALSE;

                HRESULT _hr;

#pragma region create DRAM instance buffer
                w_buffer _staging_buffer;
                _hr = _staging_buffer.load_as_staging(pGDevice, pInstancedSize);
                if (_hr == S_FALSE)
                {
                    V(_hr, "creating staging instance buffer", this->_name, 3);
                    return _hr;
                }

                _hr = _staging_buffer.set_data(pInstancedData);
                if (_hr == S_FALSE)
                {
                    V(_hr, "setting staging instance buffer data", this->_name, 3);
                    return _hr;
                }

                _hr = _staging_buffer.bind();
                if (_hr == S_FALSE)
                {
                    V(_hr, "binding staging instance buffer", this->_name, 3);
                    return _hr;
                }
#pragma endregion

#pragma region create VRAM instance buffer

                //release instance buffer
                if (!*pInstanceBuffer)
                {
                    auto _buffer = new (std::nothrow) w_buffer();
                    if (!_buffer)
                    {
                        V(_hr, "allcating instance buffer", this->_name, 3);
                        return S_FALSE;
                    }
                    *pInstanceBuffer = _buffer;

                    _hr = (*pInstanceBuffer)->load(pGDevice,
                        pInstancedSize,
                        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

                    if (_hr == S_FALSE)
                    {
                        V(_hr, "loading instance buffer", this->_name, 3);
                        return S_FALSE;
                    }
                }

#pragma endregion

                //bind instance buffer
                _hr = (*pInstanceBuffer)->bind();
                if (_hr == S_FALSE)
                {
                    V(_hr, "binding instance buffer", this->_name, 3);
                    return _hr;
                }

                this->_copy_command_buffer->begin(0);
                {
                    auto _copy_cmd = _copy_command_buffer->get_command_at(0);

                    VkBufferCopy _copy_region = {};

                    _copy_region.size = pInstancedSize;
                    vkCmdCopyBuffer(_copy_cmd,
                        _staging_buffer.get_handle(),
                        (*pInstanceBuffer)->get_handle(),
                        1,
                        &_copy_region);
                }
                this->_copy_command_buffer->flush(0);
                
                _staging_buffer.release();

                return S_OK;
            }


            std::string                                                        _name;
            wolf::graphics::w_texture*                                         _texture;
            wolf::graphics::w_shader*                                          _shader;
            wolf::graphics::w_command_buffers*                                 _copy_command_buffer;

            struct widget_render
            {
                wolf::graphics::w_mesh*                                        _widget_mesh = nullptr;
                UINT                                                           _widget_childs_instances_size = 0;
                wolf::graphics::w_buffer*                                      _widget_childs_instances = nullptr;

                void release()
                {
                    SAFE_RELEASE(this->_widget_mesh);
                    SAFE_RELEASE(this->_widget_childs_instances);
                    this->_widget_childs_instances_size = 0;
                }
            };

            //widgets
            std::map<std::string, widget_render>                                _widgets;
        };
    }
}

using namespace wolf::graphics;

w_gui_render::w_gui_render() :
    _pimp(new w_gui_render_pimp())
{
	_super::set_class_name("w_gui_render");
}

w_gui_render::~w_gui_render()
{
	release();
}

HRESULT w_gui_render::load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice)
{
    if (!this->_pimp) return S_FALSE;
    return this->_pimp->load(pGDevice);
}

HRESULT w_gui_render::render(
    _In_ const std::shared_ptr<w_graphics_device>& pGDevice,
    _In_ const w_render_pass* pRenderPass,
    _In_ const std::string& pPipelineCacheName,
    _In_ const VkCommandBuffer& pCommandBuffer,
    _In_ std::map<std::string, std::vector<wolf::gui::w_gui_vertex>>& pWidgetsVertices)
{
    if (!this->_pimp) return S_FALSE;
    return this->_pimp->render(pGDevice, pRenderPass, pPipelineCacheName, pCommandBuffer, pWidgetsVertices);
}

ULONG w_gui_render::release()
{
    if (_super::get_is_released()) return 0;

    SAFE_RELEASE(this->_pimp);

    return _super::release();
}

#pragma region Getters


#pragma endregion

#pragma region Setters

#pragma endregion
