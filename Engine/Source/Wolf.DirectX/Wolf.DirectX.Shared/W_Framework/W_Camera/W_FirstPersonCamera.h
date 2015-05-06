/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @Wolf_Engine on twitter
	Name			 : W_FirstPersonCamera.h
	Description		 : The declaration of first person camera
	Comment          :
*/

#pragma once

#include "W_BaseCamera.h"

namespace Wolf
{
	namespace Framework
	{
#define CameraMovementSpeed 100
#define CameraRotationSpeed 1

		//The first person camera
		class W_FirstPersonCamera sealed : public W_BaseCamera
		{
		public:
			W_FirstPersonCamera();
			virtual ~W_FirstPersonCamera();

			//Update view matrix of camera
			void UpdateView();
			//Update world matrix of camera
			void UpdateWorld(DirectX::XMFLOAT3 MoveVector);
			//Reset camera to default worldViewProjection 
			void Reset();
			//void ProcessInput(float time);
			//Update camera
			void Update(float time);

#pragma region Getters
			//Get the angle(XY-Rotation) of the camera
			DirectX::XMFLOAT2 GetAngle() const			{ return this->angle; }
#pragma endregion

#pragma region Getters
			//Set the angle(XY-Rotation) of camera
			void SetAngle(DirectX::XMFLOAT2 pValue)		{ this->angle = pValue; }

#pragma endregion

		private:
			DirectX::XMFLOAT2 angle;
			DirectX::XMMATRIX rotationMatrix;
			DirectX::XMFLOAT2 lastPointerPos;
			DirectX::XMFLOAT2 currentPointerPos;
		};
	}
}