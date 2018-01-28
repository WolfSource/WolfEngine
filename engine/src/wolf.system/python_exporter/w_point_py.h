/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website          : http://WolfSource.io
    Name             : w_point.h
    Description      : The python exporter for w_point structs
    Comment          :
 */

namespace pywolf
{
    static void w_point_py_export()
    {
        using namespace wolf::system;
        using namespace boost::python;

        class_<w_point>("w_point", init<>())
            .def_readwrite("x", &w_point::x, "x")
            .def_readwrite("y", &w_point::y, "y")
            ;

        class_<w_point_f>("w_point_f", init<>())
            .def_readwrite("x", &w_point_f::x, "x")
            .def_readwrite("y", &w_point_f::y, "y")
            ;

        class_<w_point_t>("w_point_t", init<>())
            .def_readwrite("x", &w_point_t::x, "x")
            .def_readwrite("y", &w_point_t::y, "y")
            ;
    }
}

