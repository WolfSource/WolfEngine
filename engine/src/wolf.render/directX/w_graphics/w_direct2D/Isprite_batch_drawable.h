/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : ISpriteBatchDrawable.h
	Description		 : An interface for SpriteBatchDrawable
	Comment          :
*/

#ifndef __W_ISPRITEBATCHDRAWABLE_H__
#define __W_ISPRITEBATCHDRAWABLE_H__

#include <Windows.h>

namespace wolf
{
	namespace graphics
	{
		namespace direct2D
		{
			__interface Isprite_batch_drawable
			{
				HRESULT draw();
				ULONG	release();
			};
		}
	}
}

#endif