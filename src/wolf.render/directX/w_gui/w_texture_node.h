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

namespace wolf
{
	namespace gui
	{
		struct w_texture_node
		{
			//True if this texture is loaded from a file. False if from resource.
			bool						is_loaded_from_source; 
			//resource hModule
			HMODULE						resource_module;
			//the ID of resource
			int							resource_id;
			//path of file, MAXPATH is 260 character
			wchar_t						file_name[260];
			//width of texture
			unsigned long				width;
			//height of texture
			unsigned long				height;
			//texture 2d
			ID3D11Texture2D*			texture_2d;
			//shader resource view of texture 2d
			ID3D11ShaderResourceView*	shader_resource_view;
		};
	}
}

#endif
