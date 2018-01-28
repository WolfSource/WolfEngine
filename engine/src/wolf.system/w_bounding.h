/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_bounding.h
	Description		 : The bounding class
	Comment          :
*/

#ifndef __W_BOUNDING_H__
#define __W_BOUNDING_H__

#include "w_system_export.h"
#include <vector>
#include <array>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <msgpack.hpp>
#include "python_exporter/w_boost_python_helper.h"

namespace wolf
{
	namespace system
	{
		enum w_containment_type { DISJOINT, INTERSECTS, CONTAINS };

		//forward declaration
		struct w_bounding_sphere;

		struct w_bounding_box
		{
			float           min[3] = { 0 };
			float           max[3] = { 0 };

			float	        position[3] = { 0 };
			float	        rotation[3] = { 0 };

			//used for rendering to masked occulusion culling buffer
			float			vertices[108] = { 0 };

			//create bounding box from bounding sphere
			WSYS_EXP static w_bounding_box create_from_bounding_sphere(_In_ const w_bounding_sphere& pBoundingSphere);

			WSYS_EXP void generate_vertices();
			WSYS_EXP void merge(_In_ const w_bounding_box& pAdditional);
			WSYS_EXP bool intersects(_In_ const w_bounding_box& pBox);
			WSYS_EXP w_containment_type contains(_In_ const glm::vec3& pPoint);
			WSYS_EXP w_containment_type contains(_In_ const w_bounding_box& pBox);
			WSYS_EXP w_containment_type contains(_In_ const w_bounding_sphere& pSphere);
			WSYS_EXP void get_corners(_Inout_ std::array<glm::vec3, 8>& pCorners);

			MSGPACK_DEFINE(min, max, position, rotation);

#ifdef __PYTHON__

			//min
			boost::python::tuple py_get_min() { return w_boost_wrap_array(this->min, 3); }
			void py_set_min(_In_ boost::python::tuple pValue) { w_boost_extract_tuple(this->min[0], this->min[1], this->min[2], pValue); }

			//max
			boost::python::tuple py_get_max() { return w_boost_wrap_array(this->max, 3); }
			void py_set_max(_In_ boost::python::tuple pValue) { w_boost_extract_tuple(this->max[0], this->max[1], this->max[2], pValue); }

			//position
			boost::python::tuple py_get_position() { return w_boost_wrap_array(this->position, 3); }
			void py_set_position(_In_ boost::python::tuple pValue) { w_boost_extract_tuple(this->position[0], this->position[1], this->position[2], pValue); }

			//rotation
			boost::python::tuple py_get_rotation() { return w_boost_wrap_array(this->rotation, 3); }
			void py_set_rotation(_In_ boost::python::tuple pValue) { w_boost_extract_tuple(this->rotation[0], this->rotation[1], this->rotation[2], pValue); }

			//vertices
			boost::python::tuple py_get_vertices() { return w_boost_wrap_array(this->vertices, 108); }

			//corners
			boost::python::list py_get_corners()
			{
				boost::python::list _list;
				//get corners
				std::array<glm::vec3, 8> _corners;
				get_corners(_corners);
				//append to list
				for (auto _c : _corners)
				{
					_list.append(boost::python::make_tuple(_c.x, _c.y, _c.z));
				}
				return _list;
			}
			w_containment_type py_contains_point(_In_ boost::python::tuple pPoint)
			{
				boost::python::extract<float> _x(pPoint[0]);
				boost::python::extract<float> _y(pPoint[1]);
				boost::python::extract<float> _z(pPoint[2]);
				return contains(glm::vec3(
					_x.check() ? _x() : 0.0f,
					_y.check() ? _y() : 0.0f,
					_z.check() ? _z() : 0.0f));
			}
			w_containment_type py_contains_bounding_box(_In_ const w_bounding_box& pBox) { return contains(pBox); }
#endif
		};

		struct w_bounding_sphere
		{
			float center[3];
			float radius;

			WSYS_EXP static w_bounding_sphere create_from_bounding_box(_In_ const w_bounding_box& pBoundingBox);

			WSYS_EXP void merge(_In_ const w_bounding_sphere& pAdditional);
			WSYS_EXP bool intersects(_In_ const w_bounding_sphere& pSphere);
			WSYS_EXP bool intersects(_In_ const w_bounding_box& pBox);

			MSGPACK_DEFINE(center, radius);

#ifdef __PYTHON__
			//center
			boost::python::tuple py_get_center() { return w_boost_wrap_array(this->center, 3); }
			void py_set_center(_In_ boost::python::tuple pValue) { w_boost_extract_tuple(this->center[0], this->center[1], this->center[2], pValue); }

			//radius
			float py_get_radius() { return this->radius; }
			void py_set_radius(_In_ float pValue) { this->radius = pValue; }

			bool intersects_bounding_sphere(_In_ const w_bounding_sphere& pSphere) { return intersects(pSphere); }
#endif
		};

		struct w_bounding_frustum
		{
			WSYS_EXP std::array<glm::vec4, 6> get_plans() const;
			WSYS_EXP void update(_In_ const glm::mat4& pMatrix);
			WSYS_EXP bool intersects(_In_ const w_bounding_sphere& pSphere);
			WSYS_EXP bool intersects(_In_ const w_bounding_box& pBox);

			MSGPACK_DEFINE(_planes);

#ifdef __PYTHON__
			boost::python::list py_get_plans()
			{
				using namespace boost::python;
				list _list;
				for (size_t i = 0; i < 6; ++i)
				{
					auto _t = make_tuple(_planes[i][0], _planes[i][1], _planes[i][2], _planes[i][3]);
					_list.append(_t);
				}
				return _list;
			}
			void py_update(boost::python::tuple pMatrix)
			{
				glm::mat4 _mat4;
				w_boost_extract_tuple(
					_mat4[0][0], _mat4[0][1], _mat4[0][2], _mat4[0][3],
					_mat4[1][0], _mat4[1][1], _mat4[1][2], _mat4[1][3],
					_mat4[2][0], _mat4[2][1], _mat4[2][2], _mat4[2][3],
					_mat4[3][0], _mat4[3][1], _mat4[3][2], _mat4[3][3],
					pMatrix);

				update(_mat4);
			}

			bool py_intersects_bounding_sphere(_In_ const w_bounding_sphere& pSphere) { return intersects(pSphere); }
			bool py_intersects_bounding_box(_In_ const w_bounding_box& pBox) { return intersects(pBox); }
#endif

		private:
			enum w_frustum_side { LEFT = 0, RIGHT = 1, TOP = 2, BOTTOM = 3, BACK = 4, FRONT = 5 };
			float _planes[6][4];//6 plans with 4 floats(w,x,y,z)
		};
	}
}

#ifdef __PYTHON__
#include "python_exporter/w_bounding_py.h"
#endif

#endif
