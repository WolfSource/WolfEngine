/*
    Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website			 : http://WolfSource.io
    Name			 : w_vector4.h
    Description		 : The 4D vector
    Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_VECTOR4_H__
#define __W_VECTOR4_H__

#include "w_system_export.h"
#include <vector>
#include "w_matrix.h"
//#include "w_quaternion.h"
#include "w_vector2.h"
#include "w_vector3.h"

#ifdef __GNUC__
#pragma GCC visibility push(default)
#endif

namespace wolf
{
    namespace system
    {
        //4D vector
        struct w_vector4
        {
        public:
            w_vector4();
            w_vector4(_In_ const float& pX, _In_ const float& pY, _In_ const float& pZ, _In_ const float& pW);
            w_vector4(_In_ const float& pValue);
            w_vector4(_In_ const w_vector2& pValue, _In_ const float& pZ, _In_ const float& pW);
            w_vector4(_In_ const w_vector3& pValue, _In_ const float& pW);

            float length();
            float length_squared();
            void normalize();
            std::string to_string();

            template<typename T>
            T get_hash_code() { std::hash<T>(this->x) + std::hash<T>(this->y) + std::hash<T>(this->z) + std::hash<T>(this->w); }

            static w_vector4 zero();
            static w_vector4 one();
            static w_vector4 unit_x();
            static w_vector4 unit_y();
            static w_vector4 unit_z();
            static w_vector4 unit_w();

            static void distance(_In_ const w_vector4& pValue1, _In_ const w_vector4& pValue2, _Out_ float& pResult);
            static void distance_squared(_In_ const w_vector4& pValue1, _In_ const w_vector4& pValue2, _Out_ float& pResult);
            static void dot(_In_ const w_vector4&  pValue1, _In_  const w_vector4& pValue2, _Out_ float& pResult);
            static void normalize(_In_ const w_vector4& pValue, _Out_ w_vector4& pResult);
            static void min(_In_ const w_vector4& pValue1, _In_ const w_vector4& pValue2, _Out_ w_vector4& pResult);
            static void max(_In_ w_vector4& pValue1, _In_ const w_vector4& pValue2, _Out_ w_vector4& pResult);
            static void clamp(_In_ const w_vector4& pValue1, _In_ const w_vector4& pMin, _In_ const w_vector4& pMax, _Out_ w_vector4& pResult);
            static void lerp(_In_ const w_vector4& pValue1, _In_ const w_vector4& pValue2, _In_ const float& pAmount, _Out_ w_vector4& pResult);
            static void barycentric(_In_ const w_vector4& pValue1, _In_ const w_vector4& pValue2, _In_ const w_vector4& pValue3,
                _In_ const float& pAmount1, _In_ const float& pAmount2, _Out_ w_vector4& pResult);
            static void smooth_step(_In_ const w_vector4& pValue1, _In_ const w_vector4& pValue2, float pAmount, _Out_ w_vector4& pResult);
            static void catmullrom(_In_ const w_vector4& pValue1, _In_ const w_vector4& pValue2,
                _In_ const w_vector4& pValue3, _In_ const w_vector4& pValue4,
                _In_ const float& pAmount, _Out_ w_vector4& pResult);
            static void hermite(_In_ const w_vector4& pValue1, _In_ const w_vector4& pTangent1,
                _In_ const w_vector4& pValue2, _In_ const w_vector4& pTangent2,
                _In_ const float& pAmount, _Out_ w_vector4& pResult);

            static void transform(_In_ const w_vector2& pPosition, _In_ const w_matrix& pMatrix, _Out_ w_vector4 pResult);
            static void transform(_In_ const w_vector3& pPosition, _In_ const w_matrix& pMatrix, _Out_ w_vector4 pResult);
            static void transform(_In_ const w_vector4& pPosition, _In_ const w_matrix& pMatrix, _Out_ w_vector4 pResult);

            //static void transform(_In_ const w_vector2& pValue, _In_ const w_quaternion& pRotation, _Out_ w_vector4& pResult);
            //static void transform(_In_ const w_vector3& pValue, _In_ const w_quaternion& pRotation, _Out_ w_vector4& pResult);
            //static void transform(_In_ const w_vector4& pValue, _In_ const w_quaternion& pRotation, _Out_ w_vector4& pResult);

            static void transform(_In_ const std::vector<w_vector4>& pSourceArray, _In_ const w_matrix& pMatrix,
                _Out_ std::vector<w_vector4>& pDestinationArray);
            static void transform(
                _In_ const std::vector<w_vector4>& pSourceArray, _In_ int pSourceIndex,
                _In_  const w_matrix& pMatrix,
                _Out_ std::vector<w_vector4> pDestinationArray, _In_ int pDestinationIndex, _In_ int pLength);
            /*static void transform(
                _In_ const std::vector<w_vector4>& pSourceArray,
                _In_ const w_quaternion& pRotation,
                _In_ std::vector<w_vector4>& pDestinationArray);

            static void transform(_In_ const std::vector<w_vector4>& pSourceArray, _In_ int pSourceIndex,
                _In_ const w_quaternion& pRotation,
                _In_ std::vector<w_vector4> pDestinationArray, _In_ int pDestinationIndex, _In_ int pLength);
*/
            float x;
            float y;
            float z;
            float w;

        private:
            static w_vector4 _zero;
            static w_vector4 _one;
            static w_vector4 _unit_x;
            static w_vector4 _unit_y;
            static w_vector4 _unit_z;
            static w_vector4 _unit_w;
        };

        inline w_vector4 operator -(_In_ const w_vector4& pValue)
        {
            w_vector4 _vector;
            _vector.x = -pValue.x;
            _vector.y = -pValue.y;
            return _vector;
        }

        inline bool operator ==(_In_ const w_vector4& pValue1, _In_ const w_vector4& pValue2)
        {
            return (pValue1.x == pValue2.x) && (pValue1.y == pValue2.y) && (pValue1.z == pValue2.z);
        }

        inline bool operator !=(_In_ const w_vector4& pValue1, _In_ const w_vector4 pValue2)
        {
            if ((pValue1.x == pValue2.x) && (pValue1.y == pValue2.y))
            {
                return !(pValue1.z == pValue2.z);
            }
            return true;
        }

        inline w_vector4 operator +(_In_ const w_vector4& pValue1, _In_ const w_vector4& pValue2)
        {
            w_vector4 _vector;
            _vector.x = pValue1.x + pValue2.x;
            _vector.y = pValue1.y + pValue2.y;
            _vector.z = pValue1.z + pValue2.z;
            return _vector;
        }

        inline w_vector4 operator -(_In_ const w_vector4& pValue1, _In_ const w_vector4& pValue2)
        {
            w_vector4 _vector;
            _vector.x = pValue1.x - pValue2.x;
            _vector.y = pValue1.y - pValue2.y;
            _vector.z = pValue1.z - pValue2.z;
            return _vector;
        }

        inline w_vector4 operator *(_In_ const w_vector4& pValue1, _In_ const w_vector4& pValue2)
        {
            w_vector4 _vector;
            _vector.x = pValue1.x * pValue2.x;
            _vector.y = pValue1.y * pValue2.y;
            _vector.z = pValue1.z * pValue2.z;
            return _vector;
        }

        inline w_vector4 operator *(_In_ const w_vector4& pValue, _In_ const float& pScaleFactor)
        {
            w_vector4 _vector;
            _vector.x = pValue.x * pScaleFactor;
            _vector.y = pValue.y * pScaleFactor;
            _vector.z = pValue.z * pScaleFactor;
            return _vector;
        }

        inline w_vector4 operator *(_In_ const float& pScaleFactor, _In_ const w_vector4& pValue)
        {
            w_vector4 _vector;
            _vector.x = pValue.x * pScaleFactor;
            _vector.y = pValue.y * pScaleFactor;
            _vector.z = pValue.z * pScaleFactor;
            return _vector;
        }

        inline w_vector4 operator /(_In_ const w_vector4& pValue1, _In_ const w_vector4& pValue2)
        {
            w_vector4 _vector;
            _vector.x = pValue1.x / pValue2.x;
            _vector.y = pValue1.y / pValue2.y;
            _vector.z = pValue1.z / pValue2.z;
            return _vector;
        }

        inline w_vector4 operator /(_In_ const w_vector4& pValue1, _In_ const float& pDivider)
        {
            w_vector4 _vector;
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
