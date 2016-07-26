/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_font.h
	Description		 : The font class. This class modified and improved based on Microsoft DXUT library https://github.com/Microsoft/DXUT
	Comment          :
*/

#ifndef __W_FONT__
#define __W_FONT__

#include <vector>

#include <DirectXMath.h>

//Wolf
#include "w_object.h"
#include <memory>
#include "w_graphics/w_textures/w_texture_2D.h"

namespace wolf
{
	namespace gui
	{
		struct w_font_node
		{
			WCHAR name[MAX_PATH];
			LONG height;
			LONG weight;
		};

		struct w_sprite_vertex
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT4 color;
			DirectX::XMFLOAT2 tex;
		};

		class w_font : public system::w_object
		{
		public:
			w_font(_In_ ID3D11Device1* pDevice, _In_ ID3D11InputLayout* pInputLayout);
			virtual ~w_font();

			void begin_text();
			void draw_text(
				ID3D11Device1* pDevice,
				ID3D11DeviceContext1* pContext,
				LPCWSTR pText,
				const RECT& pRectScreen,
				DirectX::XMFLOAT4 pFontColor,
				float pWidth,
				float pHeight,
				bool pCenter);
			void end_text(ID3D11Device1* pDevice, ID3D11DeviceContext1* pContext);

			ULONG release() override;

#pragma region Getters

			ID3D11ShaderResourceView* get_srv() const { return this->_font_texture->get_srv(); }

#pragma endregion

		private:
			typedef system::w_object					_super;
			std::unique_ptr<graphics::w_texture_2D>		_font_texture;
			ID3D11Buffer*								_font_buffer;
			UINT										_font_buffer_bytes;
			std::vector<w_sprite_vertex>				_font_vertices;
			ID3D11InputLayout*							_input_layout;
		};
	}
}

#endif