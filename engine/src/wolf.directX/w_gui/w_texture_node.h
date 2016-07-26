/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_texture_node.h 
	Description		 : The texture node structure. This class modified and improved based on Microsoft DXUT library https://github.com/Microsoft/DXUT
	Comment          :
*/

#ifndef __W_TEXTURE_NODE_H__
#define __W_TEXTURE_NODE_H__

#include <Windows.h>
//#include <d3d11_2.h>

namespace wolf
{
	namespace gui
	{
		struct w_texture_node
		{
			bool fileSource;  // True if this texture is loaded from a file. False if from resource.
			HMODULE hResourceModule;
			int nResourceID;   // Resource ID. If 0, string-based ID is used and stored in strFilename.
			WCHAR strFilename[MAX_PATH];
			DWORD dwWidth;
			DWORD dwHeight;
			ID3D11Texture2D* texture2D;
			ID3D11ShaderResourceView* pTexResView11;
		};
	}
}

#endif
