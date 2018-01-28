/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website          : http://WolfSource.io
    Name             : w_time_span_py.h
    Description      : The python exporter for w_time_span class
    Comment          :
 */

namespace pywolf
{
    static void w_time_span_py_export()
    {
        using namespace wolf::system;
        using namespace boost::python;
        
        //export w_timer class
        class_<w_time_span>("w_time_span", init<>())
            .def("add", &w_time_span::add, "Add two timespans with each other")
            .def("duration", &w_time_span::duration, "Return duration in timespan")
            .def("negate", &w_time_span::negate, "Returns time in string")
            .def("to_string", &w_time_span::to_wstring, "Returns time in string")
            .def("zero", &w_time_span::py_zero, "Create timespan with zero value")
            .def("now", &w_time_span::py_now, "Create timespan from now")
            .def("min_value", &w_time_span::py_min_value, "Create timespan with minimum value")
            .def("max_value", &w_time_span::py_max_value, "Create timespan with maximum value")
            .def("from_days", &w_time_span::py_from_days, "Create timeSpan from days")
            .def("from_hours", &w_time_span::py_from_hours, "Create timeSpan from hours")

            .def("py_from_milliseconds", &w_time_span::py_from_milliseconds, "Create timeSpan from milliSeconds")
            .def("py_from_minutes", &w_time_span::py_from_minutes, "Create timespan from minutes")
            .def("py_from_seconds", &w_time_span::py_from_seconds, "Create timespan from seconds")
            .def("py_from_ticks", &w_time_span::py_from_ticks, "Create timespan from ticks")
            .def("py_from_string", &w_time_span::py_from_string, "Create timespan from string in the format of \"00:00:00:00:000\"")
            .def("get_has_overflowed", &w_time_span::get_has_overflowed, "Returns the overflow status")
            .def("get_ticks", &w_time_span::get_ticks, "Reset timer")
            .def("get_days", &w_time_span::get_days, "Returns total days")
            .def("get_hours", &w_time_span::get_hours, "Returns total hours")
            .def("get_milliseconds", &w_time_span::get_milliseconds, "Returns total milliseconds")
            .def("get_minutes", &w_time_span::get_minutes, "Returns total minutes")
            .def("get_seconds", &w_time_span::get_seconds, "Returns total seconds")
            .def("get_total_days", &w_time_span::get_total_days, "Returns total days")
            .def("get_total_hours", &w_time_span::get_total_hours, "Returns total hours")
            .def("get_total_milliseconds", &w_time_span::get_total_milliseconds, "Returns total milliseconds")
            .def("get_total_minutes", &w_time_span::get_total_minutes, "Returns total minutes")
            .def("get_total_seconds", &w_time_span::get_total_seconds, "Returns total seconds")
            ;
    }
}

