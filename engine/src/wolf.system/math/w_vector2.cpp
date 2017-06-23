#include "w_system_pch.h"
#include "w_vector2.h"
#include <assert.h>

using namespace std;
using namespace wolf::system;

w_vector2 w_vector2::_zero = w_vector2();
w_vector2 w_vector2::_one = w_vector2(1.0f, 1.0f);
w_vector2 w_vector2::_unit_x = w_vector2(1.0f, 0.0f);
w_vector2 w_vector2::_unit_y = w_vector2(0.0f, 1.0f);

w_vector2::w_vector2() : 
    x(0), y(0)
{
}

w_vector2::w_vector2(_In_ const float& pX, _In_ const float& pY) :
    x(pX), y(pY)
{
}

w_vector2::w_vector2(_In_ const float& pValue) :
    x(pValue), y(pValue)
{
}

float w_vector2::length()
{
    float _num = this->x * this->x + this->y * this->y;
    return (float)std::sqrt((double)_num);
}

float w_vector2::length_squared()
{
    return this->x * this->x + this->y * this->y;
}

void w_vector2::normalize()
{
    float _num2 = this->x * this->x + this->y * this->y;
    float _num = 1.0f / ((float)std::sqrt((double)_num2));
    this->x *= _num;
    this->y *= _num;
}

std::string w_vector2::to_string()
{
    return "{x:" + std::to_string(this->x) + ",y:" + std::to_string(this->y) + "}";
}

#pragma region Static Methods

w_vector2 w_vector2::zero()             
{ 
    return _zero; 
}

w_vector2 w_vector2::one() 
{ 
    return _one; 
}

w_vector2 w_vector2::unit_x() 
{ 
    return _unit_x; 
}

w_vector2 w_vector2::unit_y() 
{ 
    return _unit_y; 
}

void w_vector2::distance(_In_ const w_vector2& pValue1, _In_ const w_vector2& pValue2, _Out_ float& pResult)
{
    float _num2 = pValue1.x - pValue2.x;
    float _num = pValue1.y - pValue2.y;
    float _num3 = _num2 * _num2 + _num * _num;
    pResult = (float)std::sqrt((double)_num3);
}

void w_vector2::distance_squared(_In_ const w_vector2& pValue1, _In_ const w_vector2& pValue2, _Out_ float& pResult)
{
    float _num2 = pValue1.x - pValue2.x;
    float _num = pValue1.y - pValue2.y;
    pResult = _num2 * _num2 + _num * _num;
}

void w_vector2::dot(_In_ const w_vector2&  pValue1, _In_  const w_vector2& pValue2, _Out_ float& pResult)
{
    pResult = pValue1.x * pValue2.x + pValue1.y * pValue2.y;
}

void w_vector2::normalize(_In_ const w_vector2& pValue, _Out_ w_vector2& pResult)
{
    float _num2 = pValue.x * pValue.x + pValue.y * pValue.y;
    float _num = 1.0f / ((float)std::sqrt((double)_num2));
    pResult.x = pValue.x * _num;
    pResult.y = pValue.y * _num;
}

void w_vector2::reflect(_In_ const w_vector2& pVector, _In_ const w_vector2& pNormal, _Out_ w_vector2& pResult)
{
    float _num = pVector.x * pNormal.x + pVector.y * pNormal.y;
    pResult.x = pVector.x - (2.0f * _num * pNormal.x);
    pResult.y = pVector.y - (2.0f * _num * pNormal.y);
}

void w_vector2::min(_In_ const w_vector2& pValue1, _In_ const w_vector2& pValue2, _Out_ w_vector2& pResult)
{
    pResult.x = (pValue1.x < pValue2.x) ? pValue1.x : pValue2.x;
    pResult.y = (pValue1.y < pValue2.y) ? pValue1.y : pValue2.y;
}

void w_vector2::max(_In_ w_vector2& pValue1, _In_ const w_vector2& pValue2, _Out_ w_vector2& pResult)
{
    pResult.x = (pValue1.x > pValue2.x) ? pValue1.x : pValue2.x;
    pResult.y = (pValue1.y > pValue2.y) ? pValue1.y : pValue2.y;
}

void w_vector2::clamp(_In_ const w_vector2& pValue1, _In_ const w_vector2& pMin, _In_ const w_vector2& pMax, _Out_ w_vector2& pResult)
{
    float _x = pValue1.x;
    _x = (_x > pMax.x) ? pMax.x : _x;
    _x = (_x < pMin.x) ? pMin.x : _x;
    float _y = pValue1.y;
    _y = (_y > pMax.y) ? pMax.y : _y;
    _y = (_y < pMin.y) ? pMin.y : _y;
    pResult.x = _x;
    pResult.y = _y;
}

void w_vector2::lerp(_In_ const w_vector2& pValue1, _In_ const w_vector2& pValue2, _In_ const float& pAmount, _Out_ w_vector2& pResult)
{
    pResult.x = pValue1.x + ((pValue2.x - pValue1.x) * pAmount);
    pResult.y = pValue1.y + ((pValue2.y - pValue1.y) * pAmount);
}

void w_vector2::barycentric(_In_ const w_vector2& pValue1, _In_ const w_vector2& pValue2, _In_ const w_vector2& pValue3,
    _In_ const float& pAmount1, _In_ const float& pAmount2, _Out_ w_vector2& pResult)
{
    pResult.x = (pValue1.x + (pAmount1 * (pValue2.x - pValue1.x))) + (pAmount2 * (pValue3.x - pValue1.x));
    pResult.y = (pValue1.y + (pAmount1 * (pValue2.y - pValue1.y))) + (pAmount2 * (pValue3.y - pValue1.y));
}

void w_vector2::smooth_step(_In_ const w_vector2& pValue1, _In_ const w_vector2& pValue2, _In_ float pAmount, _Out_ w_vector2& pResult)
{
    pAmount = (pAmount > 1.0f) ? 1.0f : ((pAmount < 0.0f) ? 0.0f : pAmount);
    pAmount = (pAmount * pAmount) * (3.0f - (2.0f * pAmount));
    pResult.x = pValue1.x + ((pValue2.x - pValue1.x) * pAmount);
    pResult.y = pValue1.y + ((pValue2.y - pValue1.y) * pAmount);
}

void w_vector2::catmullrom(_In_ const w_vector2& pValue1, _In_ const w_vector2& pValue2,
    _In_ const w_vector2& pValue3, _In_ const w_vector2& pValue4,
    _In_ const float& pAmount, _Out_ w_vector2& pResult)
{
    float _num = pAmount * pAmount;
    float _num2 = pAmount * _num;
    pResult.x = 0.5f * ((((2.0f * pValue2.x) + ((-pValue1.x + pValue3.x) * pAmount)) + (((((2.0f * pValue1.x) - (5.0f * pValue2.x)) + (4.0f * pValue3.x)) - pValue4.x) * _num)) + ((((-pValue1.x + (3.0f * pValue2.x)) - (3.0f * pValue3.x)) + pValue4.x) * _num2));
    pResult.y = 0.5f * ((((2.0f * pValue2.y) + ((-pValue1.y + pValue3.y) * pAmount)) + (((((2.0f * pValue1.y) - (5.0f * pValue2.y)) + (4.0f * pValue3.y)) - pValue4.y) * _num)) + ((((-pValue1.y + (3.0f * pValue2.y)) - (3.0f * pValue3.y)) + pValue4.y) * _num2));
}

void w_vector2::hermite(_In_ const w_vector2& pValue1, _In_ const w_vector2& pTangent1,
    _In_ const w_vector2& pValue2, _In_ const w_vector2& pTangent2,
    _In_ const float& pAmount, _Out_ w_vector2& pResult)
{
    float _num = pAmount * pAmount;
    float _num2 = pAmount * _num;
    float _num6 = ((2.0f * _num2) - (3.0f * _num)) + 1.0f;
    float _num5 = (-2.0f * _num2) + (3.0f * _num);
    float _num4 = (_num2 - (2.0f * _num)) + pAmount;
    float _num3 = _num2 - _num;
    pResult.x = (((pValue1.x * _num6) + (pValue2.x * _num5)) + (pTangent1.x * _num4)) + (pTangent2.x * _num3);
    pResult.y = (((pValue1.y * _num6) + (pValue2.y * _num5)) + (pTangent1.y * _num4)) + (pTangent2.y * _num3);
}

void w_vector2::transform_normal(_In_ const w_vector2& pNormal, _In_ const w_matrix& pMatrix, _Out_ w_vector2& pResult)
{
    float _num2 = (pNormal.x * pMatrix.M11()) + (pNormal.y * pMatrix.M21());
    float _num = (pNormal.x * pMatrix.M12()) + (pNormal.y * pMatrix.M22());
    pResult.x = _num2;
    pResult.y = _num;
}

void w_vector2::transform_normal(_In_ const std::vector<w_vector2>& pSourceArray,
    _In_ const w_matrix& pMatrix,
    _In_ std::vector<w_vector2>& pDestinationArray)
{
    auto _size = pSourceArray.size();
    if (_size == 0) return;
    if (pDestinationArray.size() != _size)
    {
        pDestinationArray.resize(_size);
    }
    float _x, _y;
    for (size_t i = 0; i < _size; ++i)
    {
        _x = pSourceArray[i].x;
        _y = pSourceArray[i].y;
        pDestinationArray[i].x = _x * pMatrix.M11() + _y * pMatrix.M21();
        pDestinationArray[i].y = _x * pMatrix.M12() + _y * pMatrix.M22();
    }
}


void w_vector2::transform_normal(
    _In_ const std::vector<w_vector2>& pSourceArray,
    _In_ int pSourceIndex, _In_ const w_matrix& pMatrix,
    _In_ std::vector<w_vector2>& pDestinationArray, _In_ int pDestinationIndex, _In_ int pLength)
{
    assert(pSourceArray.size() < (pSourceIndex + pLength));
    assert(pDestinationArray.size() < (pDestinationIndex + pLength));

    float _x, _y;
    while (pLength > 0)
    {
        _x = pSourceArray[pSourceIndex].x;
        _y = pSourceArray[pSourceIndex].y;
        pDestinationArray[pDestinationIndex].x = _x * pMatrix.M11() + _y * pMatrix.M21();
        pDestinationArray[pDestinationIndex].y = _x * pMatrix.M12() + _y * pMatrix.M22();
        pSourceIndex++;
        pDestinationIndex++;
        pLength--;
    }
}

void w_vector2::transform(_In_ const w_vector2& pPosition, _In_ const w_matrix& pMatrix, _Out_ w_vector2 pResult)
{
    float _num2 = pPosition.x * pMatrix.M11() + pPosition.y * pMatrix.M21() + pMatrix.M41();
    float _num = pPosition.x * pMatrix.M12() + pPosition.y * pMatrix.M22() + pMatrix.M42();
    pResult.x = _num2;
    pResult.y = _num;
}

//void w_vector2::transform(_In_ const w_vector2& pValue, _In_ const w_quaternion& pRotation, _Out_ w_vector2& pResult)
//{
//    float _num10 = pRotation.x + pRotation.x;
//    float _num5 = pRotation.y + pRotation.y;
//    float _num4 = pRotation.z + pRotation.z;
//    float _num3 = pRotation.w * _num4;
//    float _num9 = pRotation.x * _num10;
//    float _num2 = pRotation.x * _num5;
//    float _num8 = pRotation.y * _num5;
//    float _num = pRotation.z * _num4;
//    float _num7 = (pValue.x * ((1.0f - _num8) - _num)) + (pValue.y * (_num2 - _num3));
//    float _num6 = (pValue.x * (_num2 + _num3)) + (pValue.y * ((1.0f - _num9) - _num));
//    pResult.x = _num7;
//    pResult.y = _num6;
//}

void w_vector2::transform(
    _In_ const std::vector<w_vector2>& pSourceArray, 
    _In_ const w_matrix& pMatrix,
    _Out_ std::vector<w_vector2>& pDestinationArray)
{
    auto _size = pSourceArray.size();
    if (_size == 0) return;
    if (pDestinationArray.size() != _size)
    {
        pDestinationArray.resize(_size);
    }
    float _x, _y;
    for (size_t i = 0; i < _size; ++i)
    {
        _x = pSourceArray[i].x;
        _y = pSourceArray[i].y;
        pDestinationArray[i].x = _x * pMatrix.M11() + _y * pMatrix.M21() + pMatrix.M41();
        pDestinationArray[i].y = _x * pMatrix.M12() + _y * pMatrix.M22() + pMatrix.M42();
    }
}

void w_vector2::transform(
    _In_ const std::vector<w_vector2>& pSourceArray, _In_ int pSourceIndex,
    _In_  const w_matrix& pMatrix,
    _Out_ std::vector<w_vector2> pDestinationArray, _In_ int pDestinationIndex, _In_ int pLength)
{
    assert(pSourceArray.size() < (pSourceIndex + pLength));
    assert(pDestinationArray.size() < (pDestinationIndex + pLength));

    float _x, _y;
    while (pLength > 0)
    {
        _x = pSourceArray[pSourceIndex].x;
        _y = pSourceArray[pSourceIndex].y;
        pDestinationArray[pDestinationIndex].x = _x * pMatrix.M11() + _y * pMatrix.M21() + pMatrix.M41();
        pDestinationArray[pDestinationIndex].y = _x * pMatrix.M12() + _y * pMatrix.M22() + pMatrix.M42();
        pSourceIndex++;
        pDestinationIndex++;
        pLength--;
    }
}

//void w_vector2::transform(
//    _In_ const std::vector<w_vector2>& pSourceArray,
//    _In_ const w_quaternion& pRotation,
//    _In_ std::vector<w_vector2>& pDestinationArray)
//{
//    auto _size = pSourceArray.size();
//    if (_size == 0) return;
//    if (pDestinationArray.size() != _size)
//    {
//        pDestinationArray.resize(_size);
//    }
//
//    float _num15 = pRotation.x + pRotation.x;
//    float _num8 = pRotation.y + pRotation.y;
//    float _num7 = pRotation.z + pRotation.z;
//    float _num6 = pRotation.w * _num7;
//    float _num14 = pRotation.x * _num15;
//    float _num5 = pRotation.x * _num8;
//    float _num13 = pRotation.y * _num8;
//    float _num4 = pRotation.z * _num7;
//    float _num12 = (1.0f - _num13) - _num4;
//    float _num11 = _num5 - _num6;
//    float _num10 = _num5 + _num6;
//    float _num9 = (1.0f - _num14) - _num4;
//
//    float _x, _y;
//    for (size_t i = 0; i < _size; ++i)
//    {
//        _x = pSourceArray[i].x;
//        _y = pSourceArray[i].y;
//        pDestinationArray[i].x = (_x * _num12) + (_y * _num11);
//        pDestinationArray[i].y = (_x * _num10) + (_y * _num9);
//    }
//}
//
//void w_vector2::transform(
//    _In_ const std::vector<w_vector2>& pSourceArray, _In_ int pSourceIndex,
//    _In_ const w_quaternion& pRotation,
//    _In_ std::vector<w_vector2> pDestinationArray, _In_ int pDestinationIndex, _In_ int pLength)
//{
//    assert(pSourceArray.size() < (pSourceIndex + pLength));
//    assert(pDestinationArray.size() < (pDestinationIndex + pLength));
//
//    float _num14 = pRotation.x + pRotation.x;
//    float _num7 = pRotation.y + pRotation.y;
//    float _num6 = pRotation.z + pRotation.z;
//    float _num5 = pRotation.w * _num6;
//    float _num13 = pRotation.x * _num14;
//    float _num4 = pRotation.x * _num7;
//    float _num12 = pRotation.y * _num7;
//    float _num3 = pRotation.z * _num6;
//    float _num11 = (1.0f - _num12) - _num3;
//    float _num10 = _num4 - _num5;
//    float _num9 = _num4 + _num5;
//    float _num8 = (1.0f - _num13) - _num3;
//
//    float _x, _y;
//    while (pLength > 0)
//    {
//        _x = pSourceArray[pSourceIndex].x;
//        _y = pSourceArray[pSourceIndex].y;
//        pDestinationArray[pDestinationIndex].x = _x * _num11 + _y * _num10;
//        pDestinationArray[pDestinationIndex].y = _x * _num9 + _y * _num8;
//        pSourceIndex++;
//        pDestinationIndex++;
//        pLength--;
//    }
//}

#pragma endregion