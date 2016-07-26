/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_quad.h
	Description		 : Render a quad with primitives
	Comment          :
*/

#ifndef __W_QUAD_H__
#define __W_QUAD_H__

#include "w_mesh.h"

namespace wolf
{
	namespace graphics
	{
		//Initialize and render a quad with primitives
		class w_quad : public w_mesh
		{
		public:
			enum SHADER_TYPE { DEFAULT, TRANSITION_CROSS_X, TRANSITION_CROSS_Y };

			DX_EXP w_quad();
			DX_EXP virtual ~w_quad();

			DX_EXP HRESULT load(_In_ ID3D11Device1* pDevice, bool pLoadDeafultTexture2D = true, std::wstring pTexture2DPath = L"", SHADER_TYPE pShaderType = SHADER_TYPE::DEFAULT);

			DX_EXP virtual ULONG release() override;

		private:
			typedef	 w_mesh			_super;
			class Impl;
			std::unique_ptr<Impl>	_impl;
		};
	}
}

#endif