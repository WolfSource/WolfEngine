/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : glm_extension.h
	Description		 : The manager class for IO functions
	Comment          :
*/

#ifndef __W_GLM_EXTENSION_H__
#define __W_GLM_EXTENSION_H__

#if _MSC_VER > 1000
#pragma once
#endif


#include "w_system_pch.h"
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <string>
#include "w_convert.h"

namespace glm
{
	inline vec4 vec4_from_mat4x4_row(mat4x4 pMat, unsigned int PRowindex)
	{
		return vec4(pMat[PRowindex][0], pMat[PRowindex][1], pMat[PRowindex][2], pMat[PRowindex][3]);
	}

	inline vec3 transform_from_mat4x4(mat4x4 pMat)
	{
		return vec3(pMat[3][0], pMat[3][1], pMat[3][2]);
	}

	inline mat4x4 rotate(float pX, float pY, float pZ)
	{
		return
			rotate(pX, vec3(1.0f, 0.0f, 0.0f)) *
			rotate(pY, vec3(0.0f, 1.0f, 0.0f)) *
			rotate(pZ, vec3(0.0f, 0.0f, 1.0f));
	}

	inline mat4x4 rotate(vec3 pValue)
	{
		return
			rotate(pValue.x, vec3(1.0f, 0.0f, 0.0f)) *
			rotate(pValue.y, vec3(0.0f, 1.0f, 0.0f)) *
			rotate(pValue.z, vec3(0.0f, 0.0f, 1.0f));
	}

	inline mat4x4 make_wpv_mat(vec3 pScale, vec3 pRotate, vec3 pTransform)
	{
		return
			glm::scale(pScale)*
			rotate(pRotate) *
			glm::translate(pTransform);
	}

	inline vec3 to_vec3(std::string pValue)
	{
		std::vector<std::string> pResult;
		wolf::system::convert::split_string(pValue, " ", pResult);

		return vec3(
			std::atof(pResult[0].c_str()),
			std::atof(pResult[1].c_str()),
			std::atof(pResult[2].c_str()));
	}

	inline vec4 to_vec4(std::string pValue)
	{
		std::vector<std::string> pResult;
		wolf::system::convert::split_string(pValue, " ", pResult);

		return vec4(
			std::atof(pResult[0].c_str()),
			std::atof(pResult[1].c_str()),
			std::atof(pResult[2].c_str()),
			std::atof(pResult[3].c_str()));
	}
	
	inline std::vector<glm::mat4x4> to_matrix_array(float pFloats[], int pArraySize = 16, int pRowSize = 4)
	{
		std::vector<glm::mat4x4> _mats;

		for (size_t j = 0; j < W_ARRAY_SIZE(pFloats); j += pArraySize)
		{
			glm::mat4x4 _mat;
			for (size_t i = 0; i < pArraySize; i += pRowSize)
			{
				auto _index = i / 4;
				_mat[_index][0] = pFloats[i + j];
				_mat[_index][1] = pFloats[i + 1 + j];
				_mat[_index][2] = pFloats[i + 2 + j];
				_mat[_index][3] = pFloats[i + 3 + j];
			}
			_mats.push_back(_mat);
		}
		return _mats;
	}

}

#endif
