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
#include "w_boost_python_helper.h"

namespace wolf
{
	namespace system
	{
        enum w_containment_type { DISJOINT, INTERSECTS, CONTAINS };

        //forward declaration
        struct w_bounding_sphere;

		struct w_bounding_box
		{
			float           min[3] = { 0,0,0 };
			float           max[3] = { 0,0,0 };

			float	        position[3] = { 0,0,0 };
			float	        rotation[3] = { 0,0,0 };

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

			boost::python::tuple py_min() { return w_boost_wrap_array(this->min, 3); }
			boost::python::tuple py_max() { return w_boost_wrap_array(this->max, 3); }
			boost::python::tuple py_position() { return w_boost_wrap_array(this->position, 3); }
			boost::python::tuple py_rotation() { return w_boost_wrap_array(this->rotation, 3); }
			boost::python::tuple py_vertices() { return w_boost_wrap_array(this->vertices, 108); }
			w_containment_type py_contains_point(boost::python::tuple pPoint)
			{
				boost::python::extract<float> _x = pPoint[0];
				boost::python::extract<float> _y = pPoint[1];
				boost::python::extract<float> _z = pPoint[2];
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
        };

		struct w_bounding_frustum
		{
			WSYS_EXP std::array<glm::vec4, 6> get_plans() const;
			WSYS_EXP void update(_In_ const glm::mat4& pMatrix);
			WSYS_EXP bool intersects(_In_ const w_bounding_sphere& pSphere);
			WSYS_EXP bool intersects(_In_ const w_bounding_box& pBox);

			MSGPACK_DEFINE(_planes);

		private:
			enum w_frustum_side { LEFT = 0, RIGHT = 1, TOP = 2, BOTTOM = 3, BACK = 4, FRONT = 5 };
			float _planes[6][4];//6 plans with 4 floats(w,x,y,z)
		};

#ifdef __PYTHON__
		
		namespace pywolf
		{
			static void w_bounding_py_export()
			{
				using namespace boost::python;

				enum_<w_containment_type>("w_containment_type")
					.value("DISJOINT", w_containment_type::DISJOINT)
					.value("INTERSECTS", w_containment_type::INTERSECTS)
					.value("CONTAINS", w_containment_type::CONTAINS)
					.export_values()
					;

				// Create the Python type object for our extension class and define __init__ function.
				class_<w_bounding_box>("w_bounding_box")
					.def_readwrite("min", &w_bounding_box::py_min, "min point of w_bounding_box")
					.def_readwrite("max", &w_bounding_box::py_max, "max point of w_bounding_box")
					.def_readwrite("position", &w_bounding_box::py_position, "position of w_bounding_box")
					.def_readwrite("rotation", &w_bounding_box::py_rotation, "rotation of w_bounding_box")
					.def_readwrite("vertices", &w_bounding_box::py_vertices, "used for rendering to masked occulusion culling buffer")
					.def("generate_vertices_indices", &w_bounding_box::generate_vertices, "generate vertices of this bounding box for rendering")
					.def("merge", &w_bounding_box::merge, "merge with another bounding box")
					.def("intersects", &w_bounding_box::intersects, "check weather this bounding box intersects with another bounding box")
					.def("contains_point", &w_bounding_box::py_contains_point, "check weather this bounding box contains a point")
					//.def("get_corners", &w_bounding_box::py_get_corners, "get corners points")
					.def("create_from_bounding_sphere", &w_bounding_box::create_from_bounding_sphere, "create bounding box from bounding sphere")


					/*

					.def("contains_bounding_box", &w_bounding_box::py_contains_bounding_box, "check weather bounding box contains a bounding box")
					.def("contains_bounding_sphere", &w_bounding_box::py_contains_bounding_sphere, "check weather bounding box contains a bounding sphere")

					WSYS_EXP void (_Inout_ std::array<glm::vec3, 8>& pCorners);
					*/
					;
			}
		}
#endif
	}
}

#endif
