/*
    Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website			 : http://WolfSource.io
    Name			 : py_glm.h
    Description		 : The python exporter for glm 
    Comment          :
*/

#ifdef __PYTHON__

#ifndef __PY_GLM_H__
#define __PY_GLM_H__

namespace pyWolf
{
	static void py_glm_export()
	{
		using namespace glm;
		using namespace boost::python;
		using namespace wolf::system;

        //export w_vec2 struct
        class_<w_vec2>("vec2")
	    .def(init<>())
		.def(init<float, float>())
        .add_property("x", &w_vec2::get_x, &w_vec2::set_x, "X")
        .add_property("y", &w_vec2::get_y, &w_vec2::set_y, "Y")
		.def("to_list", &w_vec2::to_list, "to list")
        ;
        
        //export w_vec3 struct
        class_<w_vec3>("vec3")
		.def(init<>())
		.def(init<float, float, float>())
        .add_property("x", &w_vec3::get_x, &w_vec3::set_x, "X")
        .add_property("y", &w_vec3::get_y, &w_vec3::set_y, "Y")
        .add_property("z", &w_vec3::get_z, &w_vec3::set_z, "Z")
		.def("to_list", &w_vec3::to_list, "to list")
        ;
        
        //export w_vec4 struct
        class_<w_vec4>("vec4", init<>())
		.def(init<>())
		.def(init<float, float, float, float>())
        .add_property("x", &w_vec4::get_x, &w_vec4::set_x, "X")
        .add_property("y", &w_vec4::get_y, &w_vec4::set_y, "Y")
        .add_property("z", &w_vec4::get_z, &w_vec4::set_z, "Z")
        .add_property("w", &w_vec4::get_w, &w_vec4::set_w, "W")
		.def("to_list", &w_vec4::to_list, "to list")
        ;
        
        //export w_mat3x3 struct
        class_<w_mat3x3>("mat3x3", init<>())
        .add_property("m00", &w_mat3x3::get_00, &w_mat3x3::set_00, "matrix element [0][0]")
        .add_property("m01", &w_mat3x3::get_01, &w_mat3x3::set_01, "matrix element [0][1]")
        .add_property("m02", &w_mat3x3::get_02, &w_mat3x3::set_02, "matrix element [0][2]")
        .add_property("m10", &w_mat3x3::get_10, &w_mat3x3::set_10, "matrix element [1][0]")
        .add_property("m11", &w_mat3x3::get_11, &w_mat3x3::set_11, "matrix element [1][1]")
        .add_property("m12", &w_mat3x3::get_12, &w_mat3x3::set_12, "matrix element [1][2]")
        .add_property("m20", &w_mat3x3::get_20, &w_mat3x3::set_20, "matrix element [2][0]")
        .add_property("m21", &w_mat3x3::get_21, &w_mat3x3::set_21, "matrix element [2][1]")
        .add_property("m22", &w_mat3x3::get_22, &w_mat3x3::set_22, "matrix element [2][2]")
		.def("to_list", &w_mat3x3::to_list, "to list")
        ;
        
        //export w_mat4 struct
        class_<w_mat4x4>("mat4x4", init<>())
        .add_property("m00", &w_mat4x4::get_00, &w_mat4x4::set_00, "matrix element [0][0]")
        .add_property("m01", &w_mat4x4::get_01, &w_mat4x4::set_01, "matrix element [0][1]")
        .add_property("m02", &w_mat4x4::get_02, &w_mat4x4::set_02, "matrix element [0][2]")
        .add_property("m03", &w_mat4x4::get_03, &w_mat4x4::set_03, "matrix element [0][3]")
        .add_property("m10", &w_mat4x4::get_10, &w_mat4x4::set_10, "matrix element [1][0]")
        .add_property("m11", &w_mat4x4::get_11, &w_mat4x4::set_11, "matrix element [1][1]")
        .add_property("m12", &w_mat4x4::get_12, &w_mat4x4::set_12, "matrix element [1][2]")
        .add_property("m13", &w_mat4x4::get_13, &w_mat4x4::set_13, "matrix element [1][3]")
        .add_property("m20", &w_mat4x4::get_20, &w_mat4x4::set_20, "matrix element [2][0]")
        .add_property("m21", &w_mat4x4::get_21, &w_mat4x4::set_21, "matrix element [2][1]")
        .add_property("m22", &w_mat4x4::get_22, &w_mat4x4::set_22, "matrix element [2][2]")
        .add_property("m23", &w_mat4x4::get_23, &w_mat4x4::set_23, "matrix element [2][3]")
        .add_property("m30", &w_mat4x4::get_30, &w_mat4x4::set_30, "matrix element [3][0]")
        .add_property("m31", &w_mat4x4::get_31, &w_mat4x4::set_31, "matrix element [3][1]")
        .add_property("m32", &w_mat4x4::get_32, &w_mat4x4::set_32, "matrix element [3][2]")
        .add_property("m33", &w_mat4x4::get_33, &w_mat4x4::set_33, "matrix element [3][3]")
		.def("to_list", &w_mat4x4::to_list, "to list")
        ;        
	}
}

#endif//__PY_GLM_H__

#endif //__PYTHON__
