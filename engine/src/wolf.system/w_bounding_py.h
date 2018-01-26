/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_bounding_py.h
	Description		 : The python exporter for w_bounding class
	Comment          :
*/

#ifndef __W_BOUNDING_PY_H__
#define __W_BOUNDING_PY_H__

#ifdef __PYTHON__

#include "w_bounding.h"
#include "w_boost_python_helper.h"

namespace wolf
{
	namespace system
	{
        struct w_bounding_box_py : w_bounding_box
		{
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
        };
        
        
        struct w_bounding_sphere_py : w_bounding_sphere
        {
            //center
            boost::python::tuple py_get_center() { return w_boost_wrap_array(this->center, 3); }
            void py_set_center(_In_ boost::python::tuple pValue) { w_boost_extract_tuple(this->center[0], this->center[1], this->center[2], pValue); }
            
            //radius
            float py_get_radius() { return this->radius; }
            void py_set_radius(_In_ float pValue) { this->radius = pValue; }
            
            bool intersects_bounding_sphere(_In_ const w_bounding_sphere& pSphere) { return intersects(pSphere); }
        };
    }
}

namespace pywolf
{
    static void w_bounding_py_export()
    {
        using namespace wolf::system;
        using namespace boost::python;
        
        //export w_containment_type enum
        enum_<w_containment_type>("w_containment_type")
        .value("DISJOINT", w_containment_type::DISJOINT)
        .value("INTERSECTS", w_containment_type::INTERSECTS)
        .value("CONTAINS", w_containment_type::CONTAINS)
        .export_values();
        
        //export w_bounding_box class
        class_<w_bounding_box_py>("w_bounding_box")
        .def("get_min", &w_bounding_box_py::py_get_min, "get min point of w_bounding_box")
        .def("set_min", &w_bounding_box_py::py_set_min, "set min point of w_bounding_box")
        .def("get_max", &w_bounding_box_py::py_get_max, "get max point of w_bounding_box")
        .def("set_max", &w_bounding_box_py::py_set_max, "set max point of w_bounding_box")
        .def("get_position", &w_bounding_box_py::py_get_position, "get position of w_bounding_box")
        .def("set_position", &w_bounding_box_py::py_set_position, "set position of w_bounding_box")
        .def("get_rotation", &w_bounding_box_py::py_set_rotation, "get rotation of w_bounding_box")
        .def("set_rotation", &w_bounding_box_py::py_set_rotation, "set rotation of w_bounding_box")
        .def("get_vertices", &w_bounding_box_py::py_get_vertices, "used for rendering to masked occulusion culling buffer")
        .def("generate_vertices", &w_bounding_box_py::generate_vertices, "generate vertices of this bounding box for rendering")
        .def("merge", &w_bounding_box_py::merge, "merge with another bounding box")
        .def("intersects", &w_bounding_box_py::intersects, "check weather this bounding box intersects with another bounding box")
        .def("contains_point", &w_bounding_box_py::py_contains_point, "check weather this bounding box contains a point")
        .def("contains_bounding_box", &w_bounding_box_py::py_contains_bounding_box, "check weather bounding box contains a bounding box")
        .def("get_corners", &w_bounding_box_py::py_get_corners, "get corners points")
        ;
        
        //export w_bounding_sphere class
        class_<w_bounding_sphere_py>("w_bounding_sphere")
        .def("get_center", &w_bounding_sphere_py::py_get_center, "get center point of bounding sphere")
        .def("set_center", &w_bounding_sphere_py::py_set_center, "set center point of bounding sphere")
        .def("py_get_radius", &w_bounding_sphere_py::py_get_radius, "get radius of bounding sphere")
        .def("py_set_radius", &w_bounding_sphere_py::py_set_radius, "set radius of bounding sphere")
        .def("merge", &w_bounding_sphere_py::merge, "merge with another bounding sphere")
        .def("intersects", &w_bounding_sphere_py::intersects_bounding_sphere, "check weather this bounding sphere intersects with another bounding sphere")
        ;
    }
}

#endif//__PYTHON__

#endif//__W_BOUNDING_PY_H__
