/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website          : http://WolfSource.io
    Name             : w_timer_py.h
    Description      : The python exporter for w_timer class
    Comment          :
 */

namespace pywolf
{
    static void w_timer_py_export()
    {
        using namespace wolf::system;
        using namespace boost::python;
        
        //export w_timer class
        class_<w_timer>("w_timer", init<>())
            .def("start", &w_timer::start, "Start timer")
            .def("stop", &w_timer::stop, "Stop timer")
            .def("reset", &w_timer::reset, "Reset timer")
            .def("get_seconds", &w_timer::get_seconds, "Get total time in seconds")
            .def("get_milliseconds", &w_timer::get_milliseconds, "Get total time in milliseconds")
            ;
    }
}

