/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : glm_extension.h
	Description		 : The manager class for IO functions
	Comment          :
*/

#pragma once

#include "w_system_pch.h"
#include "glm/glm.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/compatibility.hpp"
#include <string>
#include "w_convert.h"
#include "w_color.h"
#include "python_exporter/w_boost_python_helper.h"

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

	inline int round_up(_In_ int pNumToRound, _In_ int pMultiple)
	{
		if (pMultiple == 0) return pNumToRound;

		int _reminder = pNumToRound % pMultiple;
		if (_reminder == 0) return pNumToRound;

		return pNumToRound + pMultiple - _reminder;
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

#ifdef __PYTHON__

	class w_vec2
	{
	public:
		w_vec2()
		{
			this->_v.x = 0; this->_v.y = 0;
		}

		w_vec2(_In_ float pX, _In_ float pY)
		{
			this->_v.x = pX; this->_v.y = pY;
		}

		float get_x() { return this->_v.x; }
		void  set_x(_In_ const float& pValue) { this->_v.x = pValue; }

		float get_y() { return this->_v.y; }
		void  set_y(_In_ const float& pValue) { this->_v.y = pValue; }

		boost::python::list						to_list()
		{
			boost::python::list _list;

			_list.append(_v.x);
			_list.append(_v.y);

			return _list;
		}

		glm::vec2 data() const { return this->_v; }

	private:
		glm::vec2 _v;
	};

	class w_vec3
	{
	public:
		w_vec3()
		{
			this->_v.x = 0; this->_v.y = 0; this->_v.z = 0;
		}

		w_vec3(_In_ float pX, _In_ float pY, _In_ float pZ)
		{
			this->_v.x = pX; this->_v.y = pY; this->_v.z = pZ;
		}

		float get_x() { return this->_v.x; }
		void  set_x(_In_ const float& pValue) { this->_v.x = pValue; }

		float get_y() { return this->_v.y; }
		void  set_y(_In_ const float& pValue) { this->_v.y = pValue; }

		float get_z() { return this->_v.z; }
		void  set_z(_In_ const float& pValue) { this->_v.z = pValue; }

		boost::python::list						to_list()
		{
			boost::python::list _list;

			_list.append(_v.x);
			_list.append(_v.y);
			_list.append(_v.z);

			return _list;
		}

		glm::vec3 data() const { return this->_v; }

	private:
		glm::vec3 _v;
	};

	class w_vec4
	{
	public:
		w_vec4()
		{
			this->_v.x = 0; this->_v.y = 0; this->_v.z = 0; this->_v.w = 0;
		}

		w_vec4(_In_ float pX, _In_ float pY, _In_ float pZ, _In_ float pW)
		{
			this->_v.x = pX; this->_v.y = pY; this->_v.z = pZ; this->_v.w = pW;
		}

		float get_x() { return this->_v.x; }
		void  set_x(_In_ const float& pValue) { this->_v.x = pValue; }

		float get_y() { return this->_v.y; }
		void  set_y(_In_ const float& pValue) { this->_v.y = pValue; }

		float get_z() { return this->_v.z; }
		void  set_z(_In_ const float& pValue) { this->_v.z = pValue; }

		float get_w() { return this->_v.w; }
		void  set_w(_In_ const float& pValue) { this->_v.w = pValue; }

		boost::python::list						to_list()
		{
			boost::python::list _list;

			_list.append(_v.x);
			_list.append(_v.y);
			_list.append(_v.z);
			_list.append(_v.w);

			return _list;
		}

		glm::vec4 data() const { return this->_v; }

	private:
		glm::vec4 _v;
	};

	struct w_mat4x4
	{
		float get_00() { return this->_m[0][0]; }
		void  set_00(_In_ const float& pValue) { this->_m[0][0] = pValue; }

		float get_01() { return this->_m[0][1]; }
		void  set_01(_In_ const float& pValue) { this->_m[0][1] = pValue; }

		float get_02() { return this->_m[0][2]; }
		void  set_02(_In_ const float& pValue) { this->_m[0][2] = pValue; }

		float get_03() { return this->_m[0][3]; }
		void  set_03(_In_ const float& pValue) { this->_m[0][3] = pValue; }

		float get_10() { return this->_m[1][0]; }
		void  set_10(_In_ const float& pValue) { this->_m[1][0] = pValue; }

		float get_11() { return this->_m[1][1]; }
		void  set_11(_In_ const float& pValue) { this->_m[1][1] = pValue; }

		float get_12() { return this->_m[1][2]; }
		void  set_12(_In_ const float& pValue) { this->_m[1][2] = pValue; }

		float get_13() { return this->_m[1][3]; }
		void  set_13(_In_ const float& pValue) { this->_m[1][3] = pValue; }

		float get_20() { return this->_m[2][0]; }
		void  set_20(_In_ const float& pValue) { this->_m[2][0] = pValue; }

		float get_21() { return this->_m[2][1]; }
		void  set_21(_In_ const float& pValue) { this->_m[2][1] = pValue; }

		float get_22() { return this->_m[2][2]; }
		void  set_22(_In_ const float& pValue) { this->_m[2][2] = pValue; }

		float get_23() { return this->_m[2][3]; }
		void  set_23(_In_ const float& pValue) { this->_m[2][3] = pValue; }

		float get_30() { return this->_m[3][0]; }
		void  set_30(_In_ const float& pValue) { this->_m[3][0] = pValue; }

		float get_31() { return this->_m[3][1]; }
		void  set_31(_In_ const float& pValue) { this->_m[3][1] = pValue; }

		float get_32() { return this->_m[3][2]; }
		void  set_32(_In_ const float& pValue) { this->_m[3][2] = pValue; }

		float get_33() { return this->_m[3][3]; }
		void  set_33(_In_ const float& pValue) { this->_m[3][3] = pValue; }

		boost::python::list						to_list()
		{
			boost::python::list _list;

			_list.append(_m[0][0]);
			_list.append(_m[0][1]);
			_list.append(_m[0][2]);
			_list.append(_m[0][3]);

			_list.append(_m[1][0]);
			_list.append(_m[1][1]);
			_list.append(_m[1][2]);
			_list.append(_m[1][3]);

			_list.append(_m[2][0]);
			_list.append(_m[2][1]);
			_list.append(_m[2][2]);
			_list.append(_m[2][3]);

			_list.append(_m[3][0]);
			_list.append(_m[3][1]);
			_list.append(_m[3][2]);
			_list.append(_m[3][3]);

			return _list;
		}

		glm::mat4x4 data() const { return this->_m; }

	private:
		glm::mat4x4 _m = glm::mat4(1);
	};

	struct w_mat3x3
	{
		float get_00() { return this->_m[0][0]; }
		void  set_00(_In_ const float& pValue) { this->_m[0][0] = pValue; }

		float get_01() { return this->_m[0][1]; }
		void  set_01(_In_ const float& pValue) { this->_m[0][1] = pValue; }

		float get_02() { return this->_m[0][2]; }
		void  set_02(_In_ const float& pValue) { this->_m[0][2] = pValue; }

		float get_10() { return this->_m[1][0]; }
		void  set_10(_In_ const float& pValue) { this->_m[1][0] = pValue; }

		float get_11() { return this->_m[1][1]; }
		void  set_11(_In_ const float& pValue) { this->_m[1][1] = pValue; }

		float get_12() { return this->_m[1][2]; }
		void  set_12(_In_ const float& pValue) { this->_m[1][2] = pValue; }

		float get_20() { return this->_m[2][0]; }
		void  set_20(_In_ const float& pValue) { this->_m[2][0] = pValue; }

		float get_21() { return this->_m[2][1]; }
		void  set_21(_In_ const float& pValue) { this->_m[2][1] = pValue; }

		float get_22() { return this->_m[2][2]; }
		void  set_22(_In_ const float& pValue) { this->_m[2][2] = pValue; }

		boost::python::list						to_list()
		{
			boost::python::list _list;

			_list.append(_m[0][0]);
			_list.append(_m[0][1]);
			_list.append(_m[0][2]);

			_list.append(_m[1][0]);
			_list.append(_m[1][1]);
			_list.append(_m[1][2]);

			_list.append(_m[2][0]);
			_list.append(_m[2][1]);
			_list.append(_m[2][2]);

			return _list;
		}

		glm::mat3x3 data() const { return this->_m; }

	private:
		glm::mat3x3 _m = glm::mat3x3(1);
	};

	static w_mat4x4 py_rotate(_In_ w_vec3 pValue)
	{
		auto _rotate = glm::rotate(glm::vec3(pValue.get_x(), pValue.get_y(), pValue.get_z()));

		w_mat4x4 _m;
		_m.set_00(_rotate[0][0]);
		_m.set_01(_rotate[0][1]);
		_m.set_02(_rotate[0][2]);
		_m.set_03(_rotate[0][3]);

		_m.set_10(_rotate[1][0]);
		_m.set_11(_rotate[1][1]);
		_m.set_12(_rotate[1][2]);
		_m.set_13(_rotate[1][3]);

		_m.set_20(_rotate[2][0]);
		_m.set_21(_rotate[2][1]);
		_m.set_22(_rotate[2][2]);
		_m.set_23(_rotate[2][3]);

		_m.set_30(_rotate[3][0]);
		_m.set_31(_rotate[3][1]);
		_m.set_32(_rotate[3][2]);
		_m.set_33(_rotate[3][3]);

		return _m;
	}

	static w_mat4x4 py_scale(_In_ w_vec3 pValue)
	{
		auto _scale = glm::scale(glm::vec3(pValue.get_x(), pValue.get_y(), pValue.get_z()));

		w_mat4x4 _m;
		_m.set_00(_scale[0][0]);
		_m.set_01(_scale[0][1]);
		_m.set_02(_scale[0][2]);
		_m.set_03(_scale[0][3]);

		_m.set_10(_scale[1][0]);
		_m.set_11(_scale[1][1]);
		_m.set_12(_scale[1][2]);
		_m.set_13(_scale[1][3]);

		_m.set_20(_scale[2][0]);
		_m.set_21(_scale[2][1]);
		_m.set_22(_scale[2][2]);
		_m.set_23(_scale[2][3]);

		_m.set_30(_scale[3][0]);
		_m.set_31(_scale[3][1]);
		_m.set_32(_scale[3][2]);
		_m.set_33(_scale[3][3]);

		return _m;
	}

	static w_mat4x4 py_lookAtRH(_In_ w_vec3 pEye, _In_ w_vec3 pLookAt, _In_ w_vec3 pUp)
	{
		auto _eye = glm::vec3(pEye.get_x(), pEye.get_y(), pEye.get_z());
		auto _look_at = glm::vec3(pLookAt.get_x(), pLookAt.get_y(), pLookAt.get_z());
		auto _up = glm::vec3(pUp.get_x(), pUp.get_y(), pUp.get_z());

		auto _mat = glm::lookAtRH(_eye, _look_at, _up);

		w_mat4x4 _m;
		_m.set_00(_mat[0][0]);
		_m.set_01(_mat[0][1]);
		_m.set_02(_mat[0][2]);
		_m.set_03(_mat[0][3]);

		_m.set_10(_mat[1][0]);
		_m.set_11(_mat[1][1]);
		_m.set_12(_mat[1][2]);
		_m.set_13(_mat[1][3]);

		_m.set_20(_mat[2][0]);
		_m.set_21(_mat[2][1]);
		_m.set_22(_mat[2][2]);
		_m.set_23(_mat[2][3]);

		_m.set_30(_mat[3][0]);
		_m.set_31(_mat[3][1]);
		_m.set_32(_mat[3][2]);
		_m.set_33(_mat[3][3]);

		return _m;
	}

	static w_mat4x4 py_lookAtLH(_In_ w_vec3 pEye, _In_ w_vec3 pLookAt, _In_ w_vec3 pUp)
	{
		auto _eye = glm::vec3(pEye.get_x(), pEye.get_y(), pEye.get_z());
		auto _look_at = glm::vec3(pLookAt.get_x(), pLookAt.get_y(), pLookAt.get_z());
		auto _up = glm::vec3(pUp.get_x(), pUp.get_y(), pUp.get_z());

		auto _mat = glm::lookAtLH(_eye, _look_at, _up);

		w_mat4x4 _m;
		_m.set_00(_mat[0][0]);
		_m.set_01(_mat[0][1]);
		_m.set_02(_mat[0][2]);
		_m.set_03(_mat[0][3]);

		_m.set_10(_mat[1][0]);
		_m.set_11(_mat[1][1]);
		_m.set_12(_mat[1][2]);
		_m.set_13(_mat[1][3]);

		_m.set_20(_mat[2][0]);
		_m.set_21(_mat[2][1]);
		_m.set_22(_mat[2][2]);
		_m.set_23(_mat[2][3]);

		_m.set_30(_mat[3][0]);
		_m.set_31(_mat[3][1]);
		_m.set_32(_mat[3][2]);
		_m.set_33(_mat[3][3]);

		return _m;
	}

	static w_mat4x4 py_perspectiveRH(_In_ float pFOV, _In_ float pAspectRatio, _In_ float pzNear, _In_ float pzFar)
	{
		auto _mat = glm::perspectiveRH(pFOV, pAspectRatio, pzNear, pzFar);

		w_mat4x4 _m;
		_m.set_00(_mat[0][0]);
		_m.set_01(_mat[0][1]);
		_m.set_02(_mat[0][2]);
		_m.set_03(_mat[0][3]);

		_m.set_10(_mat[1][0]);
		_m.set_11(_mat[1][1]);
		_m.set_12(_mat[1][2]);
		_m.set_13(_mat[1][3]);

		_m.set_20(_mat[2][0]);
		_m.set_21(_mat[2][1]);
		_m.set_22(_mat[2][2]);
		_m.set_23(_mat[2][3]);

		_m.set_30(_mat[3][0]);
		_m.set_31(_mat[3][1]);
		_m.set_32(_mat[3][2]);
		_m.set_33(_mat[3][3]);

		return _m;
	}

	static w_mat4x4 py_perspectiveLH(_In_ float pFOV, _In_ float pAspectRatio, _In_ float pzNear, _In_ float pzFar)
	{
		auto _mat = glm::perspectiveLH(pFOV, pAspectRatio, pzNear, pzFar);

		w_mat4x4 _m;
		_m.set_00(_mat[0][0]);
		_m.set_01(_mat[0][1]);
		_m.set_02(_mat[0][2]);
		_m.set_03(_mat[0][3]);

		_m.set_10(_mat[1][0]);
		_m.set_11(_mat[1][1]);
		_m.set_12(_mat[1][2]);
		_m.set_13(_mat[1][3]);

		_m.set_20(_mat[2][0]);
		_m.set_21(_mat[2][1]);
		_m.set_22(_mat[2][2]);
		_m.set_23(_mat[2][3]);

		_m.set_30(_mat[3][0]);
		_m.set_31(_mat[3][1]);
		_m.set_32(_mat[3][2]);
		_m.set_33(_mat[3][3]);

		return _m;
	}

	static w_vec2 py_multiply_vec2(_In_ w_vec2 pV1, _In_ w_vec2 pV2)
	{
		glm::vec2 _v1 = glm::vec2(pV1.get_x(), pV1.get_y());
		glm::vec2 _v2 = glm::vec2(pV2.get_x(), pV2.get_y());
		glm::vec2 _r = _v1 * _v2;
		w_vec2 _m;
		_m.set_x(_r.x);
		_m.set_y(_r.y);
		return _m;
	}

	static w_vec3 py_multiply_vec3(_In_ w_vec3 pV1, _In_ w_vec3 pV2)
	{
		glm::vec3 _v1 = glm::vec3(pV1.get_x(), pV1.get_y(), pV1.get_z());
		glm::vec3 _v2 = glm::vec3(pV2.get_x(), pV2.get_y(), pV2.get_z());
		glm::vec3 _r = _v1 * _v2;
		w_vec3 _m;
		_m.set_x(_r.x);
		_m.set_y(_r.y);
		_m.set_z(_r.z);
		return _m;
	}

	static w_vec4 py_multiply_vec4(_In_ w_vec4 pV1, _In_ w_vec4 pV2)
	{
		glm::vec4 _v1 = glm::vec4(pV1.get_x(), pV1.get_y(), pV1.get_z(), pV1.get_w());
		glm::vec4 _v2 = glm::vec4(pV2.get_x(), pV2.get_y(), pV2.get_z(), pV2.get_w());
		glm::vec4 _r = _v1 * _v2;

		w_vec4 _m;
		_m.set_x(_r.x);
		_m.set_y(_r.y);
		_m.set_z(_r.z);
		_m.set_w(_r.w);
		return _m;
	}

	static w_mat3x3 py_multiply_mat3x3(_In_ w_mat3x3 pM1, _In_ w_mat3x3 pM2)
	{
		glm::mat3x3 _m1 = glm::mat3x3(pM1.get_00(), pM1.get_01(), pM1.get_02(),
			pM1.get_10(), pM1.get_11(), pM1.get_12(),
			pM1.get_20(), pM1.get_21(), pM1.get_22());

		glm::mat3x3 _m2 = glm::mat3x3(pM2.get_00(), pM2.get_01(), pM2.get_02(),
			pM2.get_10(), pM2.get_11(), pM2.get_12(),
			pM2.get_20(), pM2.get_21(), pM2.get_22());

		glm::mat3x3 _r = _m1 * _m2;

		w_mat3x3 _m;
		_m.set_00(_r[0][0]);
		_m.set_01(_r[0][1]);
		_m.set_02(_r[0][2]);

		_m.set_10(_r[1][0]);
		_m.set_11(_r[1][1]);
		_m.set_12(_r[1][2]);

		_m.set_20(_r[2][0]);
		_m.set_21(_r[2][1]);
		_m.set_22(_r[2][2]);

		return _m;
	}

	static w_mat4x4 py_multiply_mat4x4(_In_ w_mat4x4 pM1, _In_ w_mat4x4 pM2)
	{
		glm::mat4x4 _m1 = glm::mat4x4(pM1.get_00(), pM1.get_01(), pM1.get_02(), pM1.get_03(),
			pM1.get_10(), pM1.get_11(), pM1.get_12(), pM1.get_13(),
			pM1.get_20(), pM1.get_21(), pM1.get_22(), pM1.get_23(),
			pM1.get_30(), pM1.get_31(), pM1.get_32(), pM1.get_33());

		glm::mat4x4 _m2 = glm::mat4x4(pM2.get_00(), pM2.get_01(), pM2.get_02(), pM2.get_03(),
			pM2.get_10(), pM2.get_11(), pM2.get_12(), pM2.get_13(),
			pM2.get_20(), pM2.get_21(), pM2.get_22(), pM2.get_23(),
			pM2.get_30(), pM2.get_31(), pM2.get_32(), pM2.get_33());

		glm::mat4x4 _r = _m1 * _m2;

		w_mat4x4 _m;
		_m.set_00(_r[0][0]);
		_m.set_01(_r[0][1]);
		_m.set_02(_r[0][2]);
		_m.set_03(_r[0][3]);

		_m.set_10(_r[1][0]);
		_m.set_11(_r[1][1]);
		_m.set_12(_r[1][2]);
		_m.set_13(_r[1][3]);

		_m.set_20(_r[2][0]);
		_m.set_21(_r[2][1]);
		_m.set_22(_r[2][2]);
		_m.set_23(_r[2][3]);

		_m.set_30(_r[3][0]);
		_m.set_31(_r[3][1]);
		_m.set_32(_r[3][2]);
		_m.set_33(_r[3][3]);

		return _m;
	}

#endif

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

#include "python_exporter/py_glm.h"

