/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @Wolf_Engine on twitter
	Name			 : W_Quad.h
	Description		 : Render a quad with primitives
	Comment          :
*/

#pragma once

#include "W_Mesh.h"

namespace Wolf
{
	namespace Graphics
	{
		//Initialize and render a quad with primitives
		class W_Quad : public W_Mesh
		{
		public:
			API W_Quad(const std::shared_ptr<Graphics::W_GraphicsDevice>& pGDevice);
			API virtual ~W_Quad();

			API HRESULT Load(bool pLoadDeafultTexture2D = true, std::wstring pTexture2DPath = L"", bool pIsSprite = false);
			API void IsSprite(bool pValue);

			API virtual ULONG Release() override;

		private:
			class Impl;
			std::unique_ptr<Impl> impl;
		};
	}
}
