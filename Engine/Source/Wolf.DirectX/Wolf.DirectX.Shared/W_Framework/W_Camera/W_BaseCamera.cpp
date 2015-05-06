#include "Wolf.DirectX.Shared.PCH.h"
#include "W_BaseCamera.h"

using namespace DirectX;
using namespace Wolf::Framework;

W_BaseCamera::W_BaseCamera() : aspectRatio(1.777f), nearPlane(0.01f), farPlane(1000.0f),
position(0, 0, 1.0f), lookAt(0, 0, 0), up(0, 1, 0), fieldOfView(70.0f)
{
	UpdateProjection();
}

W_BaseCamera::~W_BaseCamera()
{

}

void W_BaseCamera::UpdateView()
{
	auto vPosition = XMLoadFloat3(&this->position);
	auto vLook = XMLoadFloat3(&this->lookAt);
	auto vUp = XMLoadFloat3(&this->up);
	this->view = XMMatrixLookAtRH(vPosition, vLook, vUp);
}

void W_BaseCamera::UpdateProjection()
{
	float fovAngleY = this->fieldOfView * XM_PI / 180.0f;

	if (this->aspectRatio < 1.0f)
	{
		///
		/// portrait or snap view
		///
		this->up = XMFLOAT3(1.0f, 0.0f, 0.0f);
		fovAngleY = 120.0f * XM_PI / 180.0f;
	}
	else
	{
		///
		/// landscape view
		///
		this->up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	}

	this->projection = XMMatrixPerspectiveFovRH(fovAngleY, this->aspectRatio, this->nearPlane, this->farPlane);
}

ULONG W_BaseCamera::Release()
{
	if (IsReleased()) return 0;
	
	return W_Object::Release();
}

#pragma region Setters

void W_BaseCamera::SetAspectRatio(float pValue)
{
	this->aspectRatio = pValue;
}

void W_BaseCamera::SetPosition(XMFLOAT3 pValue)
{
	this->position = pValue;
}

#pragma endregion