/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_constant_buffer.h
	Description		 : Constant HLSL buffer
	Comment          :
*/

#ifndef __W_CBUFFER_H__
#define __W_CBUFFER_H__

#include "w_graphics_device_manager.h"
#include <DirectXMath.h>

namespace wolf
{
	namespace graphics
	{
		template<typename T>
		class w_constant_buffer sealed
		{
		public:
			w_constant_buffer() : _is_released(false) {}
			~w_constant_buffer()
			{
				release();
			}

			//Load the constant buffer
			void load(_In_ ID3D11Device1* pDevice)
			{
				D3D11_BUFFER_DESC _bufferDesc;
				std::memset(&_bufferDesc, 0, sizeof(D3D11_BUFFER_DESC));

				_bufferDesc.ByteWidth = sizeof(T);
				_bufferDesc.Usage = D3D11_USAGE_DEFAULT;
				_bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

				auto _hr = pDevice->CreateBuffer(&_bufferDesc, nullptr, &this->_buffer);
				V(_hr, "Create buffer", "W_CBuffer", 3, false);
			}

			// Writes new data into the constant buffer.
			void set_data(_In_ ID3D11DeviceContext1* pContext, T& pValue)
			{
				D3D11_MAPPED_SUBRESOURCE _mappedResource;
				std::memset(&_mappedResource, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));

				this->data = &pValue;
				auto _hr = pContext->Map(_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &_mappedResource);
				{
					V(hr);
					*(T*) _mappedResource.pData = pValue;
					pContext->Unmap(_buffer, 0);
				}
			}

			//Update constant buffer and subresources
			void update(_In_ ID3D11DeviceContext1* pDevice)
			{
				pDevice->UpdateSubresource(this->_buffer.Get(), 0, nullptr, &this->data, 0, 0);
			}

			//Release resources
			ULONG release()
			{
				if (this->_is_released) return 0;
				
				COMPTR_RELEASE(this->_buffer);
								
				this->_is_released = true;

				return 1;
			}

#pragma region Getters
			// Get the underlying D3D constant buffer.
			ID3D11Buffer* get_buffer() const { return this->_buffer.Get(); }
#pragma endregion

			T data;

		private:
			bool _is_released;
			Microsoft::WRL::ComPtr<ID3D11Buffer> _buffer;
		};
	}
}

#endif