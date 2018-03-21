/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : py_bounding.h
	Description		 : The python exporter for w_bounding classes ane enums
	Comment          :
*/

#ifdef __PYTHON__

#ifndef __PY_BOUNDING_H__
#define __PY_BOUNDING_H__

namespace pyWolf
{
    static void py_bounding_export()
    {
        using namespace boost::python;
        using namespace wolf::system;
        
        //define w_containment_type enum
        enum_<w_containment_type>("w_containment_type")
            .value("DISJOINT", w_containment_type::DISJOINT)
            .value("INTERSECTS", w_containment_type::INTERSECTS)
            .value("CONTAINS", w_containment_type::CONTAINS)
            .export_values()
            ;
        
        //export w_bounding_box class
        class_<w_bounding_box>("w_bounding_box", init<>())
            .add_property("min", &w_bounding_box::py_get_min, &w_bounding_box::py_set_min,"get or set min point of w_bounding_box")
            .add_property("max", &w_bounding_box::py_get_max, &w_bounding_box::py_set_max, "get or set max point of w_bounding_box")
            .add_property("position", &w_bounding_box::py_get_position, &w_bounding_box::py_set_position, "get or set position of w_bounding_box")
            .add_property("rotation", &w_bounding_box::py_get_rotation, &w_bounding_box::py_set_rotation, "get or set rotation of w_bounding_box")
            .def("get_vertices", &w_bounding_box::py_get_vertices, "used for rendering to masked occulusion culling buffer")
            .def("generate_vertices", &w_bounding_box::generate_vertices, "generate vertices of this bounding box for rendering")
            .def("merge", &w_bounding_box::merge, "merge with another bounding box")
            .def("intersects", &w_bounding_box::intersects, "check weather this bounding box intersects with another bounding box")
            .def("contains_point", &w_bounding_box::py_contains_point, "check weather this bounding box contains a point")
            .def("contains_bounding_box", &w_bounding_box::py_contains_bounding_box, "check weather bounding box contains a bounding box")
            .def("get_corners", &w_bounding_box::py_get_corners, "get corners points")
			.def("create_from_bounding_sphere", &w_bounding_box::create_from_bounding_sphere, "create bounding box from bounding sphere")
			.staticmethod("create_from_bounding_sphere")
            ;
        
        //export w_bounding_sphere class
        class_<w_bounding_sphere>("w_bounding_sphere", init<>())
            .add_property("center", &w_bounding_sphere::py_get_center, &w_bounding_sphere::py_set_center, "get or set center point of bounding sphere")
            .add_property("radius", &w_bounding_sphere::py_get_radius, &w_bounding_sphere::py_set_radius, "get or set radius of bounding sphere")
            .def("merge", &w_bounding_sphere::merge, "merge with another bounding sphere")
            .def("intersects", &w_bounding_sphere::intersects_bounding_sphere, "check weather this bounding sphere intersects with another bounding sphere")
			.def("create_from_bounding_box", &w_bounding_sphere::create_from_bounding_box, "create bounding sphere from bounding box")
			.staticmethod("create_from_bounding_box")
            ;

		//export w_bounding_frustum class
        class_<w_bounding_frustum>("w_bounding_frustum", init<>())
            .def("get_plans", &w_bounding_frustum::py_get_plans, "get plans of bounding frustum")
            .def("update", &w_bounding_frustum::py_update, "update bounding frustum")
            .def("intersects_bounding_sphere", &w_bounding_frustum::py_intersects_bounding_sphere, "check weather this bounding frustum intersects with another bounding sphere")
            .def("intersects_bounding_box", &w_bounding_frustum::py_intersects_bounding_box, "check weather this bounding frustum intersects with another bounding box")
            ;
    }
}

#endif//__W_BOUNDING_PY_H__

#endif//__PYTHON__