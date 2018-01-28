/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website          : http://WolfSource.io
    Name             : w_rectangle.h
    Description      : The python exporter for w_rectangle structs
    Comment          :
 */

namespace pywolf
{
    static void w_rectangle_py_export()
    {
        using namespace wolf::system;
        using namespace boost::python;

        class_<w_rectangle>("w_rectangle", init<>())
            .def_readwrite("left", &w_rectangle::left, "left")
            .def_readwrite("top", &w_rectangle::top, "top")
            .def_readwrite("right", &w_rectangle::right, "right")
            .def_readwrite("bottom", &w_rectangle::bottom, "bottom")
            ;
    }
}

