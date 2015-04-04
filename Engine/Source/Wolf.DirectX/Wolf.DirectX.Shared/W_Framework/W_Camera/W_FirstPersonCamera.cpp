#include "Wolf.DirectX.Shared.PCH.h"
#include "W_FirstPersonCamera.h"
//#include "FrameWork\Input\InputManager.h"

using namespace DirectX;
using namespace Wolf::Framework;

const XMFLOAT3 InitialiPosition = XMFLOAT3(0, 7, 50);
const XMFLOAT2 InitialAngle = XMFLOAT2(-0.1f, 0.0f);
const float debounce = 2.0f;

W_FirstPersonCamera::W_FirstPersonCamera() : angle(InitialAngle)
{
	this->position = InitialiPosition;
	W_BaseCamera::UpdateView();
}

W_FirstPersonCamera::~W_FirstPersonCamera()
{

}

void W_FirstPersonCamera::UpdateView()
{
	auto forward = XMVectorSet(0.0f, 0.0f, -1.0f, 1.0f);
	auto _lookAt = XMLoadFloat3(&position) + XMVector3Transform(forward, this->rotationMatrix);

	this->lookAt.x = XMVectorGetX(_lookAt);
	this->lookAt.y = XMVectorGetY(_lookAt);
	this->lookAt.z = XMVectorGetZ(_lookAt);

	W_BaseCamera::UpdateView();
}

void W_FirstPersonCamera::UpdateWorld(XMFLOAT3 MoveVector)
{
	auto vect = XMVector3Transform(XMLoadFloat3(&MoveVector), this->rotationMatrix);

	this->position.x += XMVectorGetX(vect);
	this->position.y += XMVectorGetY(vect);
	this->position.z += XMVectorGetZ(vect);
}

void W_FirstPersonCamera::Reset()
{
	this->position = InitialiPosition;
	this->angle = InitialAngle;
	UpdateView();
}

void W_FirstPersonCamera::Update(float time)
{
	this->rotationMatrix = XMMatrixTranspose(XMMatrixRotationX(this->angle.x) * XMMatrixRotationY(this->angle.y));
	//ProcessInput(time);
	UpdateView();
}

//void FirstPersonCamera::ProcessInput(float time)
//{
//	bool ForceUpdate = false;
//	this->lastPointerPos = this->currentPointerPos;
//	this->currentPointerPos = InputManager::pointerState.Position;
//
//	auto moveVect = XMFLOAT3(0, 0, 0);	
//
//#pragma region Change camera rotation
//
//	if (InputManager::pointerState.IsRightButtonPressed())
//	{
//		if (currentPointerPos.y - lastPointerPos.y > debounce)
//		{
//			this->angle.x += time * CameraRotationSpeed;
//		}
//		else if (currentPointerPos.y - lastPointerPos.y < -debounce)
//		{
//			this->angle.x -= time * CameraRotationSpeed;
//		}
//		else if (currentPointerPos.x - lastPointerPos.x > debounce)
//		{
//			this->angle.y += time * CameraRotationSpeed;
//		}
//		else if (currentPointerPos.x - lastPointerPos.x < -debounce)
//		{
//			this->angle.y -= time * CameraRotationSpeed;
//		}
//
//		if (this->angle.x > 1.4)
//		{
//			this->angle.x = 1.4f;
//		}
//		else if (this->angle.x < -1.4)
//		{
//			this->angle.x = -1.4f;
//		}
//
//		if (this->angle.y > XM_PI)
//		{
//			this->angle.y -= 2 * XM_PI;
//		}
//		else if (this->angle.y < -XM_PI)
//		{
//			this->angle.y += 2 * XM_PI;
//		}
//		ForceUpdate = true;
//	}
//
//#pragma endregion
//
//#pragma region Change camera movements
//
//	if (InputManager::keyboardState.IsKeyDown(VirtualKey::D))
//	{
//		moveVect.x += time * CameraMovementSpeed;
//		ForceUpdate = true;
//	}
//	else if (InputManager::keyboardState.IsKeyDown(VirtualKey::A))
//	{
//		moveVect.x -= time * CameraMovementSpeed;
//		ForceUpdate = true;
//	}
//	else if (InputManager::keyboardState.IsKeyDown(VirtualKey::W))
//	{
//		moveVect.z -= time * CameraMovementSpeed;
//		ForceUpdate = true;
//	}
//	else if (InputManager::keyboardState.IsKeyDown(VirtualKey::S))
//	{
//		moveVect.z += time * CameraMovementSpeed;
//		ForceUpdate = true;
//	}
//	else if (InputManager::keyboardState.IsKeyDown(VirtualKey::Q))
//	{
//		moveVect.y += time * CameraMovementSpeed;
//		ForceUpdate = true;
//	}
//	else if (InputManager::keyboardState.IsKeyDown(VirtualKey::Z))
//	{
//		moveVect.y -= time * CameraMovementSpeed;
//		ForceUpdate = true;
//	}
//
//#pragma endregion
//
//	if (ForceUpdate)
//	{
//		UpdateWorld(moveVect);
//	}
//}
