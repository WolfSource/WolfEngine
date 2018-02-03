/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website          : http://WolfSource.io
    Name             : w_time_span_py.h
    Description      : The python exporter for w_time_span class
    Comment          :
 */

#ifdef __PYTHON__

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_TIME_SPAN_PY_H__
#define __W_TIME_SPAN_PY_H__

namespace pywolf
{
    static void w_time_span_py_export()
    {
        using namespace boost::python;
        using namespace wolf::system;
        
        //export w_timer class
        class_<w_time_span>("w_time_span", init<>())
            .def("add", &w_time_span::add, "Add two timespans with each other")
            .def("duration", &w_time_span::duration, "Return duration in timespan")
            .def("negate", &w_time_span::negate, "Returns time in string")
            .def("to_string", &w_time_span::to_wstring, "Returns time in string")
            .def("zero", &w_time_span::zero, "Returns timespan with zero value")
            .staticmethod("zero")
            .def("now", &w_time_span::now, "Return timespan from now")
            .staticmethod("now")
            .def("min_value", &w_time_span::min_value, "Return timespan with minimum value")
            .staticmethod("min_value")
            .def("max_value", &w_time_span::max_value, "Return timespan with maximum value")
            .staticmethod("max_value")
            .def("from_days", &w_time_span::from_days, "Return timeSpan from days")
            .staticmethod("from_days")
            .def("from_hours", &w_time_span::from_hours, "Return timeSpan from hours")
            .staticmethod("from_hours")
            .def("from_milliseconds", &w_time_span::from_milliseconds, "Return timeSpan from milliSeconds")
            .staticmethod("from_milliseconds")
            .def("from_minutes", &w_time_span::from_minutes, "Return timespan from minutes")
            .staticmethod("from_minutes")
            .def("from_seconds", &w_time_span::from_seconds, "Return timespan from seconds")
            .staticmethod("from_seconds")
            .def("from_ticks", &w_time_span::from_ticks, "Return timespan from ticks")
            .staticmethod("from_ticks")
            .def("from_string", &w_time_span::from_string, "Return timespan from string in the format of \"00:00:00:00:000\"")
            .staticmethod("from_string")
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

#endif//__W_TIME_SPAN_PY_H__

#endif//__PYTHON__
