/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_boost_python_helper.h
	Description		 : The main header of wolf.engine
	Comment          :
*/

#ifdef __PYTHON__

#ifndef __W_BOOST_PYTHON_HELPER_H__
#define __W_BOOST_PYTHON_HELPER_H__

#include <boost/python.hpp>
#include <boost/make_shared.hpp>
#include <vector>

template<class T> std::shared_ptr<T> boost_shared_ptr_to_std_shared_ptr(boost::shared_ptr<T>& pPointer) 
{
	return std::shared_ptr<T>(pPointer.get(), [pPointer](...) mutable { pPointer.reset(); });
}

template<class T> boost::shared_ptr<T> std_shared_ptr_to_boost_shared_ptr(std::shared_ptr<T>& pPointer)
{
	return boost::shared_ptr<T>(pPointer.get(), [pPointer](...) mutable { pPointer.reset(); });
}

template<typename T> bool boost_list_to_std_vector(boost::python::list& pList, std::vector<T>& pVector)
{
	bool _hr = true;
	for (size_t i = 0; i < len(pList); ++i)
	{
		boost::python::extract<T> _data(pList[i]);
		if (_data.check())
		{
			pVector.push_back(_data());
		}
		else
		{
			_hr = false;
			break;
		}
	}

	return _hr;
}

template<typename T>
inline boost::python::list boost_wrap_array(T* array_, size_t size_)
{
	boost::python::list _list;
	for (int i = 0; i < size_; ++i)
		_list.append(array_[i]);
	return _list;
}

template<typename T1, typename T2>
inline void boost_extract_tuple(T1& p1, boost::python::tuple& pTuple)
{
	boost::python::extract<T1> _1(pTuple[0]);
	p1 = _1;
}

template<typename T1, typename T2>
inline void boost_extract_tuple(T1& p1, T2& p2, boost::python::tuple& pTuple)
{
	boost::python::extract<T1> _1(pTuple[0]);
	boost::python::extract<T2> _2(pTuple[1]);
	p1 = _1; p2 = _2;
}

template<typename T1, typename T2, typename T3>
inline void boost_extract_tuple(T1& p1, T2& p2, T3& p3, boost::python::tuple& pTuple)
{
    boost::python::extract<T1> _1(pTuple[0]);
    boost::python::extract<T2> _2(pTuple[1]);
    boost::python::extract<T3> _3(pTuple[2]);
    p1 = _1; p2 = _2; p3 = _3;
}

template<typename T1, typename T2, typename T3>
inline void boost_extract_tuple(T1& p1, T2& p2, T3& p3, T3& p4, boost::python::tuple& pTuple)
{
	boost::python::extract<T1> _1(pTuple[0]);
	boost::python::extract<T2> _2(pTuple[1]);
	boost::python::extract<T3> _3(pTuple[2]);
	boost::python::extract<T3> _4(pTuple[3]);
	p1 = _1; p2 = _2; p3 = _3; p4 = _4;
}

template<typename T1, typename T2, typename T3>
inline void boost_extract_tuple(
	T1& p1, T2& p2, T3& p3, T3& p4,
	T1& p5, T2& p6, T3& p7, T3& p8,
	T1& p9, T2& p10, T3& p11, T3& p12,
	T1& p13, T2& p14, T3& p15, T3& p16,
	boost::python::tuple& pTuple)
{
	boost::python::extract<T1> _1(pTuple[0]);
	boost::python::extract<T2> _2(pTuple[1]);
	boost::python::extract<T3> _3(pTuple[2]);
	boost::python::extract<T3> _4(pTuple[3]);
	p1 = _1; p2 = _2; p3 = _3; p4 = _4;

	boost::python::extract<T1> _5(pTuple[4]);
	boost::python::extract<T2> _6(pTuple[5]);
	boost::python::extract<T3> _7(pTuple[6]);
	boost::python::extract<T3> _8(pTuple[7]);
	p5 = _5; p6 = _6; p7 = _7; p8 = _8;

	boost::python::extract<T1> _9(pTuple[8]);
	boost::python::extract<T2> _10(pTuple[9]);
	boost::python::extract<T3> _11(pTuple[10]);
	boost::python::extract<T3> _12(pTuple[11]);
	p9 = _9; p10 = _10; p11 = _11; p12 = _12;

	boost::python::extract<T1> _13(pTuple[12]);
	boost::python::extract<T2> _14(pTuple[13]);
	boost::python::extract<T3> _15(pTuple[14]);
	boost::python::extract<T3> _16(pTuple[15]);
	p13 = _13; p14 = _14; p15 = _15; p16 = _16;
}

#endif //__W_BOOST_PYTHON_HELPER_H__

#endif //__PYTHON__
