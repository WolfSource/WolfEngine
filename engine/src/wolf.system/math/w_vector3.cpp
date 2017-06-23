#include "w_system_pch.h"
#include "w_vector3.h"
#include <assert.h>

using namespace std;
using namespace wolf::system;

w_vector3 w_vector3::_zero = w_vector3();
w_vector3 w_vector3::_one = w_vector3(1.0f, 1.0f, 1.0f);
w_vector3 w_vector3::_unit_x = w_vector3(1.0f, 0.0f, 0.0f);
w_vector3 w_vector3::_unit_y = w_vector3(0.0f, 1.0f, 0.0f);
w_vector3 w_vector3::_unit_z = w_vector3(0.0f, 0.0f, 1.0f);
w_vector3 w_vector3::_up = w_vector3(0.0f, 1.0f, 0.0f);
w_vector3 w_vector3::_down = w_vector3(0.0f, -1.0f, 0.0f);
w_vector3 w_vector3::_right = w_vector3(1.0f, 0.0f, 0.0f);
w_vector3 w_vector3::_left = w_vector3(-1.0f, 0.0f, 0.0f);
w_vector3 w_vector3::_forward = w_vector3(0.0f, 0.0f, -1.0f);
w_vector3 w_vector3::_backward = w_vector3(0.0f, 0.0f, 1.0f);

w_vector3::w_vector3() : 
    x(0), y(0), z(0)
{
}

w_vector3::w_vector3(_In_ const float& pX, _In_ const float& pY, _In_ const float& pZ) :
    x(pX), y(pY), z(pZ)
{
}

w_vector3::w_vector3(_In_ const float& pValue) :
    x(pValue), y(pValue), z(pValue)
{
}

w_vector3::w_vector3(_In_ const w_vector2& pValue, _In_ const float& pZ) :
    x(pValue.x), y(pValue.y), z(pZ)
{
}

float w_vector3::length()
{
    float _num = this->x * this->x + this->y * this->y + this->z * this->z;
    return (float)std::sqrt((double)_num);
}

float w_vector3::length_squared()
{
    return this->x * this->x + this->y * this->y + this->z * this->z;
}

void w_vector3::normalize()
{
    float _num2 = this->x * this->x + this->y * this->y + this->z * this->z;
    float _num = 1.0f / ((float)std::sqrt((double)_num2));
    this->x *= _num;
    this->y *= _num;
    this->z *= _num;
}

std::string w_vector3::to_string()
{
    return "{x:" + std::to_string(this->x) + ",y:" + std::to_string(this->y) + ",z:" + std::to_string(this->z) + "}";
}

#pragma region Static Methods

w_vector3 w_vector3::zero()
{
    return _zero;
}

w_vector3 w_vector3::one()
{
    return _one;
}

w_vector3 w_vector3::unit_x()
{
    return _unit_x;
}

w_vector3 w_vector3::unit_y()
{
    return _unit_y;
}

w_vector3 w_vector3::unit_z()
{
    return _unit_z;
}

w_vector3 w_vector3::up()
{
    return _up;
}

w_vector3 w_vector3::down()
{
    return _down;
}

w_vector3 w_vector3::right()
{
    return _right;
}

w_vector3 w_vector3::left()
{
    return _left;
}

w_vector3 w_vector3::forward()
{
    return _forward;
}

w_vector3 w_vector3::backward()
{
    return _backward;
}

void w_vector3::distance(_In_ const w_vector3& pValue1, _In_ const w_vector3& pValue2, _Out_ float& pResult)
{
    float _num2 = pValue1.x - pValue2.x;
    float _num1 = pValue1.y - pValue2.y;
    float _num = pValue1.z - pValue2.z;
    float _num3 = _num2 * _num2 + _num1 * _num1 + _num * _num;
    pResult = (float)std::sqrt((double)_num3);
}

void w_vector3::distance_squared(_In_ const w_vector3& pValue1, _In_ const w_vector3& pValue2, _Out_ float& pResult)
{
    float _num2 = pValue1.x - pValue2.x;
    float _num1 = pValue1.y - pValue2.y;
    float _num = pValue1.z - pValue2.z;
    pResult = _num2 * _num2 + _num1 * _num1 + _num * _num;
}

void w_vector3::dot(_In_ const w_vector3&  pValue1, _In_  const w_vector3& pValue2, _Out_ float& pResult)
{
    pResult = pValue1.x * pValue2.x + pValue1.y * pValue2.y + pValue1.z * pValue2.z;
}

void w_vector3::normalize(_In_ const w_vector3& pValue, _Out_ w_vector3& pResult)
{
    float _num2 = pValue.x * pValue.x + pValue.y * pValue.y + pValue.z * pValue.z;
    float _num = 1.0f / ((float)std::sqrt((double)_num2));
    pResult.x = pValue.x * _num;
    pResult.y = pValue.y * _num;
    pResult.z = pValue.z * _num;
}

void w_vector3::cross(_In_ const w_vector3& pVector1, _In_ const w_vector3& pVector2, _Out_ w_vector3& result)
{
    float _num3 = (pVector1.y * pVector2.z) - (pVector1.z * pVector2.y);
    float _num2 = (pVector1.z * pVector2.x) - (pVector1.x * pVector2.z);
    float _num = (pVector1.x * pVector2.y) - (pVector1.y * pVector2.x);
    result.x = _num3;
    result.y = _num2;
    result.z = _num;
}

void w_vector3::reflect(_In_ const w_vector3& pVector, _In_ const w_vector3& pNormal, _Out_ w_vector3& pResult)
{
    float _num = pVector.x * pNormal.x + pVector.y * pNormal.y + pVector.z * pNormal.z;
    pResult.x = pVector.x - 2.0f * _num * pNormal.x;
    pResult.y = pVector.y - 2.0f * _num * pNormal.y;
    pResult.z = pVector.z - 2.0f * _num * pNormal.z;
}

void w_vector3::min(_In_ const w_vector3& pValue1, _In_ const w_vector3& pValue2, _Out_ w_vector3& pResult)
{
    pResult.x = (pValue1.x < pValue2.x) ? pValue1.x : pValue2.x;
    pResult.y = (pValue1.y < pValue2.y) ? pValue1.y : pValue2.y;
    pResult.z = (pValue1.z < pValue2.z) ? pValue1.z : pValue2.z;
}

void w_vector3::max(_In_ w_vector3& pValue1, _In_ const w_vector3& pValue2, _Out_ w_vector3& pResult)
{
    pResult.x = (pValue1.x > pValue2.x) ? pValue1.x : pValue2.x;
    pResult.y = (pValue1.y > pValue2.y) ? pValue1.y : pValue2.y;
    pResult.z = (pValue1.z > pValue2.z) ? pValue1.z : pValue2.z;
}

void w_vector3::clamp(_In_ const w_vector3& pValue1, _In_ const w_vector3& pMin, _In_ const w_vector3& pMax, _Out_ w_vector3& pResult)
{
    float _x = pValue1.x;
    _x = (_x > pMax.x) ? pMax.x : _x;
    _x = (_x < pMin.x) ? pMin.x : _x;
    float _y = pValue1.y;
    _y = (_y > pMax.y) ? pMax.y : _y;
    _y = (_y < pMin.y) ? pMin.y : _y;
    float _z = pValue1.z;
    _z = (_z > pMax.z) ? pMax.z : _z;
    _z = (_z < pMin.z) ? pMin.z : _z;
    pResult.x = _x;
    pResult.y = _y;
    pResult.z = _z;
}

void w_vector3::lerp(_In_ const w_vector3& pValue1, _In_ const w_vector3& pValue2, _In_ const float& pAmount, _Out_ w_vector3& pResult)
{
    pResult.x = pValue1.x + ((pValue2.x - pValue1.x) * pAmount);
    pResult.y = pValue1.y + ((pValue2.y - pValue1.y) * pAmount);
    pResult.z = pValue1.z + ((pValue2.z - pValue1.z) * pAmount);
}

void w_vector3::barycentric(_In_ const w_vector3& pValue1, _In_ const w_vector3& pValue2, _In_ const w_vector3& pValue3,
    _In_ const float& pAmount1, _In_ const float& pAmount2, _Out_ w_vector3& pResult)
{
    pResult.x = (pValue1.x + (pAmount1 * (pValue2.x - pValue1.x))) + (pAmount2 * (pValue3.x - pValue1.x));
    pResult.y = (pValue1.y + (pAmount1 * (pValue2.y - pValue1.y))) + (pAmount2 * (pValue3.y - pValue1.y));
    pResult.z = (pValue1.z + (pAmount1 * (pValue2.z - pValue1.z))) + (pAmount2 * (pValue3.z - pValue1.z));
}

void w_vector3::smooth_step(_In_ const w_vector3& pValue1, _In_ const w_vector3& pValue2, _In_ float pAmount, _Out_ w_vector3& pResult)
{
    pAmount = (pAmount > 1.0f) ? 1.0f : ((pAmount < 0.0f) ? 0.0f : pAmount);
    pAmount = (pAmount * pAmount) * (3.0f - (2.0f * pAmount));
    pResult.x = pValue1.x + ((pValue2.x - pValue1.x) * pAmount);
    pResult.y = pValue1.y + ((pValue2.y - pValue1.y) * pAmount);
    pResult.z = pValue1.z + ((pValue2.z - pValue1.z) * pAmount);
}

void w_vector3::catmullrom(_In_ const w_vector3& pValue1, _In_ const w_vector3& pValue2,
    _In_ const w_vector3& pValue3, _In_ const w_vector3& pValue4,
    _In_ const float& pAmount, _Out_ w_vector3& pResult)
{
    float _num = pAmount * pAmount;
    float _num2 = pAmount * _num;
    pResult.x = 0.5f * ((((2.0f * pValue2.x) + ((-pValue1.x + pValue3.x) * pAmount)) + (((((2.0f * pValue1.x) - (5.0f * pValue2.x)) + (4.0f * pValue3.x)) - pValue4.x) * _num)) + ((((-pValue1.x + (3.0f * pValue2.x)) - (3.0f * pValue3.x)) + pValue4.x) * _num2));
    pResult.y = 0.5f * ((((2.0f * pValue2.y) + ((-pValue1.y + pValue3.y) * pAmount)) + (((((2.0f * pValue1.y) - (5.0f * pValue2.y)) + (4.0f * pValue3.y)) - pValue4.y) * _num)) + ((((-pValue1.y + (3.0f * pValue2.y)) - (3.0f * pValue3.y)) + pValue4.y) * _num2));
    pResult.z = 0.5f * ((((2.0f * pValue2.z) + ((-pValue1.z + pValue3.z) * pAmount)) + (((((2.0f * pValue1.z) - (5.0f * pValue2.z)) + (4.0f * pValue3.z)) - pValue4.z) * _num)) + ((((-pValue1.z + (3.0f * pValue2.z)) - (3.0f * pValue3.z)) + pValue4.z) * _num2));
}

void w_vector3::hermite(_In_ const w_vector3& pValue1, _In_ const w_vector3& pTangent1,
    _In_ const w_vector3& pValue2, _In_ const w_vector3& pTangent2,
    _In_ const float& pAmount, _Out_ w_vector3& pResult)
{
    float _num = pAmount * pAmount;
    float _num2 = pAmount * _num;
    float _num6 = ((2.0f * _num2) - (3.0f * _num)) + 1.0f;
    float _num5 = (-2.0f * _num2) + (3.0f * _num);
    float _num4 = (_num2 - (2.0f * _num)) + pAmount;
    float _num3 = _num2 - _num;
    pResult.x = (((pValue1.x * _num6) + (pValue2.x * _num5)) + (pTangent1.x * _num4)) + (pTangent2.x * _num3);
    pResult.y = (((pValue1.y * _num6) + (pValue2.y * _num5)) + (pTangent1.y * _num4)) + (pTangent2.y * _num3);
    pResult.z = (((pValue1.z * _num6) + (pValue2.z * _num5)) + (pTangent1.z * _num4)) + (pTangent2.z * _num3);
}

void w_vector3::transform_normal(_In_ const w_vector3& pNormal, _In_ const w_matrix& pMatrix, _Out_ w_vector3& pResult)
{
    float _num2 = (pNormal.x * pMatrix.M11()) + (pNormal.y * pMatrix.M21()) + (pNormal.z * pMatrix.M31());
    float _num1 = (pNormal.x * pMatrix.M12()) + (pNormal.y * pMatrix.M22()) + (pNormal.z * pMatrix.M32());
    float _num = (pNormal.x * pMatrix.M13()) + (pNormal.y * pMatrix.M23()) + (pNormal.z * pMatrix.M33());
    pResult.x = _num2;
    pResult.y = _num1;
    pResult.z = _num;
}

void w_vector3::transform_normal(_In_ const std::vector<w_vector3>& pSourceArray,
    _In_ const w_matrix& pMatrix,
    _In_ std::vector<w_vector3>& pDestinationArray)
{
    auto _size = pSourceArray.size();
    if (_size == 0) return;
    if (pDestinationArray.size() != _size)
    {
        pDestinationArray.resize(_size);
    }
    float _x, _y, _z;
    for (size_t i = 0; i < _size; ++i)
    {
        _x = pSourceArray[i].x;
        _y = pSourceArray[i].y;
        _z = pSourceArray[i].z;
        pDestinationArray[i].x = _x * pMatrix.M11() + _y * pMatrix.M21() + _z * pMatrix.M31();
        pDestinationArray[i].y = _x * pMatrix.M12() + _y * pMatrix.M22() + _z * pMatrix.M32();
        pDestinationArray[i].z = _x * pMatrix.M13() + _y * pMatrix.M23() + _z * pMatrix.M33();
    }
}

void w_vector3::transform_normal(
    _In_ const std::vector<w_vector3>& pSourceArray,
    _In_ int pSourceIndex, _In_ const w_matrix& pMatrix,
    _In_ std::vector<w_vector3>& pDestinationArray, _In_ int pDestinationIndex, _In_ int pLength)
{
    assert(pSourceArray.size() < (pSourceIndex + pLength));
    assert(pDestinationArray.size() < (pDestinationIndex + pLength));

    float _x, _y, _z;
    while (pLength > 0)
    {
        _x = pSourceArray[pSourceIndex].x;
        _y = pSourceArray[pSourceIndex].y;
        _z = pSourceArray[pSourceIndex].z;
        pDestinationArray[pDestinationIndex].x = _x * pMatrix.M11() + _y * pMatrix.M21() + _y * pMatrix.M31();
        pDestinationArray[pDestinationIndex].y = _x * pMatrix.M12() + _y * pMatrix.M22() + _y * pMatrix.M32();
        pDestinationArray[pDestinationIndex].z = _x * pMatrix.M13() + _y * pMatrix.M23() + _y * pMatrix.M33();
        pSourceIndex++;
        pDestinationIndex++;
        pLength--;
    }
}

void w_vector3::transform(_In_ const w_vector3& pPosition, _In_ const w_matrix& pMatrix, _Out_ w_vector3 pResult)
{
    float _num2 = pPosition.x * pMatrix.M11() + pPosition.y * pMatrix.M21() + pPosition.z * pMatrix.M31() + pMatrix.M41();
    float _num1 = pPosition.x * pMatrix.M12() + pPosition.y * pMatrix.M22() + pPosition.z * pMatrix.M32() + pMatrix.M42();
    float _num = pPosition.x * pMatrix.M13() + pPosition.y * pMatrix.M23() + pPosition.z * pMatrix.M33() + pMatrix.M43();

    pResult.x = _num2;
    pResult.y = _num1;
    pResult.z = _num;
}

//void w_vector3::transform(_In_ const w_vector3& pValue, _In_ const w_quaternion& pRotation, _Out_ w_vector3& pResult)
//{
//    float _num12 = pRotation.x + pRotation.x;
//    float _num2 = pRotation.y + pRotation.y;
//    float _num = pRotation.z + pRotation.z;
//    float _num11 = pRotation.w * _num12;
//    float _num10 = pRotation.w * _num2;
//    float _num9 = pRotation.w * _num;
//    float _num8 = pRotation.x * _num12;
//    float _num7 = pRotation.x * _num2;
//    float _num6 = pRotation.x * _num;
//    float _num5 = pRotation.y * _num2;
//    float _num4 = pRotation.y * _num;
//    float _num3 = pRotation.y * _num;
//    float _num15 = ((pValue.x * ((1.0f - _num5) - _num3)) + (pValue.y * (_num7 - _num9))) + (pValue.z * (_num6 + _num10));
//    float _num14 = ((pValue.x * (_num7 + _num9)) + (pValue.y * ((1.0f - _num8) - _num3))) + (pValue.z * (_num4 - _num11));
//    float _num13 = ((pValue.x * (_num6 - _num10)) + (pValue.y * (_num4 + _num11))) + (pValue.z * ((1.0f - _num8) - _num5));
//    pResult.x = _num15;
//    pResult.y = _num14;
//    pResult.z = _num13;
//}

void w_vector3::transform(
    _In_ const std::vector<w_vector3>& pSourceArray,
    _In_ const w_matrix& pMatrix,
    _Out_ std::vector<w_vector3>& pDestinationArray)
{
    auto _size = pSourceArray.size();
    if (_size == 0) return;
    if (pDestinationArray.size() != _size)
    {
        pDestinationArray.resize(_size);
    }
    float _x, _y, _z;
    for (size_t i = 0; i < _size; ++i)
    {
        _x = pSourceArray[i].x;
        _y = pSourceArray[i].y;
        _z = pSourceArray[i].z;
        pDestinationArray[i].x = _x * pMatrix.M11() + _y * pMatrix.M21() + _z * pMatrix.M31() + pMatrix.M41();
        pDestinationArray[i].y = _x * pMatrix.M12() + _y * pMatrix.M22() + _z * pMatrix.M32() + pMatrix.M42();
        pDestinationArray[i].z = _x * pMatrix.M13() + _y * pMatrix.M23() + _z * pMatrix.M33() + pMatrix.M43();
    }
}

void w_vector3::transform(
    _In_ const std::vector<w_vector3>& pSourceArray, _In_ int pSourceIndex,
    _In_  const w_matrix& pMatrix,
    _Out_ std::vector<w_vector3> pDestinationArray, _In_ int pDestinationIndex, _In_ int pLength)
{
    assert(pSourceArray.size() < (pSourceIndex + pLength));
    assert(pDestinationArray.size() < (pDestinationIndex + pLength));

    float _x, _y, _z;
    while (pLength > 0)
    {
        _x = pSourceArray[pSourceIndex].x;
        _y = pSourceArray[pSourceIndex].y;
        _z = pSourceArray[pSourceIndex].z;
        pDestinationArray[pDestinationIndex].x = _x * pMatrix.M11() + _y * pMatrix.M21() + _z * pMatrix.M31() + pMatrix.M41();
        pDestinationArray[pDestinationIndex].y = _x * pMatrix.M12() + _y * pMatrix.M22() + _z * pMatrix.M32() + pMatrix.M42();
        pDestinationArray[pDestinationIndex].z = _x * pMatrix.M13() + _y * pMatrix.M23() + _z * pMatrix.M33() + pMatrix.M43();
        pSourceIndex++;
        pDestinationIndex++;
        pLength--;
    }
}

//void w_vector3::transform(
//    _In_ const std::vector<w_vector3>& pSourceArray,
//    _In_ const w_quaternion& pRotation,
//    _In_ std::vector<w_vector3>& pDestinationArray)
//{
//    auto _size = pSourceArray.size();
//    if (_size == 0) return;
//    if (pDestinationArray.size() != _size)
//    {
//        pDestinationArray.resize(_size);
//    }
//
//    float _num16 = pRotation.x + pRotation.x;
//    float _num6 = pRotation.y + pRotation.y;
//    float _num2 = pRotation.z + pRotation.z;
//    float _num15 = pRotation.w * _num16;
//    float _num14 = pRotation.w * _num6;
//    float _num13 = pRotation.w * _num2;
//    float _num12 = pRotation.x * _num16;
//    float _num11 = pRotation.x * _num6;
//    float _num10 = pRotation.x * _num2;
//    float _num9 = pRotation.y * _num6;
//    float _num8 = pRotation.y * _num2;
//    float _num7 = pRotation.z * _num2;
//    float _num25 = (1.0f - _num9) - _num7;
//    float _num24 = _num11 - _num13;
//    float _num23 = _num10 + _num14;
//    float _num22 = _num11 + _num13;
//    float _num21 = (1.0f - _num12) - _num7;
//    float _num20 = _num8 - _num15;
//    float _num19 = _num10 - _num14;
//    float _num18 = _num8 + _num15;
//    float _num17 = (1.0f - _num12) - _num9;
//
//    float _x, _y, _z;
//    for (int i = 0; i < _size; ++i)
//    {
//        _x = pSourceArray[i].x;
//        _y = pSourceArray[i].y;
//        _z = pSourceArray[i].z;
//        pDestinationArray[i].x = _x * _num25 + _y * _num24 + _z * _num23;
//        pDestinationArray[i].y = _x * _num22 + _y * _num21 + _z * _num20;
//        pDestinationArray[i].z = _x * _num19 + _y * _num18 + _z * _num17;
//    }
//}
//
//void w_vector3::transform(
//    _In_ const std::vector<w_vector3>& pSourceArray, _In_ int pSourceIndex,
//    _In_ const w_quaternion& pRotation,
//    _In_ std::vector<w_vector3> pDestinationArray, _In_ int pDestinationIndex, _In_ int pLength)
//{
//    assert(pSourceArray.size() < (pSourceIndex + pLength));
//    assert(pDestinationArray.size() < (pDestinationIndex + pLength));
//
//    float _num15 = pRotation.x + pRotation.x;
//    float _num5 = pRotation.y + pRotation.y;
//    float _num = pRotation.z + pRotation.z;
//    float _num14 = pRotation.w * _num15;
//    float _num13 = pRotation.w * _num5;
//    float _num12 = pRotation.w * _num;
//    float _num11 = pRotation.x * _num15;
//    float _num10 = pRotation.x * _num5;
//    float _num9 = pRotation.x * _num;
//    float _num8 = pRotation.y * _num5;
//    float _num7 = pRotation.y * _num;
//    float _num6 = pRotation.z * _num;
//    float _num24 = (1.0f - _num8) - _num6;
//    float _num23 = _num10 - _num12;
//    float _num22 = _num9 + _num13;
//    float _num21 = _num10 + _num12;
//    float _num20 = (1.0f - _num11) - _num6;
//    float _num19 = _num7 - _num14;
//    float _num18 = _num9 - _num13;
//    float _num17 = _num7 + _num14;
//    float _num16 = (1.0f - _num11) - _num8;
//
//    float _x, _y, _z;
//    while (pLength > 0)
//    {
//        _x = pSourceArray[pSourceIndex].x;
//        _y = pSourceArray[pSourceIndex].y;
//        _z = pSourceArray[pSourceIndex].z;
//        pDestinationArray[pDestinationIndex].x = _x * _num24 + _y * _num23 + _z * _num22;
//        pDestinationArray[pDestinationIndex].y = _x * _num21 + _y * _num20 + _z * _num19;
//        pDestinationArray[pDestinationIndex].z = _x * _num18 + _y * _num17 + _z * _num16;
//        pSourceIndex++;
//        pDestinationIndex++;
//        pLength--;
//    }
//}

#pragma endregion