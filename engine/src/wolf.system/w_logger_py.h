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
        
        def("initialize", &w_logger::py_initialize);
        def("print_buffer", &w_logger::print_buffer);
        def("clear_buffer", &w_logger::clear_buffer);
        def("flush", &w_logger::flush);
        def("get_buffer", &w_logger::py_get_buffer);
        def("write", &w_logger::py_write);
        def("user", &w_logger::py_user);
        def("warning", &w_logger::py_warning);
        def("error", &w_logger::py_error);
        def("release", &w_logger::release);
        def("get_is_open", &w_logger::get_is_open);
    }
}
