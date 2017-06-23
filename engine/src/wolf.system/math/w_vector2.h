/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_vector2.h
	Description		 : The 2D vector
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_VECTOR2_H__
#define __W_VECTOR2_H__

#include <vector>
#include "w_matrix.h"
//#include "w_quaternion.h"

#ifdef __GNUC__
#pragma GCC visibility push(default)
#endif

namespace wolf
{
    namespace system
    {
        //2D vector
        struct w_vector2
        {
        public:
            WSYS_EXP w_vector2();
            WSYS_EXP w_vector2(_In_ const float& pX, _In_ const float& pY);
            WSYS_EXP w_vector2(_In_ const float& pValue);
            
            WSYS_EXP float length();
            WSYS_EXP float length_squared();
            WSYS_EXP void normalize();
            WSYS_EXP std::string to_string();
            
            template<typename T>
            T get_hash_code() { std::hash<T>(this->x) + std::hash<T>(this->y); }

            WSYS_EXP static w_vector2 zero();
            WSYS_EXP static w_vector2 one();
            WSYS_EXP static w_vector2 unit_x();
            WSYS_EXP static w_vector2 unit_y();
            WSYS_EXP static void distance(_In_ const w_vector2& pValue1, _In_ const w_vector2& pValue2, _Out_ float& pResult);
            WSYS_EXP static void distance_squared(_In_ const w_vector2& pValue1, _In_ const w_vector2& pValue2, _Out_ float& pResult);
            WSYS_EXP static void dot(_In_ const w_vector2&  pValue1, _In_  const w_vector2& pValue2, _Out_ float& pResult);
            WSYS_EXP static void normalize(_In_ const w_vector2& pValue, _Out_ w_vector2& pResult);
            WSYS_EXP static void reflect(_In_ const w_vector2& pVector, _In_ const w_vector2& pNormal, _Out_ w_vector2& pResult);
            WSYS_EXP static void min(_In_ const w_vector2& pValue1, _In_ const w_vector2& pValue2, _Out_ w_vector2& pResult);
            WSYS_EXP static void max(_In_ w_vector2& pValue1, _In_ const w_vector2& pValue2, _Out_ w_vector2& pResult);
            WSYS_EXP static void clamp(_In_ const w_vector2& pValue1, _In_ const w_vector2& pMin, _In_ const w_vector2& pMax, _Out_ w_vector2& pResult);
            WSYS_EXP static void lerp(_In_ const w_vector2& pValue1, _In_ const w_vector2& pValue2, _In_ const float& pAmount, _Out_ w_vector2& pResult);
            WSYS_EXP static void barycentric(_In_ const w_vector2& pValue1, _In_ const w_vector2& pValue2, _In_ const w_vector2& pValue3,
                _In_ const float& pAmount1, _In_ const float& pAmount2, _Out_ w_vector2& pResult);
            WSYS_EXP static void smooth_step(_In_ const w_vector2& pValue1, _In_ const w_vector2& pValue2, float pAmount, _Out_ w_vector2& pResult);
            WSYS_EXP static void catmullrom(_In_ const w_vector2& pValue1, _In_ const w_vector2& pValue2,
                _In_ const w_vector2& pValue3, _In_ const w_vector2& pValue4,
                _In_ const float& pAmount, _Out_ w_vector2& pResult);
            WSYS_EXP static void hermite(_In_ const w_vector2& pValue1, _In_ const w_vector2& pTangent1,
                _In_ const w_vector2& pValue2, _In_ const w_vector2& pTangent2,
                _In_ const float& pAmount, _Out_ w_vector2& pResult);

            WSYS_EXP static void transform_normal(_In_ const w_vector2& pNormal, _In_ const w_matrix& pMatrix, _Out_ w_vector2& pResult);
            WSYS_EXP static void transform_normal(_In_ const std::vector<w_vector2>& pSourceArray, 
                _In_ const w_matrix& pMatrix, 
                _Out_ std::vector<w_vector2>& pDestinationArray);
            WSYS_EXP static void transform_normal(_In_ const std::vector<w_vector2>& pSourceArray,
                _In_ int pSourceIndex, _In_ const w_matrix& pMatrix,
                _Out_ std::vector<w_vector2>& pDestinationArray, _In_ int pDestinationIndex, _In_ int length);

            WSYS_EXP static void transform(_In_ const w_vector2& pPosition, _In_ const w_matrix& pMatrix, _Out_ w_vector2 pResult);
            //static void transform(_In_ const w_vector2& pValue, _In_ const w_quaternion& pRotation, _Out_ w_vector2& pResult);
            WSYS_EXP static void transform(_In_ const std::vector<w_vector2>& pSourceArray, _In_ const w_matrix& pMatrix,
                _Out_ std::vector<w_vector2>& pDestinationArray);
            WSYS_EXP static void transform(
                _In_ const std::vector<w_vector2>& pSourceArray, _In_ int pSourceIndex,
                _In_  const w_matrix& pMatrix,
                _Out_ std::vector<w_vector2> pDestinationArray, _In_ int pDestinationIndex, _In_ int pLength);
            //WSYS_EXP static void transform(
            //    _In_ const std::vector<w_vector2>& pSourceArray,
            //    _In_ const w_quaternion& pRotation,
            //    _In_ std::vector<w_vector2>& pDestinationArray);

            //WSYS_EXP static void transform(_In_ const std::vector<w_vector2>& pSourceArray, _In_ int pSourceIndex,
            //    _In_ const w_quaternion& pRotation,
            //    _In_ std::vector<w_vector2> pDestinationArray, _In_ int pDestinationIndex, _In_ int pLength);

            float x;
            float y;

        private:
            static w_vector2 _zero;
            static w_vector2 _one;
            static w_vector2 _unit_x;
            static w_vector2 _unit_y;
        };

        inline w_vector2 operator -(_In_ const w_vector2& pValue)
        {
            w_vector2 _vector;
            _vector.x = -pValue.x;
            _vector.y = -pValue.y;
            return _vector;
        }

        inline bool operator ==(_In_ const w_vector2& pValue1, _In_ const w_vector2& pValue2)
        {
            return (pValue1.x == pValue2.x) && (pValue1.y == pValue2.y);
        }

        inline bool operator !=(_In_ const w_vector2& pValue1, _In_ const w_vector2 pValue2)
        {
            if (pValue1.x == pValue2.x)
            {
                return !(pValue1.y == pValue2.y);
            }
            return true;
        }

        inline w_vector2 operator +(_In_ const w_vector2& pValue1, _In_ const w_vector2& pValue2)
        {
            w_vector2 _vector;
            _vector.x = pValue1.x + pValue2.x;
            _vector.y = pValue1.y + pValue2.y;
            return _vector;
        }

        inline w_vector2 operator -(_In_ const w_vector2& pValue1, _In_ const w_vector2& pValue2)
        {
            w_vector2 _vector;
            _vector.x = pValue1.x - pValue2.x;
            _vector.y = pValue1.y - pValue2.y;
            return _vector;
        }

        inline w_vector2 operator *(_In_ const w_vector2& pValue1, _In_ const w_vector2& pValue2)
        {
            w_vector2 _vector;
            _vector.x = pValue1.x * pValue2.x;
            _vector.y = pValue1.y * pValue2.y;
            return _vector;
        }

        inline w_vector2 operator *(_In_ const w_vector2& pValue, _In_ const float& pScaleFactor)
        {
            w_vector2 _vector;
            _vector.x = pValue.x * pScaleFactor;
            _vector.y = pValue.y * pScaleFactor;
            return _vector;
        }

        inline w_vector2 operator *(_In_ const float& pScaleFactor, _In_ const w_vector2& pValue)
        {
            w_vector2 _vector;
            _vector.x = pValue.x * pScaleFactor;
            _vector.y = pValue.y * pScaleFactor;
            return _vector;
        }

        inline w_vector2 operator /(_In_ const w_vector2& pValue1, _In_ const w_vector2& pValue2)
        {
            w_vector2 _vector;
            _vector.x = pValue1.x / pValue2.x;
            _vector.y = pValue1.y / pValue2.y;
            return _vector;
        }

        inline w_vector2 operator /(_In_ const w_vector2& pValue1, _In_ const float& pDivider)
        {
            w_vector2 _vector;
            float _num = 1.0f / pDivider;
            _vector.x = pValue1.x * _num;
            _vector.y = pValue1.y * _num;
            return _vector;
        }
    }
}

#ifdef __GNUC__
#pragma GCC visibility pop
#endif

#endif
