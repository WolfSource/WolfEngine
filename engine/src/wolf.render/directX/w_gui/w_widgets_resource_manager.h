/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_widgets_resource_manager.h 
	Description		 : The resource manager for all widgets. This class modified and improved based on Microsoft DXUT library https://github.com/Microsoft/DXUT
	Comment          :
*/

#ifndef __W_WIDGETS_RESOURCE_MANAGER__
#define __W_WIDGETS_RESOURCE_MANAGER__

#include <Windows.h>
#include <DirectXMath.h>
#include <vector>
#include <memory>

//Wolf
#include "w_object.h"
#include "w_texture_node.h"
#include "w_user_controls/w_widget.h"
#include "w_graphics/w_shaders/w_shader.h"

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

		struct w_gui_screen_vertex
		{
			float x, y, z;
			DirectX::XMFLOAT4 color;
			float tu, tv;
		};

		class w_widgets_resource_manager : public system::w_object
		{
		public:
			DX_EXP w_widgets_resource_manager();
			DX_EXP ~w_widgets_resource_manager();

			// D3D11 specific
			DX_EXP HRESULT on_device_created(_In_ ID3D11Device1* pDevice, _In_ ID3D11DeviceContext1* pContext);
			DX_EXP HRESULT on_swapChain_resized(const UINT pWidth, const UINT pHeight);

			void on_releasing_swapChain();
			void on_destroy_device();
			void store_D3D_state(_In_ ID3D11DeviceContext1* pContext);
			void restore_D3D_state(_In_ ID3D11DeviceContext1* pContext);
			void apply_render_UI(_In_ ID3D11DeviceContext1* pContext);
			void apply_render_UI_Untex(_In_ ID3D11DeviceContext1* pContext);

			void begin_sprites();
			void end_sprites(_In_ ID3D11Device1* pDevice, _In_ ID3D11DeviceContext1* pContext);

			bool register_widget(_In_ w_widget* pWidget);
			void unregister_widget(_In_ w_widget* pWidget);
			void enable_keyboard_input_for_all_widgets();

			int add_font(_In_z_ LPCWSTR pName, _In_ LONG pHeight, _In_ LONG pWeight);
			int add_texture(_In_z_ LPCWSTR pFilename);
			int add_texture(_In_z_ LPCWSTR pResourceName, _In_ HMODULE pHResourceModule);

			HRESULT update_texture_cache(ID3D11Device1* pDevice, ID3D11DeviceContext1* pContext, UINT pIndex);

			// States
			ID3D11DepthStencilState* m_pDepthStencilStateUI11;
			ID3D11RasterizerState* m_pRasterizerStateUI11;
			ID3D11BlendState* m_pBlendStateUI11;
			ID3D11SamplerState* m_pSamplerStateUI11;

			// Stored states
			ID3D11DepthStencilState* m_pDepthStencilStateStored11;
			UINT m_StencilRefStored11;
			ID3D11RasterizerState* m_pRasterizerStateStored11;
			ID3D11BlendState* m_pBlendStateStored11;
			float m_BlendFactorStored11[4];
			UINT m_SampleMaskStored11;
			ID3D11SamplerState* m_pSamplerStateStored11;

			// Sprite workaround
			ID3D11Buffer* m_pSpriteBuffer11;
			UINT m_SpriteBufferBytes11;
			std::vector<wolf::content_pipeline::vertex_declaration_structs::vertex_color_uv> m_SpriteVertices;

			UINT back_buffer_width;
			UINT back_buffer_height;

			std::vector<w_widget*> widgets;            // Widgets registered

#pragma region Getters

			size_t						get_texture_cache_size() const						{ return this->texture_cache.size(); }
			w_font_node*				get_font_node(_In_ size_t iIndex) const				{ return this->font_cache[iIndex]; }
			w_texture_node*				get_texture_node(_In_ size_t iIndex) const			{ return this->texture_cache[iIndex]; }
			ID3D11InputLayout*			get_input_layout() const							{ return this->_shader->get_input_layout(); }

#pragma endregion

		protected:
			HRESULT create_texture(ID3D11Device1* pDevice, ID3D11DeviceContext1* pContext, _In_ UINT pIndex);
			
			std::vector<w_texture_node*> texture_cache;   // Shared textures
			std::vector<w_font_node*> font_cache;         // Shared fonts

		private:
			typedef system::w_object _super;

			//shader between all dialogs
			std::unique_ptr<wolf::graphics::w_shader>					_shader;
		};
	}
}

#endif
