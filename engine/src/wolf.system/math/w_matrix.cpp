#include "w_system_pch.h"
#include "w_matrix.h"
#include "w_vector3.h"

using namespace std;
using namespace wolf::system;

w_matrix w_matrix::_identity = w_matrix(
    1.0f, 0.0f, 0.0f, 0.0f, 
    0.0f, 1.0f, 0.0f, 0.0f, 
    0.0f, 0.0f, 1.0f, 0.0f, 
    0.0f, 0.0f, 0.0f, 1.0f);

w_matrix::w_matrix()
{
    std::memset(&_m[0][0], 0.0f, sizeof(float) * 16);
}

w_matrix::w_matrix(
    _In_ const float& pM11, _In_ const float& pM12, _In_ const float& pM13, _In_ const float& pM14,
    _In_ const float& pM21, _In_ const float& pM22, _In_ const float& pM23, _In_ const float& pM24,
    _In_ const float& pM31, _In_ const float& pM32, _In_ const float& pM33, _In_ const float& pM34,
    _In_ const float& pM41, _In_ const float& pM42, _In_ const float& pM43, _In_ const float& pM44)
{
    _m[0][0] = pM11; _m[0][1] = pM12; _m[0][2] = pM13; _m[0][3] = pM14;
    _m[1][0] = pM21; _m[1][1] = pM22; _m[1][2] = pM23; _m[1][3] = pM24;
    _m[2][0] = pM31; _m[2][1] = pM32; _m[2][2] = pM33; _m[2][3] = pM34;
    _m[3][0] = pM41; _m[3][1] = pM42; _m[3][2] = pM43; _m[3][3] = pM44;
}

w_matrix::~w_matrix()
{

}

w_vector3 w_matrix::up()
{
    w_vector3 _vector;
    _vector.x = M21();
    _vector.y = M22();
    _vector.z = M23();
    return _vector;
}

void w_matrix::up(_In_ const w_vector3& pValue)
{
    M21(pValue.x);
    M22(pValue.y);
    M23(pValue.z);
}

w_vector3 w_matrix::down()
{
    w_vector3 _vector;
    _vector.x = -M21();
    _vector.y = -M22();
    _vector.z = -M23();
    return _vector;
}

void w_matrix::down(_In_ const w_vector3& pValue)
{
    M21(-pValue.x);
    M22(-pValue.y);
    M23(-pValue.z);
}

w_vector3 w_matrix::right()
{
    w_vector3 _vector;
    _vector.x = M11();
    _vector.y = M12();
    _vector.z = M13();
    return _vector;
}

void w_matrix::right(_In_ const w_vector3& pValue)
{
    M11(pValue.x);
    M12(pValue.y);
    M13(pValue.z);
}

w_vector3 w_matrix::left()
{
    w_vector3 _vector;
    _vector.x = -M11();
    _vector.y = -M12();
    _vector.z = -M13();
    return _vector;
}

void w_matrix::left(_In_ const w_vector3& pValue)
{
    M11(-pValue.x);
    M12(-pValue.y);
    M13(-pValue.z);
}

w_vector3 w_matrix::forward()
{
    w_vector3 _vector;
    _vector.x = -M31();
    _vector.y = -M32();
    _vector.z = -M33();
    return _vector;
}

void w_matrix::forward(_In_ const w_vector3& pValue)
{
    M31(-pValue.x);
    M32(-pValue.y);
    M33(-pValue.z);
}

w_vector3 w_matrix::backward()
{
    w_vector3 _vector;
    _vector.x = M31();
    _vector.y = M32();
    _vector.z = M33();
    return _vector;
}

void w_matrix::backward(_In_ const w_vector3& pValue)
{
    M31(pValue.x);
    M32(pValue.y);
    M33(pValue.z);
}

w_vector3 w_matrix::translation()
{
    w_vector3 _vector;
    _vector.x = M41();
    _vector.y = M42();
    _vector.z = M43();
    return _vector;
}

void w_matrix::translation(_In_ const w_vector3& pValue)
{
    M41(pValue.x);
    M42(pValue.y);
    M43(pValue.z);
}

#pragma region Static Methods

void w_matrix::create_billboard(
    _In_ const w_vector3& pObjectPosition,
    _In_ const w_vector3& pCameraPosition,
    _In_ const w_vector3& pCameraUpVector,
    _In_ const w_vector3* pCameraForwardVector,
    _Out_ w_matrix& pResult)
{
    w_vector3 _vector;
    w_vector3 _vector2;
    w_vector3 _vector3;
    _vector.x = pObjectPosition.x - pCameraPosition.x;
    _vector.y = pObjectPosition.y - pCameraPosition.y;
    _vector.z = pObjectPosition.z - pCameraPosition.z;
    float _num = _vector.length_squared();
    if (_num < 0.0001f)
    {
        _vector = pCameraForwardVector == nullptr ? w_vector3::forward() : -(*pCameraForwardVector);
    }
    else
    {
        _vector = _vector * (float)(1.0f / ((float)std::sqrt((double)_num)));
    }
    w_vector3::cross(pCameraUpVector, _vector, _vector3);
    _vector3.normalize();
    w_vector3::cross(_vector, _vector3, _vector2);

    float __m[16] =
    {
        _vector3.x          , _vector3.y            , _vector3.z            , 0.0f,
        _vector2.x          , _vector2.y            , _vector2.z            , 0.0f,
        _vector.x           , _vector.y             , _vector.z             , 0.0f,
        pObjectPosition.x   , pObjectPosition.y     , pObjectPosition.z     , 1.0f,
    };
    std::memcpy(pResult.data(), &__m[0], 16 * sizeof(float));
}

void w_matrix::create_constrained_billboard(
    _In_ const w_vector3& pObjectPosition,
    _In_ const w_vector3& pCameraPosition,
    _In_ const w_vector3& pRotateAxis,
    _In_ const w_vector3* pCameraForwardVector,
    _In_ const w_vector3* pObjectForwardVector,
    _Out_ w_matrix& pResult)
{
    float _num;
    w_vector3 _vector;
    w_vector3 _vector2;
    w_vector3 _vector3;
    _vector2.x = pObjectPosition.x - pCameraPosition.x;
    _vector2.y = pObjectPosition.y - pCameraPosition.y;
    _vector2.z = pObjectPosition.z - pCameraPosition.z;
    float _num2 = _vector2.length_squared();
    if (_num2 < 0.0001f)
    {
        _vector2 = pCameraForwardVector == nullptr ? w_vector3::forward() : -(*pCameraForwardVector);
    }
    else
    {
        _vector2 = _vector2 * (float)(1.0f / ((float)std::sqrt((double)_num2)));
    }
    w_vector3 _vector4 = pRotateAxis;
    w_vector3::dot(pRotateAxis, _vector2, _num);
    if (std::abs(_num) > 0.9982547f)
    {
        if (pObjectForwardVector == nullptr)
        {
            _num = ((pRotateAxis.x * w_vector3::forward().x) + (pRotateAxis.y * w_vector3::forward().y)) + (pRotateAxis.z * w_vector3::forward().z);
            _vector = (std::abs(_num) > 0.9982547f) ? w_vector3::right() : w_vector3::forward();
        }
        else
        {
            _vector = *pObjectForwardVector;
            w_vector3::dot(pRotateAxis, _vector, _num);
            if (std::abs(_num) > 0.9982547f)
            {
                _num = ((pRotateAxis.x * w_vector3::forward().x) + (pRotateAxis.y * w_vector3::forward().y)) + (pRotateAxis.z * w_vector3::forward().z);
                _vector = (std::abs(_num) > 0.9982547f) ? w_vector3::right() : w_vector3::forward();
            }
        }

        w_vector3::cross(pRotateAxis, _vector, _vector3);
        _vector3.normalize();
        w_vector3::cross(_vector3, pRotateAxis, _vector);
        _vector.normalize();
    }
    else
    {
        w_vector3::cross(pRotateAxis, _vector2, _vector3);
        _vector3.normalize();
        w_vector3::cross(_vector3, _vector4, _vector);
        _vector.normalize();
    }

    float __m[16] =
    {
        _vector3.x          , _vector3.y            , _vector3.z            , 0.0f,
        _vector4.x          , _vector4.y            , _vector4.z            , 0.0f,
        _vector.x           , _vector.y             , _vector.z             , 0.0f,
        pObjectPosition.x   , pObjectPosition.y     , pObjectPosition.z     , 1.0f,
    };
    std::memcpy(pResult.data(), &__m[0], 16 * sizeof(float));
}

void w_matrix::create_translation(_In_ const w_vector3& pPosition, _Out_ w_matrix& pResult)
{
    float __m[16] =
    {
        1.0f        , 0.0f          , 0.0f          , 0.0f,
        0.0f        , 1.0f          , 0.0f          , 0.0f,
        0.0f        , 0.0f          , 1.0f          , 0.0f,
        pPosition.x , pPosition.y   , pPosition.z   , 0.0f,
    };
    std::memcpy(pResult.data(), &__m[0], 16 * sizeof(float));
}

void w_matrix::create_scale(_In_ const float& pScaleX, _In_ const float& pScaleY, _In_ const float& pScaleZ, _Out_  w_matrix& pResult)
{
    float __m[16] =
    {
        pScaleX     , 0.0f          , 0.0f          , 0.0f,
        0.0f        , pScaleY       , 0.0f          , 0.0f,
        0.0f        , 0.0f          , pScaleZ       , 0.0f,
        0.0f        , 0.0f          , 0.0f          , 1.0f,
    };
    std::memcpy(pResult.data(), &__m[0], 16 * sizeof(float));
}

void w_matrix::create_scale(_In_ const w_vector3& pScale, _Out_ w_matrix& pResult)
{
    float __m[16] =
    {
        pScale.x    , 0.0f          , 0.0f          , 0.0f,
        0.0f        , pScale.y      , 0.0f          , 0.0f,
        0.0f        , 0.0f          , pScale.z      , 0.0f,
        0.0f        , 0.0f          , 0.0f          , 1.0f,
    };
    std::memcpy(pResult.data(), &__m[0], 16 * sizeof(float));
}

void w_matrix::create_scale(_In_ const float& pScale, _Out_ w_matrix& pResult)
{
    float __m[16] =
    {
        pScale      , 0.0f          , 0.0f          , 0.0f,
        0.0f        , pScale        , 0.0f          , 0.0f,
        0.0f        , 0.0f          , pScale        , 0.0f,
        0.0f        , 0.0f          , 0.0f          , 1.0f,
    };
    std::memcpy(pResult.data(), &__m[0], 16 * sizeof(float));
}

void w_matrix::create_rotation_x(_In_ const float& pRadians, _Out_ w_matrix& pResult)
{
    float _num2 = (float)std::cos((double)pRadians);
    float _num = (float)std::sin((double)pRadians);

    float __m[16] =
    {
        1.0f        , 0.0f          , 0.0f          , 0.0f,
        0.0f        , _num2         , _num          , 0.0f,
        0.0f        , -_num         , _num2         , 0.0f,
        0.0f        , 0.0f          , 0.0f          , 1.0f,
    };
    std::memcpy(pResult.data(), &__m[0], 16 * sizeof(float));
}

void w_matrix::create_rotation_y(_In_ const float& pRadians, _Out_ w_matrix& pResult)
{
    float _num2 = (float)std::cos((double)pRadians);
    float _num = (float)std::sin((double)pRadians);

    float __m[16] =
    {
        _num2        , 0.0f          , 0.0f          , 0.0f,
        0.0f        , _num2         , _num          , 0.0f,
        0.0f        , -_num         , _num2         , 0.0f,
        0.0f        , 0.0f          , 0.0f          , 1.0f,
    };
    std::memcpy(pResult.data(), &__m[0], 16 * sizeof(float));

    result.M11 = num2;
    result.M12 = 0f;
    result.M13 = -num;
    result.M14 = 0f;
    result.M21 = 0f;
    result.M22 = 1f;
    result.M23 = 0f;
    result.M24 = 0f;
    result.M31 = num;
    result.M32 = 0f;
    result.M33 = num2;
    result.M34 = 0f;
    result.M41 = 0f;
    result.M42 = 0f;
    result.M43 = 0f;
    result.M44 = 1f;
}

void w_matrix::create_rotation_z(_In_ const float& pRadians, _Out_ w_matrix& pResult)
{
    float num2 = (float)Math.Cos((double)radians);
    float num = (float)Math.Sin((double)radians);
    result.M11 = num2;
    result.M12 = num;
    result.M13 = 0f;
    result.M14 = 0f;
    result.M21 = -num;
    result.M22 = num2;
    result.M23 = 0f;
    result.M24 = 0f;
    result.M31 = 0f;
    result.M32 = 0f;
    result.M33 = 1f;
    result.M34 = 0f;
    result.M41 = 0f;
    result.M42 = 0f;
    result.M43 = 0f;
    result.M44 = 1f;
}

#pragma endregion