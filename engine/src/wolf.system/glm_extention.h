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
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/constants.hpp>
#include <string>
#include "w_convert.h"

#ifndef RGBA_TO_HEX_COLOR
#define RGBA_TO_HEX_COLOR(r,g,b,a) ((unsigned long)((((a)&0xff)<<24) | (((r)&0xff)<<16) | (((g)&0xff)<<8) | ((b)&0xff)))
#endif

namespace std
{
    //create glm::vec2 from string in the format of "0,0" (x,y)
    inline glm::vec2 atov2(const char* pValue)
    {
        //create glm::vec2 from string in the format of "0,0"
        std::stringstream _sstrream(pValue);
        
        float _x = 0, _y = 0;
        int i = 0;
        char _c;
        std::string _number;
        while (_sstrream >> _c)
        {
            _number += _c;
            if (_sstrream.peek() == ',')
            {
                _sstrream.ignore();
                
                if (i == 0)
                {
                    _x = (float) std::atof(_number.c_str());
                }
                
                i++;
                _number = "";
            }
        }
        
        if (!_number.empty())
        {
            _y = (float) std::atof(_number.c_str());
        }
        
        _sstrream.clear();
        
        return glm::vec2(_x, _y);
    }
    
    //create glm::vec3 from string in the format of "0,0,0" (w,x,y)
    inline glm::vec3 atov3(const char* pValue)
    {
        //create glm::vec3 from string in the format of "0,0,0"
        std::stringstream _sstrream(pValue);
        
        float _x = 0, _y = 0, _z = 0;
        int i = 0;
        char _c;
        std::string _number;
        while (_sstrream >> _c)
        {
            _number += _c;
            if (_sstrream.peek() == ',')
            {
                _sstrream.ignore();
                
                if (i == 0)
                {
                    _x = (float) std::atof(_number.c_str());
                }
                else if (i == 1)
                {
                    _y = (float) std::atof(_number.c_str());
                }
                
                i++;
                _number = "";
            }
        }
        
        if (!_number.empty())
        {
            _z = (float) std::atof(_number.c_str());
        }
        
        _sstrream.clear();
        
        return glm::vec3(_x, _y, _z);
    }
    
    //create glm::vec4 from string in the format of "0,0,0,0" (w,x,y,z)
    inline glm::vec4 atov4(const char* pValue)
    {
        std::stringstream _sstrream(pValue);
        
        float _x = 0, _y = 0, _z = 0, _w = 0;
        int i = 0;
        char _c;
        std::string _number;
        while (_sstrream >> _c)
        {
            _number += _c;
            if (_sstrream.peek() == ',')
            {
                _sstrream.ignore();
                
                if (i == 0)
                {
                    _w = (float) std::atof(_number.c_str());
                }
                else if (i == 1)
                {
                    _x = (float) std::atof(_number.c_str());
                }
                else if (i == 2)
                {
                    _y = (float) std::atof(_number.c_str());
                }
                
                i++;
                _number = "";
            }
        }
        
        if (!_number.empty())
        {
            _z = (float) std::atof(_number.c_str());
        }
        
        _sstrream.clear();
        
        return glm::vec4(_w, _x, _y, _z);
    }
}

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
        if (pValue.empty()) return vec3(0);

		std::vector<std::string> pResult;
		wolf::system::convert::split_string(pValue, " ", pResult);

		return vec3(
			std::atof(pResult[0].c_str()),
			std::atof(pResult[1].c_str()),
			std::atof(pResult[2].c_str()));
	}

	inline vec4 to_vec4(std::string pValue)
	{
        if (pValue.empty()) return vec4(0);

		std::vector<std::string> pResult;
		wolf::system::convert::split_string(pValue, " ", pResult);

		return vec4(
			std::atof(pResult[0].c_str()),
			std::atof(pResult[1].c_str()),
			std::atof(pResult[2].c_str()),
			std::atof(pResult[3].c_str()));
	}
	
    inline mat4 to_mat4(std::string pValue)
    {
        if (pValue.empty()) return mat4(0);

        std::vector<std::string> pResult;
        wolf::system::convert::split_string(pValue, " ", pResult);

        return mat4(
            std::atof(pResult[0].c_str()), std::atof(pResult[1].c_str()), std::atof(pResult[2].c_str()), std::atof(pResult[3].c_str()),
            std::atof(pResult[4].c_str()), std::atof(pResult[5].c_str()), std::atof(pResult[6].c_str()), std::atof(pResult[7].c_str()),
            std::atof(pResult[8].c_str()), std::atof(pResult[9].c_str()), std::atof(pResult[10].c_str()), std::atof(pResult[11].c_str()),
            std::atof(pResult[12].c_str()), std::atof(pResult[13].c_str()), std::atof(pResult[14].c_str()), std::atof(pResult[15].c_str()));
    }

    inline std::vector<glm::mat4x4> to_matrix_array(float pFloats[], int pArraySize = 16, int pRowSize = 4)
	{
		std::vector<glm::mat4x4> _mats;

        auto _size = static_cast<size_t>(W_ARRAY_SIZE(pFloats));
		for (size_t j = 0; j < _size; j += pArraySize)
		{
			glm::mat4x4 _mat;
			for (int i = 0; i < pArraySize; i += pRowSize)
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
