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
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>
#include <string>
#include "w_convert.h"
#include "w_color.h"

#ifndef RGBA_TO_HEX_COLOR
#define RGBA_TO_HEX_COLOR(r,g,b,a) ((unsigned long)((((a)&0xff)<<24) | (((r)&0xff)<<16) | (((g)&0xff)<<8) | ((b)&0xff)))
#endif

namespace std
{
    //create glm::vec2 from string in the format of "0,0" (x,y)
    inline glm::vec2 atov2(_In_z_ const char* pValue)
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
    inline glm::vec3 atov3(_In_z_ const char* pValue)
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
	inline vec4 vec4_from_mat4x4_row(_In_ const mat4x4 pMat, _In_ const unsigned int PRowindex)
	{
		return vec4(pMat[PRowindex][0], pMat[PRowindex][1], pMat[PRowindex][2], pMat[PRowindex][3]);
	}

	inline vec3 transform_from_mat4x4(_In_ const mat4x4 pMat)
	{
		return vec3(pMat[3][0], pMat[3][1], pMat[3][2]);
	}

	inline mat4x4 rotate(_In_ const float pX, _In_ const float pY, _In_ const float pZ)
	{
		return
			rotate(pX, vec3(1.0f, 0.0f, 0.0f)) *
			rotate(pY, vec3(0.0f, 1.0f, 0.0f)) *
			rotate(pZ, vec3(0.0f, 0.0f, 1.0f));
	}

	inline mat4x4 rotate(_In_ const vec3 pValue)
	{
		return
			rotate(pValue.x, vec3(1.0f, 0.0f, 0.0f)) *
			rotate(pValue.y, vec3(0.0f, 1.0f, 0.0f)) *
			rotate(pValue.z, vec3(0.0f, 0.0f, 1.0f));
	}

	inline mat4x4 make_wpv_mat(_In_ const vec3 pScale, _In_ const vec3 pRotate, _In_ const vec3 pTransform)
	{
		return
			glm::scale(pScale)*
			rotate(pRotate) *
			glm::translate(pTransform);
	}

	inline vec3 to_vec3(_In_z_ const std::string& pValue)
	{
        if (pValue.empty()) return vec3(0);

		std::vector<std::string> pResult;
		wolf::system::convert::split_string(pValue, " ", pResult);

		return vec3(
			std::atof(pResult[0].c_str()),
			std::atof(pResult[1].c_str()),
			std::atof(pResult[2].c_str()));
	}

	inline vec4 to_vec4(_In_z_ const std::string& pValue)
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

    inline glm::vec4 to_vec4(_In_ const DWORD& pValue)
    {
        //Convert ARGB(DWORD) to RGBA(in the range of 0..1)
        return glm::vec4(
            ((pValue >> 16) & 0xFF) / 255.0f,//R
            ((pValue >> 8) & 0xFF) / 255.0f,//G
            (pValue & 0xFF) / 255.0f,//B
            ((pValue >> 24) & 0xFF) / 255.0f);//A
    }
	
    inline glm::vec4 to_vec4(_In_ const w_color pValue)
    {
        return glm::vec4(pValue.r / 255.0f, pValue.g / 255.0f, pValue.b / 255.0f, pValue.a / 255.0f);
    }

    inline std::array<float, 4> to_float_array(_In_ const vec4 pValue)
    {
        return { pValue.r, pValue.g, pValue.b, pValue.a };
    }
    
    inline std::array<float, 3> to_float_array(_In_ const vec3 pValue)
    {
        return { pValue.r, pValue.g, pValue.b };
    }
    
    inline std::array<float, 2> to_float_array(_In_ const vec2 pValue)
    {
        return { pValue.r, pValue.g };
    }
    
    inline glm::mat4 to_mat4(_In_z_ const std::string& pValue)
    {
        if (pValue.empty()) return glm::mat4(0);

        std::vector<std::string> pResult;
        wolf::system::convert::split_string(pValue, " ", pResult);

        return mat4(
            std::atof(pResult[0].c_str()), std::atof(pResult[1].c_str()), std::atof(pResult[2].c_str()), std::atof(pResult[3].c_str()),
            std::atof(pResult[4].c_str()), std::atof(pResult[5].c_str()), std::atof(pResult[6].c_str()), std::atof(pResult[7].c_str()),
            std::atof(pResult[8].c_str()), std::atof(pResult[9].c_str()), std::atof(pResult[10].c_str()), std::atof(pResult[11].c_str()),
            std::atof(pResult[12].c_str()), std::atof(pResult[13].c_str()), std::atof(pResult[14].c_str()), std::atof(pResult[15].c_str()));
    }
   
    inline std::vector<glm::mat4x4> to_matrix_array(_In_ float pFloats[], _In_ const  int& pLenghtOfFloats,  _In_ const int pArraySize = 16, _In_ const int pRowSize = 4)
   {
         std::vector<glm::mat4x4> _mats;

        for (size_t j = 0; j < pLenghtOfFloats; j += pArraySize)
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
    
    inline glm::vec3 rotation_from_angle_axis(_In_ const float pAxisX, _In_ const float pAxisY, _In_ const float pAxisZ, _In_ const float pAngleDegree)
    {
        glm::quat _quat;
        
        float _half_angle_radian ( 0.5f * glm::radians(pAngleDegree));
        float fSin = sin( _half_angle_radian );
        _quat.w = cos( _half_angle_radian );
    
        _quat.x = fSin * pAxisX;
        _quat.y = fSin * pAxisY;
        _quat.z = fSin * pAxisZ;
    
        auto _pitch = glm::pitch(_quat);
        auto _yaw = glm::yaw(_quat);
        auto _roll = glm::roll(_quat);

        return glm::vec3(_pitch, _yaw, _roll);
    }
}

#if defined(__DX12__) || defined(__DX11__)
    inline XMMATRIX XMMatrixFromGLMMatrix(_In_ glm::tmat4x4<float, _In_ glm::precision::highp> const& pValue)
    {
        return XMMATRIX(
            pValue[0][0], pValue[0][1], pValue[0][2], pValue[0][3],
            pValue[1][0], pValue[1][1], pValue[1][2], pValue[1][3],
            pValue[2][0], pValue[2][1], pValue[2][2], pValue[2][3],
            pValue[3][0], pValue[3][1], pValue[3][2], pValue[3][3]);
    }
#endif //defined(__DX12__) || defined(__DX11__)

#endif
