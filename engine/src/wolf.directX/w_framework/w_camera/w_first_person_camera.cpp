#include "w_directX_pch.h"
#include "w_first_person_camera.h"
//#include "FrameWork\Input\InputManager.h"

using namespace DirectX;
using namespace wolf::framework;

static const XMFLOAT3 INITIAL_POSITION = XMFLOAT3(0, 7, 50);
static const XMFLOAT2 INITIAL_ANGLE = XMFLOAT2(-0.1f, 0.0f);
static const float debounce = 2.0f;

w_first_person_camera::w_first_person_camera() : _angle(INITIAL_ANGLE)
{
	/*this->_position = INITIAL_POSITION;
	_super::update_view();*/
}

w_first_person_camera::~w_first_person_camera()
{

}

void w_first_person_camera::update_view()
{
	//auto forward = XMVectorSet(0.0f, 0.0f, -1.0f, 1.0f);
	//auto _lookAt = XMLoadFloat3(&this->_position) + XMVector3Transform(forward, this->_rotationMatrix);

	//this->_lookAt.x = XMVectorGetX(_lookAt);
	//this->_lookAt.y = XMVectorGetY(_lookAt);
	//this->_lookAt.z = XMVectorGetZ(_lookAt);

	//_super::update_view();
}

void w_first_person_camera::update_world(XMFLOAT3 MoveVector)
{
	//auto vect = XMVector3Transform(XMLoadFloat3(&MoveVector), this->_rotationMatrix);

	//this->_position.x += XMVectorGetX(vect);
	//this->_position.y += XMVectorGetY(vect);
	//this->_position.z += XMVectorGetZ(vect);
}

void w_first_person_camera::reset()
{
	//this->_position = INITIAL_POSITION;
	//this->_angle = INITIAL_ANGLE;
	//update_view();
}

void w_first_person_camera::update(float time)
{
	//this->_rotationMatrix = XMMatrixTranspose(XMMatrixRotationX(this->_angle.x) * XMMatrixRotationY(this->_angle.y));
	//ProcessInput(time);
	//update_view();
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
