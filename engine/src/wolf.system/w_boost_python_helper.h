/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_boost_python.h
	Description		 : The main header of wolf.engine
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_BOOST_PYTHON_HELPER_H__
#define __W_BOOST_PYTHON_HELPER_H__

#ifdef __PYTHON__

#include <boost/python.hpp>

template<typename T>
inline boost::python::tuple w_boost_wrap_array(T* array_, size_t size_)
{
	boost::python::list _list;
	for (int i = 0; i < size_; ++i)
		_list.append(array_[i]);
	return boost::python::tuple(_list);
}

#endif //__PYTHON__

#endif //__W_BOOST_PYTHON_HELPER_H__


