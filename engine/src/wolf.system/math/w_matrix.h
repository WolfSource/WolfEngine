/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_matix.h
	Description		 : The matrix4x4 class
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_MATRIX_H__
#define __W_MATRIX_H__

#include "w_system_export.h"
#include "w_std.h"
#include <string>

#ifdef __GNUC__
#pragma GCC visibility push(default)
#endif

namespace wolf
{
    namespace system
    {
        //forward declarations
        struct w_vector3;

        //The 4x4 matrix
        struct w_matrix
        {
        public:
            WSYS_EXP w_matrix();
            WSYS_EXP w_matrix(
                _In_ const float& pM11, _In_ const float& pM12, _In_ const float& pM13, _In_ const float& pM14, 
                _In_ const float& pM21, _In_ const float& pM22, _In_ const float& pM23, _In_ const float& pM24, 
                _In_ const float& pM31, _In_ const float& pM32, _In_ const float& pM33, _In_ const float& pM34, 
                _In_ const float& pM41, _In_ const float& pM42, _In_ const float& pM43, _In_ const float& pM44);
            WSYS_EXP virtual ~w_matrix();

            WSYS_EXP float   M11() const { return _m[0][0]; }
            WSYS_EXP float   M12() const { return _m[0][1]; }
            WSYS_EXP float   M13() const { return _m[0][2]; }
            WSYS_EXP float   M14() const { return _m[0][3]; }
            WSYS_EXP float   M21() const { return _m[1][0]; }
            WSYS_EXP float   M22() const { return _m[1][1]; }
            WSYS_EXP float   M23() const { return _m[1][2]; }
            WSYS_EXP float   M24() const { return _m[1][3]; }
            WSYS_EXP float   M31() const { return _m[2][0]; }
            WSYS_EXP float   M32() const { return _m[2][1]; }
            WSYS_EXP float   M33() const { return _m[2][2]; }
            WSYS_EXP float   M34() const { return _m[2][3]; }
            WSYS_EXP float   M41() const { return _m[3][0]; }
            WSYS_EXP float   M42() const { return _m[3][1]; }
            WSYS_EXP float   M43() const { return _m[3][2]; }
            WSYS_EXP float   M44() const { return _m[3][3]; }

            WSYS_EXP void   M11(_In_ const float& pValue) { this->_m[0][0] = pValue; }
            WSYS_EXP void   M12(_In_ const float& pValue) { this->_m[0][1] = pValue; }
            WSYS_EXP void   M13(_In_ const float& pValue) { this->_m[0][2] = pValue; }
            WSYS_EXP void   M14(_In_ const float& pValue) { this->_m[0][3] = pValue; }
            WSYS_EXP void   M21(_In_ const float& pValue) { this->_m[1][0] = pValue; }
            WSYS_EXP void   M22(_In_ const float& pValue) { this->_m[1][1] = pValue; }
            WSYS_EXP void   M23(_In_ const float& pValue) { this->_m[1][2] = pValue; }
            WSYS_EXP void   M24(_In_ const float& pValue) { this->_m[1][3] = pValue; }
            WSYS_EXP void   M31(_In_ const float& pValue) { this->_m[2][0] = pValue; }
            WSYS_EXP void   M32(_In_ const float& pValue) { this->_m[2][1] = pValue; }
            WSYS_EXP void   M33(_In_ const float& pValue) { this->_m[2][2] = pValue; }
            WSYS_EXP void   M34(_In_ const float& pValue) { this->_m[2][3] = pValue; }
            WSYS_EXP void   M41(_In_ const float& pValue) { this->_m[3][0] = pValue; }
            WSYS_EXP void   M42(_In_ const float& pValue) { this->_m[3][1] = pValue; }
            WSYS_EXP void   M43(_In_ const float& pValue) { this->_m[3][2] = pValue; }
            WSYS_EXP void   M44(_In_ const float& pValue) { this->_m[3][3] = pValue; }

            WSYS_EXP float*  data() { return &(_m[0][0]); }

            WSYS_EXP w_vector3 up();
            WSYS_EXP void up(_In_ const w_vector3& pValue);
            
            WSYS_EXP w_vector3 down();
            WSYS_EXP void down(_In_ const w_vector3& pValue);

            WSYS_EXP w_vector3 right();
            WSYS_EXP void right(_In_ const w_vector3& pValue);

            WSYS_EXP w_vector3 left();
            WSYS_EXP void left(_In_ const w_vector3& pValue);

            WSYS_EXP w_vector3 forward();
            WSYS_EXP void forward(_In_ const w_vector3& pValue);

            WSYS_EXP w_vector3 backward();
            WSYS_EXP void backward(_In_ const w_vector3& pValue);

            WSYS_EXP w_vector3 translation();
            WSYS_EXP void translation(_In_ const w_vector3& pValue);


#pragma region Static methods
            
            WSYS_EXP static w_matrix identity() { return _identity; }
            WSYS_EXP static void create_billboard(
                _In_ const w_vector3& pObjectPosition,
                _In_ const w_vector3& pCameraPosition, 
                _In_ const w_vector3& pCameraUpVector, 
                _In_ const w_vector3* pCameraForwardVector,
                _Out_ w_matrix& pResult);
            WSYS_EXP static void create_constrained_billboard(
                _In_ const w_vector3& pObjectPosition,
                _In_ const w_vector3& pCameraPosition,
                _In_ const w_vector3& pRotateAxis,
                _In_ const w_vector3* pCameraForwardVector,
                _In_ const w_vector3* pObjectForwardVector,
                _Out_ w_matrix& pResult);
            WSYS_EXP static void create_translation(_In_ const w_vector3& pPosition, _Out_ w_matrix& pResult);
            WSYS_EXP static void create_scale(_In_ const float& pScaleX, _In_ const float& pScaleY, _In_ const float& pScaleZ, _Out_  w_matrix& pResult);
            WSYS_EXP static void create_scale(_In_ const w_vector3& pScale, _Out_ w_matrix& pResult);
            WSYS_EXP static void create_scale(_In_ const float& pScale, _Out_ w_matrix& pResult);
            WSYS_EXP static void create_rotation_x(_In_ const float& pRadians, _Out_ w_matrix& pResult);

#pragma endregion

        private:
            static w_matrix _identity;

            float   _m[4][4];
        };
    }
}

#ifdef __GNUC__
#pragma GCC visibility pop
#endif

#endif