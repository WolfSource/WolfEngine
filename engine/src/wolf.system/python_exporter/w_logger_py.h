/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_logger_py.h
	Description		 : The python exporter for w_logger class
	Comment          :
*/

#ifdef __PYTHON__

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_LOGGER_PY_H__
#define __W_LOGGER_PY_H__

#include <boost/python.hpp>

namespace pywolf
{
    bool py_initialize(_In_z_ const std::wstring pAppName, _In_z_ const std::wstring pLogPath) { return wolf::logger.initialize(pAppName, pLogPath); }
    boost::python::list py_get_buffer()
    {
        boost::python::list _list;
        auto _buffers = wolf::logger.get_buffer();
        for (auto& _buffer : _buffers)
        {
            _list.append(_buffer);
        }
        return _list;
    }
    void py_clear_buffer() { wolf::logger.clear_buffer(); }
    void py_print_buffer(std::wstring pMsg) { wolf::logger.print_buffer(pMsg);}
    void py_flush() { wolf::logger.flush();}
    void py_write(const std::wstring pMsg, const std::wstring pState) { wolf::logger.write(pMsg, pState); }
    void py_user(const std::wstring pMsg) { wolf::logger.user(pMsg); }
    void py_warning(const std::wstring pMsg) { wolf::logger.warning(pMsg); }
    void py_error(const std::wstring pMsg) { wolf::logger.error(pMsg); }
    bool py_get_is_open() { return wolf::logger.get_is_open(); }
    unsigned long py_release() { return wolf::logger.release(); }

    static void w_logger_py_export()
    {
        using namespace boost::python;
        using namespace wolf::system;
        
        def("logger_initialize", &py_initialize);
        def("logger_get_buffer", &py_get_buffer);
        def("logger_print_buffer", &py_print_buffer);
        def("logger_clear_buffer", &py_clear_buffer);
        def("logger_flush", &py_flush);
        def("logger_write", &py_write);
        def("logger_user", &py_user);
        def("logger_warning", &py_warning);
        def("logger_error", &py_error);
        def("logger_get_is_open", &py_get_is_open);
        def("logger_release", &py_release);
    }
}

#endif//__W_LOGGER_PY_H__

#endif //__PYTHON__