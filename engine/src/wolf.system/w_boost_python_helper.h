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

template<typename T1, typename T2, typename T3>
inline void w_boost_extract_tuple(T1& p1, T2& p2, T3& p3, boost::python::tuple pTuple)
{
    boost::python::extract<T1> _1(pTuple[0]);
    boost::python::extract<T2> _2(pTuple[1]);
    boost::python::extract<T3> _3(pTuple[2]);
    p1 = _1; p2 = _2; p3 = _3;
}

template<typename T1, typename T2>
inline void w_boost_extract_tuple(T1& p1, T2& p2, boost::python::tuple pTuple)
{
    boost::python::extract<T1> _1(pTuple[0]);
    boost::python::extract<T2> _2(pTuple[1]);
    p1 = _1; p2 = _2;
}


#endif //__PYTHON__

#endif //__W_BOOST_PYTHON_HELPER_H__


