/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @Wolf_Engine on twitter
	Name			 : W_BaseCamera.h
	Description		 : The base class for camera
	Comment          :
*/

#pragma once

#include <DirectXMath.h>
#include <W_Object.h>

namespace Wolf
{
	namespace Framework
	{
		//The base parameters of camera
		class W_BaseCamera : public System::W_Object
		{
		public:
			API W_BaseCamera();
			API virtual ~W_BaseCamera();
			
			//Update the view matrix
			API void UpdateView();
			//Update the projection matrix
			API void UpdateProjection();

			//Release resources
			API virtual ULONG Release() override;

#pragma region Getters

			//Get field of view
			API float GetFieldOfView() const					{ return this->fieldOfView; }
			//Get aspect ratio of the camera
			API float GetAspectRatio() const					{ return this->aspectRatio; }
			//Get near plane of the camera
			API float GetNearPlane() const						{ return this->nearPlane; }
			//Get far plane of the camera
			API float GetFarPlane() const						{ return this->farPlane; }
			//Get view matrix of the camera
			DirectX::XMMATRIX GetView() const					{ return this->view; }
			//Get projection matrix of the camera
			DirectX::XMMATRIX GetProjection() const				{ return this->projection; }
			//Get position of the camera
			API DirectX::XMFLOAT3 GetPosition() const			{ return this->position; }

#pragma region 

#pragma region Setters

			//Set aspect ratio of the camera
			API void SetAspectRatio(float pAspectRatio);
			API void SetPosition(DirectX::XMFLOAT3 pValue);

#pragma endregion

		protected:
			float			  fieldOfView;
			float			  aspectRatio;
			float			  nearPlane;
			float			  farPlane;
			DirectX::XMFLOAT3 up;
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT3 lookAt;
			DirectX::XMFLOAT3 direction;
			DirectX::XMMATRIX view;
			DirectX::XMMATRIX projection;
		};
	}
}