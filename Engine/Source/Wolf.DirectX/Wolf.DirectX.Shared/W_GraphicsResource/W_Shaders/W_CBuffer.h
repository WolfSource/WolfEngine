/*
	Project			 : Wolf PlayOut, powered by Wolf Engine Copyright (c) Pooya Eimandar. https://github.com/PooyaEimandar/WolfEngine
					   Please direct any bug to hello@WolfStudio.co
	File Name        : W_CBuffer.h
	File Description : Constant buffer of shader
	Comment          :
*/

#pragma once

#include "W_GraphicsDeviceManager.h"
#include <DirectXMath.h>

namespace Wolf
{
	namespace Graphics
	{
		template<typename T>
		class W_CBuffer sealed
		{
		public:
			W_CBuffer(){}
			~W_CBuffer()
			{
				Release();
			}

			//Load the constant buffer
			void Load(const std::shared_ptr<W_GraphicsDevice>& gDevice)
			{
				this->gDevice = gDevice;

				D3D11_BUFFER_DESC bufferDesc;
				ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

				bufferDesc.ByteWidth = sizeof(T);
				bufferDesc.Usage = D3D11_USAGE_DEFAULT;
				bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

				auto hr = this->gDevice->device->CreateBuffer(&bufferDesc, nullptr, &this->buffer);
				OnFailed(hr, "Create buffer", "W_CBuffer", false);
			}

			// Writes new data into the constant buffer.
			void SetData(T& value)
			{
				auto context = this->gDevice->d3dContext;
				D3D11_MAPPED_SUBRESOURCE mappedResource;
				ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
				
				this->Const = &value;
				auto hr = context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
				{
					OnFailed(hr);
					*(T*) mappedResource.pData = value;
					context->Unmap(buffer, 0);
				}
			}

			//Update constant buffer and subresources
			void Update()
			{
				auto context = this->gDevice->context;
				context->UpdateSubresource(this->buffer, 0, nullptr, &this->Const, 0, 0);
			}

			//Release resources
			ULONG Release()
			{
				if (this->isReleased) return 0;
				
				if (this->buffer != nullptr)
				{
					this->buffer->Release();
					this->buffer = nullptr;
				}
				this->gDevice = nullptr;
				this->isReleased = true;
				return 1;
			}

#pragma region Getters
			// Get the underlying D3D constant buffer.
			ID3D11Buffer* GetBuffer() const { return this->buffer; }
#pragma endregion

			T Const;

		private:
			bool isReleased;
			ID3D11Buffer* buffer;
			std::shared_ptr<W_GraphicsDevice> gDevice;
		};
	}
}