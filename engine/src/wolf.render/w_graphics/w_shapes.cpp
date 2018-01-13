#include "w_render_pch.h"
#include "w_shapes.h"
#include "w_mesh.h"

namespace wolf
{
    namespace graphics
    {
        class w_shapes_pimp
        {
        public:
			w_shapes_pimp() :
				_name("w_shapes"),
				_gDevice(nullptr)
			{
			}

			HRESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice)
			{
				this->_gDevice = pGDevice;
				return S_OK;
			}

			HRESULT add_bounding_box(_In_ wolf::content_pipeline::w_bounding_box& pBoundingBox,
                                     _In_ const w_color pColor,
                                     _In_ const wolf::system::w_time_span pLifeTime)
			{
                std::array<glm::vec3, 8> _corners;
                pBoundingBox.get_corners(_corners);
             
                auto _shape = _get_shape_for_lines(12, pLifeTime);
                
                //Fill in the vertices for the bottom of the box
                _shape->vertices[0] = new (std::nothrow) vertex_position_color(_corners[0], pColor);
                if (!_shape->vertices[0]) return S_FALSE;
                _shape->vertices[1] = new (std::nothrow) vertex_position_color(_corners[1], pColor);
                if (!_shape->vertices[1]) return S_FALSE;
                _shape->vertices[2] = new (std::nothrow) vertex_position_color(_corners[1], pColor);
                 if (!_shape->vertices[2]) return S_FALSE;
                _shape->vertices[3] = new (std::nothrow) vertex_position_color(_corners[2], pColor);
                 if (!_shape->vertices[3]) return S_FALSE;
                _shape->vertices[4] = new (std::nothrow) vertex_position_color(_corners[2], pColor);
                 if (!_shape->vertices[4]) return S_FALSE;
                _shape->vertices[5] = new (std::nothrow) vertex_position_color(_corners[3], pColor);
                 if (!_shape->vertices[5]) return S_FALSE;
                _shape->vertices[6] = new (std::nothrow) vertex_position_color(_corners[3], pColor);
                 if (!_shape->vertices[6]) return S_FALSE;
                _shape->vertices[7] = new (std::nothrow) vertex_position_color(_corners[0], pColor);
                 if (!_shape->vertices[7]) return S_FALSE;
                
                //Fill in the vertices for the top of the box
                _shape->vertices[8] = new (std::nothrow) vertex_position_color(_corners[4], pColor);
                 if (!_shape->vertices[8]) return S_FALSE;
                _shape->vertices[9] = new (std::nothrow) vertex_position_color(_corners[5], pColor);
                 if (!_shape->vertices[9]) return S_FALSE;
                _shape->vertices[10] = new (std::nothrow) vertex_position_color(_corners[5], pColor);
                 if (!_shape->vertices[10]) return S_FALSE;
                _shape->vertices[11] = new (std::nothrow) vertex_position_color(_corners[6], pColor);
                 if (!_shape->vertices[11]) return S_FALSE;
                _shape->vertices[12] = new (std::nothrow) vertex_position_color(_corners[6], pColor);
                 if (!_shape->vertices[12]) return S_FALSE;
                _shape->vertices[13] = new (std::nothrow) vertex_position_color(_corners[7], pColor);
                 if (!_shape->vertices[13]) return S_FALSE;
                _shape->vertices[14] = new (std::nothrow) vertex_position_color(_corners[7], pColor);
                 if (!_shape->vertices[14]) return S_FALSE;
                _shape->vertices[15] = new (std::nothrow) vertex_position_color(_corners[4], pColor);
                 if (!_shape->vertices[15]) return S_FALSE;
                
                //Fill in the vertices for the vertical sides of the box
                _shape->vertices[16] = new (std::nothrow) vertex_position_color(_corners[0], pColor);
                 if (!_shape->vertices[16]) return S_FALSE;
                _shape->vertices[17] = new (std::nothrow) vertex_position_color(_corners[4], pColor);
                 if (!_shape->vertices[17]) return S_FALSE;
                _shape->vertices[18] = new (std::nothrow) vertex_position_color(_corners[1], pColor);
                 if (!_shape->vertices[18]) return S_FALSE;
                _shape->vertices[19] = new (std::nothrow) vertex_position_color(_corners[5], pColor);
                 if (!_shape->vertices[19]) return S_FALSE;
                _shape->vertices[20] = new (std::nothrow) vertex_position_color(_corners[2], pColor);
                 if (!_shape->vertices[20]) return S_FALSE;
                _shape->vertices[21] = new (std::nothrow) vertex_position_color(_corners[6], pColor);
                 if (!_shape->vertices[21]) return S_FALSE;
                _shape->vertices[22] = new (std::nothrow) vertex_position_color(_corners[3], pColor);
                 if (!_shape->vertices[22]) return S_FALSE;
                _shape->vertices[23] = new (std::nothrow) vertex_position_color(_corners[7], pColor);
                 if (!_shape->vertices[23]) return S_FALSE;
                
				return S_OK;
			}

            void draw(_In_ const VkCommandBuffer& pCommandBuffer, _In_ const wolf::system::w_game_time pGameTime)
            {
                //Calculate the total number of vertices of active shapes
                int _vertex_count = 0;
                for (auto shape : this->_active_shapes)
                {
                    _vertex_count += shape->line_count * 2;
                }
                
                //If we have some vertices to draw
                if (_vertex_count > 0)
                {
                    // Make sure our array is large enough
                    if (_vertices.size() < _vertex_count)
                    {
                        //resize buffer
                        _vertices.resize(_vertex_count * 2);
                    }
                    
                    // Now go through the shapes again to move the vertices to our array
                    int _line_count = 0;
                    int _vert_index = 0;
                    for (auto shape : this->_active_shapes)
                    {
                        _line_count += shape->line_count;
                        int _shape_verts = shape->line_count * 2;
                        for (int i = 0; i < _shape_verts; i++, _vert_index+=7)
                        {
                            _vertices[_vert_index] = shape->vertices[i]->data[0];
                            _vertices[_vert_index + 1] = shape->vertices[i]->data[1];
                            _vertices[_vert_index + 2] = shape->vertices[i]->data[2];
                            _vertices[_vert_index + 3] = shape->vertices[i]->data[3];
                            _vertices[_vert_index + 4] = shape->vertices[i]->data[4];
                            _vertices[_vert_index + 5] = shape->vertices[i]->data[5];
                            _vertices[_vert_index + 6] = shape->vertices[i]->data[6];
                            _vertices[_vert_index + 7] = shape->vertices[i]->data[7];
                        }
                    }
                    
                    //set pipeline
                    
                    // We draw in a loop because the Reach profile only supports 65,535 primitives. While it's
                    // not incredibly likely, if a game tries to render more than 65,535 lines we don't want to
                    // crash. We handle this by doing a loop and drawing as many lines as we can at a time, capped
                    // at our limit. We then move ahead in our vertex array and draw the next set of lines.
                    int _vertex_offset = 0;
                    while (_line_count > 0)
                    {
                        //figure out how many lines we're going to draw
                        int linesToDraw = std::min(_line_count, 65535);
                        
                        // Draw the lines
                        this->_shapes_drawer.update_dynamic_buffer(
                            this->_gDevice,
                            this->_vertices.data(),
                            sizeof(vertex_position_color),
                            this->_vertices.size(),
                            nullptr,
                            0);
                        this->_shapes_drawer.draw(pCommandBuffer, nullptr, 1, false);
                        //graphics.DrawUserPrimitives(PrimitiveType.LineList, verts, vertexOffset, linesToDraw);
                        
                        //move our vertex offset ahead based on the lines we drew
                        _vertex_offset += linesToDraw * 2;
                        
                        //remove these lines from our total line count
                        _line_count -= linesToDraw;
                    }
                }
                
                // Go through our active shapes and retire any shapes that have expired to the
                // cache list.
//                bool resort = false;
//                for (int i = this->_active_shapes.size() - 1; i >= 0; i--)
//                {
//                    auto s = this->_active_shapes[i];
//                    s.Lifetime -= (float)gameTime.ElapsedGameTime.TotalSeconds;
//                    if (s.Lifetime <= 0)
//                    {
//                        cachedShapes.Add(s);
//                        activeShapes.RemoveAt(i);
//                        resort = true;
//                    }
//                }
                
                // If we move any shapes around, we need to resort the cached list
                // to ensure that the smallest shapes are first in the list.
//                if (resort)
//                {
//                    th.Sort(CachedShapesSort);
//                }
            }
            
			void release()
			{
                this->_name.clear();
                
                for (auto iter : this->_cached_shapes)
                {
                    SAFE_RELEASE(iter);
                }
                this->_cached_shapes.clear();
                for (auto iter : this->_active_shapes)
                {
                    SAFE_RELEASE(iter);
                }
                this->_active_shapes.clear();
                
                _vertices.clear();
                
                //release shapes drawer
                this->_shapes_drawer.release();
                
                this->_gDevice = nullptr;
			}

        private:    

			struct vertex_position_color
			{
				//Type of VERTEX_POSITION_COLOR which contains 3 float for position and 4 float for color
				float												data[7];
                
                vertex_position_color(_In_ glm::vec3 pPosition, _In_ w_color pColor)
                {
                    //position
                    data[0] = pPosition.x;
                    data[1] = pPosition.y;
                    data[2] = pPosition.z;
                    
                    //color
                    data[3] = (float)pColor.r / 255.f;
                    data[4] = (float)pColor.g / 255.f;
                    data[5] = (float)pColor.b / 255.f;
                    data[6] = (float)pColor.a / 255.f;
                }
			};
            
			struct shape
			{
				//The array of vertices for this shape
				std::vector<vertex_position_color*>					vertices;

				//The number of lines to draw this shape
				uint32_t											line_count = 0;

				//The length of time to keep this shape visible
                system::w_time_span									life_time = system::w_time_span::zero();
                
                ULONG release()
                {
                    for (auto& iter : vertices)
                    {
                        SAFE_DELETE(iter);
                    }
                    this->vertices.clear();
                    this->line_count = 0;
                    this->life_time = system::w_time_span::zero();
                    
                    return 0;
                }
			};

			//Gets a shape instance for a given line count and life time
			shape* _get_shape_for_lines(_In_ uint32_t pLineCount, _In_ system::w_time_span pLifeTime)
			{
				shape* _shape = nullptr;

				/*
					We go through our cached list trying to find a shape that contains
					a large enough array to hold our desired line count. If we find such
					a shape, we move it from our cached list to our active list and break
					out of the loop.
				*/
				auto _vert_count = pLineCount * 2;
				for (int i = 0; i < this->_cached_shapes.size(); ++i)
				{
					//7 is array size of vertices of shape
					if (7 >= _vert_count)
					{
						_shape = this->_cached_shapes[i];
						this->_cached_shapes.erase(this->_cached_shapes.begin() + i);
						this->_active_shapes.push_back(_shape);
						break;
					}
				}

				/*
					If we didn't find a shape in our cache, means pointer to memory refers to null,
					we create a new shape and add it to the active list.
				*/
				if (_shape == nullptr)
				{
					_shape = new (std::nothrow) shape();
					if (!_shape)
					{
						logger.error("Error on allocating memory for structure of shape");
						return nullptr;
					}

					_shape->vertices.resize(_vert_count);
					this->_active_shapes.push_back(_shape);
				}

				// Set the line count and lifetime of the shape based on our parameters.
				_shape->line_count = pLineCount;
				_shape->life_time = pLifeTime;

				return _shape;
			}


			std::string                                             _name;
			std::shared_ptr<w_graphics_device>                      _gDevice;
			//Cached shapes responsible to reuse shape instances 
			std::vector<shape*>										_cached_shapes;
			//Active shapes contain shapes
			std::vector<shape*>										_active_shapes;
            //vertices
            std::vector<float>                                      _vertices;
			//Used for drawing all shapes in one draw call
			w_mesh													_shapes_drawer;

        };
    }
}

using namespace wolf::graphics;

w_shapes::w_shapes() : _pimp(new w_shapes_pimp())
{
    _super::set_class_name("w_shapes");
}

w_shapes::~w_shapes()
{
    release();
}

HRESULT w_shapes::load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice)
{
	return (!this->_pimp) ? S_FALSE : this->_pimp->load(pGDevice);
}

HRESULT w_shapes::add_bounding_box(_In_ wolf::content_pipeline::w_bounding_box& pBoundingBox,
                                   _In_ const w_color& pColor,
                                   _In_ const wolf::system::w_time_span& pLifeTime)
{
	return (!this->_pimp) ? S_FALSE : this->_pimp->add_bounding_box(pBoundingBox, pColor, pLifeTime);
}

ULONG w_shapes::release()
{
    if (_super::get_is_released()) return 1;
 
    //release the private implementation
    SAFE_RELEASE(this->_pimp);
    
    return _super::release();
}
