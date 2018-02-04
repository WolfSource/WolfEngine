/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website          : http://WolfSource.io
    Name             : py_timer.h
    Description      : The python exporter for w_timer class
    Comment          :
 */

#ifdef __PYTHON__

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __PY_TIMER_H__
#define __PY_TIMER_H__

namespace pywolf
{
    static void py_timer_export()
    {
        using namespace boost::python;
        using namespace wolf::system;
        
        
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

#endif//__W_TIMER_PY_H__

#endif//__PYTHON__
