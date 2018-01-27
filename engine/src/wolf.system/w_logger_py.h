/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_logger_py.h
	Description		 : The python exporter for w_logger class
	Comment          :
*/

namespace pywolf
{
    static void w_logger_py_export()
    {
        using namespace wolf::system;
        using namespace boost::python;
        
  //      //define w_containment_type enum
  //      enum_<w_containment_type>("w_containment_type")
  //      .value("DISJOINT", w_containment_type::DISJOINT)
  //      .value("INTERSECTS", w_containment_type::INTERSECTS)
  //      .value("CONTAINS", w_containment_type::CONTAINS)
  //      .export_values();
  //      
  //      //export w_bounding_box class
  //      class_<w_bounding_box>("w_bounding_box", init<>())
  //      .def("get_min", &w_bounding_box::py_get_min, "get min point of w_bounding_box")
  //      .def("set_min", &w_bounding_box::py_set_min, "set min point of w_bounding_box")
  //      .def("get_max", &w_bounding_box::py_get_max, "get max point of w_bounding_box")
  //      .def("set_max", &w_bounding_box::py_set_max, "set max point of w_bounding_box")
  //      .def("get_position", &w_bounding_box::py_get_position, "get position of w_bounding_box")
  //      .def("set_position", &w_bounding_box::py_set_position, "set position of w_bounding_box")
  //      .def("get_rotation", &w_bounding_box::py_set_rotation, "get rotation of w_bounding_box")
  //      .def("set_rotation", &w_bounding_box::py_set_rotation, "set rotation of w_bounding_box")
  //      .def("get_vertices", &w_bounding_box::py_get_vertices, "used for rendering to masked occulusion culling buffer")
  //      .def("generate_vertices", &w_bounding_box::generate_vertices, "generate vertices of this bounding box for rendering")
  //      .def("merge", &w_bounding_box::merge, "merge with another bounding box")
  //      .def("intersects", &w_bounding_box::intersects, "check weather this bounding box intersects with another bounding box")
  //      .def("contains_point", &w_bounding_box::py_contains_point, "check weather this bounding box contains a point")
  //      .def("contains_bounding_box", &w_bounding_box::py_contains_bounding_box, "check weather bounding box contains a bounding box")
  //      .def("get_corners", &w_bounding_box::py_get_corners, "get corners points")
  //      ;
  //      
  //      //export w_bounding_sphere class
  //      class_<w_bounding_sphere>("w_bounding_sphere", init<>())
  //      .def("get_center", &w_bounding_sphere::py_get_center, "get center point of bounding sphere")
  //      .def("set_center", &w_bounding_sphere::py_set_center, "set center point of bounding sphere")
  //      .def("py_get_radius", &w_bounding_sphere::py_get_radius, "get radius of bounding sphere")
  //      .def("py_set_radius", &w_bounding_sphere::py_set_radius, "set radius of bounding sphere")
  //      .def("merge", &w_bounding_sphere::merge, "merge with another bounding sphere")
  //      .def("intersects", &w_bounding_sphere::intersects_bounding_sphere, "check weather this bounding sphere intersects with another bounding sphere")
  //      ;

		////export w_bounding_frustum class
		//class_<w_bounding_frustum>("w_bounding_frustum", init<>())
		//	.def("get_plans", &w_bounding_frustum::py_get_plans, "get plans of bounding frustum")
		//	.def("update", &w_bounding_frustum::py_update, "update bounding frustum")
		//	.def("intersects_bounding_sphere", &w_bounding_frustum::py_intersects_bounding_sphere, "check weather this bounding frustum intersects with another bounding sphere")
		//	.def("intersects_bounding_box", &w_bounding_frustum::py_intersects_bounding_box, "check weather this bounding frustum intersects with another bounding box")
		//	;
    }
}
