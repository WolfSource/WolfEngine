/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_first_person_camera.h
	Description		 : The declaration of first person camera
	Comment          :
*/

#ifndef __W_FIRSTPERSONCAMERA_H__
#define __W_FIRSTPERSONCAMERA_H__

#include "w_camera.h"

namespace wolf
{
	namespace framework
	{

#define CAMERA_MOVEMENT_SPEED 100
#define CAMERA_ROTATION_SPEED 1

		//The first person camera
		class w_first_person_camera sealed : public wolf::content_pipeline::w_camera
		{
		public:
			w_first_person_camera();
			virtual ~w_first_person_camera();

			//Update view matrix of camera
			void update_view();
			//Update world matrix of camera
			void update_world(_In_ DirectX::XMFLOAT3 MoveVector);
			//Reset camera to default worldViewProjection 
			void reset();
			//void ProcessInput(float time);
			//Update camera
			void update(_In_ float time);

#pragma region Getters
			//Get the angle(XY-Rotation) of the camera
			DirectX::XMFLOAT2 get_angle() const					{ return this->_angle; }
#pragma endregion

#pragma region Getters
			//Set the angle(XY-Rotation) of camera
			void set_angle(_In_ DirectX::XMFLOAT2 pValue)		{ this->_angle = pValue; }

#pragma endregion

		private:
			typedef	wolf::content_pipeline::w_camera _super;

			DirectX::XMFLOAT2 _angle;
			DirectX::XMMATRIX _rotationMatrix;
			DirectX::XMFLOAT2 _lastPointerPos;
			DirectX::XMFLOAT2 _currentPointerPos;
		};
	}
}

#endif