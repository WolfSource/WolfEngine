#include "w_system_pch.h"
#include "w_quaternion.h"
//#include "w_vector3.h"

using namespace std;
using namespace wolf::system;

w_quaternion::w_quaternion() : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
{
}

w_quaternion::w_quaternion(_In_ const float& pX, _In_ const float& pY, _In_ const float& pZ, _In_ const float& pW) :
    x(pX), y(pY), z(pZ), w(pW)
{
}

//w_quaternion::w_quaternion(_In_ const w_vector3& pVectorPart, _In_ const float& pScalarPart) :
//    x(pVectorPart.x), y(pVectorPart.y), z(pVectorPart.z), w(pScalarPart)
//{
//}

w_quaternion::~w_quaternion()
{
}
