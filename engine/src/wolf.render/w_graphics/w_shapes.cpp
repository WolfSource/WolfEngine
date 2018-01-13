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

			HRESULT add_bounding_box(_In_ const wolf::content_pipeline::w_bounding_box& pBBox)
			{
				return S_OK;
			}

			void release()
			{
				
			}

        private:    

			struct vertex_position_color
			{
				//Type of VERTEX_POSITION_COLOR which contains 3 float for position and 4 float for color
				float												data[7];
			};
			struct shape
			{
				//The array of vertices for this shape
				std::vector<vertex_position_color>					vertices;

				//The number of lines to draw this shape
				int													line_count = 0;

				//The length of time to keep this shape visible
				float												life_time = 0;
			};

			//Gets a shape instance for a given line count and life time
			shape* get_shape_for_lines(_In_ uint32_t pLineCount, _In_ float pLifeTime)
			{
				shape* _shape = nullptr;

				/*
					We go through our cached list trying to find a shape that contains
					a large enough array to hold our desired line count. If we find such
					a shape, we move it from our cached list to our active list and break
					out of the loop.
				*/
				auto vertCount = pLineCount * 2;
				for (int i = 0; i < this->_cached_shapes.size(); ++i)
				{
					//7 is array size of vertices of shape
					if (7 >= vertCount)
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

					_shape->vertices.resize(vertCount);
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

HRESULT w_shapes::add_bounding_box(_In_ const wolf::content_pipeline::w_bounding_box& pBoundingBox)
{
	return (!this->_pimp) ? S_FALSE : this->_pimp->add_bounding_box(pBoundingBox);
}

ULONG w_shapes::release()
{
    if (_super::get_is_released()) return 1;
 
    //release the private implementation
    SAFE_RELEASE(this->_pimp);
    
    return _super::release();
}
