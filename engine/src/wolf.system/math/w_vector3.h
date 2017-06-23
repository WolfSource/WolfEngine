/*
    Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website			 : http://WolfSource.io
    Name			 : w_vector3.h
    Description		 : The 3D vector
    Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_VECTOR3_H__
#define __W_VECTOR3_H__

#include "w_system_export.h"
#include <vector>
#include "w_matrix.h"
//#include "w_quaternion.h"
#include "w_vector2.h"

#ifdef __GNUC__
#pragma GCC visibility push(default)
#endif

namespace wolf
{
    namespace system
    {
        //3D vector
        struct w_vector3
        {
        public:
            WSYS_EXP w_vector3();
            WSYS_EXP w_vector3(_In_ const float& pX, _In_ const float& pY, _In_ const float& pZ);
            WSYS_EXP w_vector3(_In_ const float& pValue);
            WSYS_EXP w_vector3(_In_ const w_vector2& pValue, _In_ const float& pZ);

            WSYS_EXP float length();
            WSYS_EXP float length_squared();
            WSYS_EXP void normalize();
            WSYS_EXP std::string to_string();

            template<typename T>
            T get_hash_code() { std::hash<T>(this->x) + std::hash<T>(this->y) + std::hash<T>(this->z); }

            WSYS_EXP static w_vector3 zero();
            WSYS_EXP static w_vector3 one();
            WSYS_EXP static w_vector3 unit_x();
            WSYS_EXP static w_vector3 unit_y();
            WSYS_EXP static w_vector3 unit_z();
            WSYS_EXP static w_vector3 up();
            WSYS_EXP static w_vector3 down();
            WSYS_EXP static w_vector3 right();
            WSYS_EXP static w_vector3 left();
            WSYS_EXP static w_vector3 forward();
            WSYS_EXP static w_vector3 backward();

            WSYS_EXP static void distance(_In_ const w_vector3& pValue1, _In_ const w_vector3& pValue2, _Out_ float& pResult);
            WSYS_EXP static void distance_squared(_In_ const w_vector3& pValue1, _In_ const w_vector3& pValue2, _Out_ float& pResult);
            WSYS_EXP static void dot(_In_ const w_vector3&  pValue1, _In_  const w_vector3& pValue2, _Out_ float& pResult);
            WSYS_EXP static void normalize(_In_ const w_vector3& pValue, _Out_ w_vector3& pResult);
            WSYS_EXP static void cross(_In_ const w_vector3& pVector1, _In_ const w_vector3& pVector2, _Out_ w_vector3& result);
            WSYS_EXP static void reflect(_In_ const w_vector3& pVector, _In_ const w_vector3& pNormal, _Out_ w_vector3& pResult);
            WSYS_EXP static void min(_In_ const w_vector3& pValue1, _In_ const w_vector3& pValue2, _Out_ w_vector3& pResult);
            WSYS_EXP static void max(_In_ w_vector3& pValue1, _In_ const w_vector3& pValue2, _Out_ w_vector3& pResult);
            WSYS_EXP static void clamp(_In_ const w_vector3& pValue1, _In_ const w_vector3& pMin, _In_ const w_vector3& pMax, _Out_ w_vector3& pResult);
            WSYS_EXP static void lerp(_In_ const w_vector3& pValue1, _In_ const w_vector3& pValue2, _In_ const float& pAmount, _Out_ w_vector3& pResult);
            WSYS_EXP static void barycentric(_In_ const w_vector3& pValue1, _In_ const w_vector3& pValue2, _In_ const w_vector3& pValue3,
                _In_ const float& pAmount1, _In_ const float& pAmount2, _Out_ w_vector3& pResult);
            WSYS_EXP static void smooth_step(_In_ const w_vector3& pValue1, _In_ const w_vector3& pValue2, float pAmount, _Out_ w_vector3& pResult);
            WSYS_EXP static void catmullrom(_In_ const w_vector3& pValue1, _In_ const w_vector3& pValue2,
                _In_ const w_vector3& pValue3, _In_ const w_vector3& pValue4,
                _In_ const float& pAmount, _Out_ w_vector3& pResult);
            WSYS_EXP static void hermite(_In_ const w_vector3& pValue1, _In_ const w_vector3& pTangent1,
                _In_ const w_vector3& pValue2, _In_ const w_vector3& pTangent2,
                _In_ const float& pAmount, _Out_ w_vector3& pResult);

            WSYS_EXP static void transform_normal(_In_ const w_vector3& pNormal, _In_ const w_matrix& pMatrix, _Out_ w_vector3& pResult);
            WSYS_EXP static void transform_normal(_In_ const std::vector<w_vector3>& pSourceArray,
                _In_ const w_matrix& pMatrix,
                _Out_ std::vector<w_vector3>& pDestinationArray);
            WSYS_EXP static void transform_normal(_In_ const std::vector<w_vector3>& pSourceArray,
                _In_ int pSourceIndex, _In_ const w_matrix& pMatrix,
                _Out_ std::vector<w_vector3>& pDestinationArray, _In_ int pDestinationIndex, _In_ int length);

            WSYS_EXP static void transform(_In_ const w_vector3& pPosition, _In_ const w_matrix& pMatrix, _Out_ w_vector3 pResult);
            //WSYS_EXP static void transform(_In_ const w_vector3& pValue, _In_ const w_quaternion& pRotation, _Out_ w_vector3& pResult);
            WSYS_EXP static void transform(_In_ const std::vector<w_vector3>& pSourceArray, _In_ const w_matrix& pMatrix,
                _Out_ std::vector<w_vector3>& pDestinationArray);
            WSYS_EXP static void transform(
                _In_ const std::vector<w_vector3>& pSourceArray, _In_ int pSourceIndex,
                _In_  const w_matrix& pMatrix,
                _Out_ std::vector<w_vector3> pDestinationArray, _In_ int pDestinationIndex, _In_ int pLength);
            //WSYS_EXP static void transform(
            //    _In_ const std::vector<w_vector3>& pSourceArray,
            //    _In_ const w_quaternion& pRotation,
            //    _In_ std::vector<w_vector3>& pDestinationArray);

            //WSYS_EXP static void transform(_In_ const std::vector<w_vector3>& pSourceArray, _In_ int pSourceIndex,
            //    _In_ const w_quaternion& pRotation,
            //    _In_ std::vector<w_vector3> pDestinationArray, _In_ int pDestinationIndex, _In_ int pLength);

            float x;
            float y;
            float z;

        private:
            static w_vector3 _zero;
            static w_vector3 _one;
            static w_vector3 _unit_x;
            static w_vector3 _unit_y;
            static w_vector3 _unit_z;
            static w_vector3 _up;
            static w_vector3 _down;
            static w_vector3 _right;
            static w_vector3 _left;
            static w_vector3 _forward;
            static w_vector3 _backward;
        };

        inline w_vector3 operator -(_In_ const w_vector3& pValue)
        {
            w_vector3 _vector;
            _vector.x = -pValue.x;
            _vector.y = -pValue.y;
            return _vector;
        }

        inline bool operator ==(_In_ const w_vector3& pValue1, _In_ const w_vector3& pValue2)
        {
            return (pValue1.x == pValue2.x) && (pValue1.y == pValue2.y) && (pValue1.z == pValue2.z);
        }

        inline bool operator !=(_In_ const w_vector3& pValue1, _In_ const w_vector3 pValue2)
        {
            if ((pValue1.x == pValue2.x) && (pValue1.y == pValue2.y))
            {
                return !(pValue1.z == pValue2.z);
            }
            return true;
        }

        inline w_vector3 operator +(_In_ const w_vector3& pValue1, _In_ const w_vector3& pValue2)
        {
            w_vector3 _vector;
            _vector.x = pValue1.x + pValue2.x;
            _vector.y = pValue1.y + pValue2.y;
            _vector.z = pValue1.z + pValue2.z;
            return _vector;
        }

        inline w_vector3 operator -(_In_ const w_vector3& pValue1, _In_ const w_vector3& pValue2)
        {
            w_vector3 _vector;
            _vector.x = pValue1.x - pValue2.x;
            _vector.y = pValue1.y - pValue2.y;
            _vector.z = pValue1.z - pValue2.z;
            return _vector;
        }

        inline w_vector3 operator *(_In_ const w_vector3& pValue1, _In_ const w_vector3& pValue2)
        {
            w_vector3 _vector;
            _vector.x = pValue1.x * pValue2.x;
            _vector.y = pValue1.y * pValue2.y;
            _vector.z = pValue1.z * pValue2.z;
            return _vector;
        }

        inline w_vector3 operator *(_In_ const w_vector3& pValue, _In_ const float& pScaleFactor)
        {
            w_vector3 _vector;
            _vector.x = pValue.x * pScaleFactor;
            _vector.y = pValue.y * pScaleFactor;
            _vector.z = pValue.z * pScaleFactor;
            return _vector;
        }

        inline w_vector3 operator *(_In_ const float& pScaleFactor, _In_ const w_vector3& pValue)
        {
            w_vector3 _vector;
            _vector.x = pValue.x * pScaleFactor;
            _vector.y = pValue.y * pScaleFactor;
            _vector.z = pValue.z * pScaleFactor;
            return _vector;
        }

        inline w_vector3 operator /(_In_ const w_vector3& pValue1, _In_ const w_vector3& pValue2)
        {
            w_vector3 _vector;
            _vector.x = pValue1.x / pValue2.x;
            _vector.y = pValue1.y / pValue2.y;
            _vector.z = pValue1.z / pValue2.z;
            return _vector;
        }

        inline w_vector3 operator /(_In_ const w_vector3& pValue1, _In_ const float& pDivider)
        {
            w_vector3 _vector;
            float _num = 1.0f / pDivider;
            _vector.x = pValue1.x * _num;
            _vector.y = pValue1.y * _num;
            _vector.z = pValue1.z * _num;
            return _vector;
        }
    }
}

#ifdef __GNUC__
#pragma GCC visibility pop
#endif

#endif
