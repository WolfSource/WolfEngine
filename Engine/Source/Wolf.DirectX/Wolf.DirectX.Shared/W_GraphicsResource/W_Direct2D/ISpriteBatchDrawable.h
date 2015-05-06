/*
	Project			 : Wolf Engine (http://WolfStudio.co). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to hello@WolfStudio.co or tweet @Wolf_Engine on twitter
	Name			 : ISpriteBatchDrawable.h
	Description		 : An interface for SpriteBatchDrawable
	Comment          :
*/

#pragma once

#include <Windows.h>

namespace Wolf
{
	namespace Graphics
	{
		namespace Direct2D
		{
			__interface ISpriteBatchDrawable
			{
				HRESULT Draw();
			};
		}
	}
}
