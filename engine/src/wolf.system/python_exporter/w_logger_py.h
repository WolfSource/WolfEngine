/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_logger_py.h
	Description		 : The python exporter for w_logger class
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_LOGGER_PY_H__
#define __W_LOGGER_PY_H__

#include <boost/python.hpp>

namespace pywolf
{
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
    
    static void w_logger_py_export()
    {
        using namespace wolf::system;
        using namespace boost::python;
        
        def("get_buffer", &py_get_buffer);
        def("print_buffer", &py_print_buffer);
        def("clear_buffer", &py_clear_buffer);
        def("flush", &py_flush);
        def("write", &py_write);
        def("user", &py_user);
        def("warning", &py_warning);
        def("error", &py_error);
        def("get_is_open", &py_get_is_open);
    }
}

#endif
