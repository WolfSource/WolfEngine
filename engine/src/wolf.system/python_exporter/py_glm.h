/*
    Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website			 : http://WolfSource.io
    Name			 : py_glm.h
    Description		 : The python exporter for glm 
    Comment          :
*/

#ifdef __PYTHON__

#ifndef __PY_GLM_H__
#define __PY_GLM_H__

#include <glm_extention.h>
#include <boost/python.hpp>

namespace pyWolf
{
	struct py_glm
	{
		static boost::python::list mat4x4_identity()
		{
			boost::python::list _l;
			_l.append(1.0f); _l.append(0.0f); _l.append(0.0f); _l.append(0.0f);
			_l.append(0.0f); _l.append(1.0f); _l.append(0.0f); _l.append(0.0f);
			_l.append(0.0f); _l.append(0.0f); _l.append(1.0f); _l.append(0.0f);
			_l.append(0.0f); _l.append(0.0f); _l.append(0.0f); _l.append(1.0f);
			return _l;
		}

		static boost::python::list mat3x3_identity()
		{
			boost::python::list _l;
			_l.append(1.0f); _l.append(0.0f); _l.append(0.0f);
			_l.append(0.0f); _l.append(1.0f); _l.append(0.0f);
			_l.append(0.0f); _l.append(0.0f); _l.append(0.0f);
			_l.append(0.0f); _l.append(0.0f); _l.append(1.0f);
			return _l;
		}

		static boost::python::list scale(float pX, float pY, float pZ)
		{
			auto _scale = glm::scale(glm::vec3(pX, pY, pZ));

			boost::python::list _l;
			_l.append(_scale[0][0]); _l.append(_scale[0][1]); _l.append(_scale[0][2]); _l.append(_scale[0][3]);
			_l.append(_scale[1][0]); _l.append(_scale[1][1]); _l.append(_scale[1][2]); _l.append(_scale[1][3]);
			_l.append(_scale[2][0]); _l.append(_scale[2][1]); _l.append(_scale[2][2]); _l.append(_scale[2][3]);
			_l.append(_scale[3][0]); _l.append(_scale[3][1]); _l.append(_scale[3][2]); _l.append(_scale[3][3]);
			return _l;
		}
	};

	static void py_glm_export()
	{
		using namespace boost::python;
		using namespace wolf::system;

		//export py_glm class
		class_<py_glm>("glm", init<>())
			.def("mat4x4_identity", &py_glm::mat4x4_identity, "get identity mat4x4")
			.staticmethod("mat4x4_identity")
			.def("mat3x3_identity", &py_glm::mat3x3_identity, "get identity mat3x3")
			.staticmethod("mat3x3_identity")
			.def("scale", &py_glm::scale, "apply scale")
			.staticmethod("scale")
			;
	}
}

#endif//__PY_GLM_H__

#endif //__PYTHON__