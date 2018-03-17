/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : py_std.h
	Description		 : The python exporter for w_std structs
	Comment          :
*/

#ifdef __PYTHON__

#ifndef __PY_STD_H__
#define __PY_STD_H__

#include "python_exporter/w_boost_python_helper.h"
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

namespace pyWolf
{
	typedef std::vector<uint8_t> w_vector_uint8_t;
	typedef std::vector<float> w_vector_float;

	//static std::wstring py_sprintf(_In_z_ std::wstring pStr, _In_ boost::python::list pList)
	//{
	//	const wchar_t* _delimiter = L"<%>";
	//	for (size_t i = 0; i < len(pList); ++i)
	//	{
	//		boost::python::extract<double> _data(pList[i]);
	//		if (_data.check())
	//		{
	//			auto _found = pStr.find(_delimiter);
	//			if (_found == std::wstring::npos) break;
	//			
	//			pStr.replace(_found, 3, std::to_wstring(_data()));
	//		}
	//	}
	//	return pStr;
	//}

	static void py_std_export()
	{
		using namespace boost::python;

		//define W_RESULT enum
		enum_<W_RESULT>("W_RESULT")
			.value("W_PASSED", W_RESULT::W_PASSED)
			.value("W_FAILED", W_RESULT::W_FAILED)
			.value("W_INVALIDARG", W_RESULT::W_INVALIDARG)
			.value("W_OUTOFMEMORY", W_RESULT::W_OUTOFMEMORY)
            .value("W_INVALID_FILE_ATTRIBUTES", W_RESULT::W_INVALID_FILE_ATTRIBUTES)
			.export_values()
			;

		//export vector of uint8_t
		class_<w_vector_uint8_t>("w_vector_uint8_t")
			.def(vector_indexing_suite<w_vector_uint8_t>());

		//export vector of float
		class_<w_vector_float>("w_vector_float")
			.def(vector_indexing_suite<w_vector_float>());

	}
}

#endif//__PY_STD_H__

#endif//__PYTHON__
