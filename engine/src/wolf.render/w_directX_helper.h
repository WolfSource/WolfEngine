/*
	Project			 : Wolf Engine (http://WolfSource.io). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_directX_math.h
	Description		 : The math helper of Wolf
	Comment          :
*/

#ifndef __W_DIRECTX_HELPER_H__
#define __W_DIRECTX_HELPER_H__

#include <Windows.h>
#include <DirectXMath.h>
#include <glm/matrix.hpp>
#include <w_math.h>
#include <w_color.h>
#include <d3d12.h>

//inline bool operator == (_In_ const D2D1_COLOR_F& lValue, _In_ const D2D1_COLOR_F& rValue)
//{
//	return lValue.a == rValue.a && lValue.r == rValue.r && lValue.g == rValue.g && lValue.b == rValue.b;
//}
//
//inline bool operator != (_In_ const D2D1_COLOR_F& lValue, _In_ const D2D1_COLOR_F& rValue)
//{
//	return !(lValue == rValue);
//}

namespace DirectX
{
	inline XMFLOAT4 DWORD_COLOR_TO_XMFLOAT4(_In_ DWORD pC)
	{
		//Convert ARGB(DWORD) to RGBA(in the range of 0..1)
		return DirectX::XMFLOAT4(
			((pC >> 16) & 0xFF) / 255.0f,//R
			((pC >> 8) & 0xFF) / 255.0f,//G
			(pC & 0xFF) / 255.0f,//B
			((pC >> 24) & 0xFF) / 255.0f);//A
	}

	inline XMFLOAT4 W_COLOR_TO_XMFLOAT4(_In_ w_color pC)
	{
		return DirectX::XMFLOAT4(pC.r / 255.0f, pC.g / 255.0f, pC.b / 255.0f, pC.a / 255.0f);
	}

	inline w_color XMFLOAT4_TO_W_COLOR(_In_ XMFLOAT4 pC)
	{
		return w_color(static_cast<UINT>(pC.x * 255.0f), static_cast<UINT>(pC.y * 255.0f), static_cast<UINT>(pC.z * 255.0f), static_cast<UINT>(pC.w * 255.0f));
	}

	inline XMMATRIX XMMatrixFromGLMMatrix(_In_ glm::tmat4x4<float, _In_ glm::precision::highp> const& pValue)
	{
		return XMMATRIX(
			pValue[0][0], pValue[0][1], pValue[0][2], pValue[0][3],
			pValue[1][0], pValue[1][1], pValue[1][2], pValue[1][3],
			pValue[2][0], pValue[2][1], pValue[2][2], pValue[2][3],
			pValue[3][0], pValue[3][1], pValue[3][2], pValue[3][3]);
	}

	inline std::string GetFeatureLevelStr(_In_ D3D_FEATURE_LEVEL pFeatureLevel)
	{
		std::string _feature_level;
		switch (pFeatureLevel)
		{
		case D3D_FEATURE_LEVEL_9_1:
			_feature_level = "D3D_FEATURE_LEVEL_9_1";
			break;
		case D3D_FEATURE_LEVEL_9_2:
			_feature_level = "D3D_FEATURE_LEVEL_9_2";
			break;
		case D3D_FEATURE_LEVEL_9_3:
			_feature_level = "D3D_FEATURE_LEVEL_9_3";
			break;
		case D3D_FEATURE_LEVEL_10_0:
			_feature_level = "D3D_FEATURE_LEVEL_10_0";
			break;
		case D3D_FEATURE_LEVEL_10_1:
			_feature_level = "D3D_FEATURE_LEVEL_10_1";
			break;
		case D3D_FEATURE_LEVEL_11_0:
			_feature_level = "D3D_FEATURE_LEVEL_11_0";
			break;
		case D3D_FEATURE_LEVEL_11_1:
			_feature_level = "D3D_FEATURE_LEVEL_11_1";
			break;
		case D3D_FEATURE_LEVEL_12_0:
			_feature_level = "D3D_FEATURE_LEVEL_12_0";
			break;
		case D3D_FEATURE_LEVEL_12_1:
			_feature_level = "D3D_FEATURE_LEVEL_12_1";
			break;
		default:
			break;
		}

		return _feature_level;
	}

	inline std::wstring GetFeatureLevelStrW(_In_ D3D_FEATURE_LEVEL pFeatureLevel)
	{
		std::wstring _feature_level;
		switch (pFeatureLevel)
		{
		case D3D_FEATURE_LEVEL_9_1:
			_feature_level = L"D3D_FEATURE_LEVEL_9_1";
			break;
		case D3D_FEATURE_LEVEL_9_2:
			_feature_level = L"D3D_FEATURE_LEVEL_9_2";
			break;
		case D3D_FEATURE_LEVEL_9_3:
			_feature_level = L"D3D_FEATURE_LEVEL_9_3";
			break;
		case D3D_FEATURE_LEVEL_10_0:
			_feature_level = L"D3D_FEATURE_LEVEL_10_0";
			break;
		case D3D_FEATURE_LEVEL_10_1:
			_feature_level = L"D3D_FEATURE_LEVEL_10_1";
			break;
		case D3D_FEATURE_LEVEL_11_0:
			_feature_level = L"D3D_FEATURE_LEVEL_11_0";
			break;
		case D3D_FEATURE_LEVEL_11_1:
			_feature_level = L"D3D_FEATURE_LEVEL_11_1";
			break;
		case D3D_FEATURE_LEVEL_12_0:
			_feature_level = L"D3D_FEATURE_LEVEL_12_0";
			break;
		case D3D_FEATURE_LEVEL_12_1:
			_feature_level = L"D3D_FEATURE_LEVEL_12_1";
			break;
		default:
			break;
		}

		return _feature_level;
	}

	struct CD3DX12_CPU_DESCRIPTOR_HANDLE : public D3D12_CPU_DESCRIPTOR_HANDLE
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE() {}
		explicit CD3DX12_CPU_DESCRIPTOR_HANDLE(const D3D12_CPU_DESCRIPTOR_HANDLE &o) :
			D3D12_CPU_DESCRIPTOR_HANDLE(o)
		{}
		//CD3DX12_CPU_DESCRIPTOR_HANDLE(CD3DX12_DEFAULT) { ptr = 0; }
		CD3DX12_CPU_DESCRIPTOR_HANDLE(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE &other, INT offsetScaledByIncrementSize)
		{
			InitOffsetted(other, offsetScaledByIncrementSize);
		}
		CD3DX12_CPU_DESCRIPTOR_HANDLE(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE &other, INT offsetInDescriptors, UINT descriptorIncrementSize)
		{
			InitOffsetted(other, offsetInDescriptors, descriptorIncrementSize);
		}
		CD3DX12_CPU_DESCRIPTOR_HANDLE& Offset(INT offsetInDescriptors, UINT descriptorIncrementSize)
		{
			ptr += offsetInDescriptors * descriptorIncrementSize;
			return *this;
		}
		CD3DX12_CPU_DESCRIPTOR_HANDLE& Offset(INT offsetScaledByIncrementSize)
		{
			ptr += offsetScaledByIncrementSize;
			return *this;
		}
		bool operator==(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE& other) const
		{
			return (ptr == other.ptr);
		}
		bool operator!=(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE& other) const
		{
			return (ptr != other.ptr);
		}
		CD3DX12_CPU_DESCRIPTOR_HANDLE &operator=(const D3D12_CPU_DESCRIPTOR_HANDLE &other)
		{
			ptr = other.ptr;
			return *this;
		}

		inline void InitOffsetted(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE &base, INT offsetScaledByIncrementSize)
		{
			InitOffsetted(*this, base, offsetScaledByIncrementSize);
		}

		inline void InitOffsetted(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE &base, INT offsetInDescriptors, UINT descriptorIncrementSize)
		{
			InitOffsetted(*this, base, offsetInDescriptors, descriptorIncrementSize);
		}

		static inline void InitOffsetted(_Out_ D3D12_CPU_DESCRIPTOR_HANDLE &handle, _In_ const D3D12_CPU_DESCRIPTOR_HANDLE &base, INT offsetScaledByIncrementSize)
		{
			handle.ptr = base.ptr + offsetScaledByIncrementSize;
		}

		static inline void InitOffsetted(_Out_ D3D12_CPU_DESCRIPTOR_HANDLE &handle, _In_ const D3D12_CPU_DESCRIPTOR_HANDLE &base, INT offsetInDescriptors, UINT descriptorIncrementSize)
		{
			handle.ptr = base.ptr + offsetInDescriptors * descriptorIncrementSize;
		}
	};
}

#endif

