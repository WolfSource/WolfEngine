/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_mesh.h
	Description		 : Render a mesh
	Comment          : 
*/
#ifndef __W_MESH_H__
#define __W_MESH_H__

#include "w_graphics_device_manager.h"

namespace wolf
{
	namespace graphics
	{
		//Represents a 3D model mesh composed of multiple meshpart objects.
		class w_mesh : public system::w_object
		{
		public:
			W_EXP w_mesh();
			W_EXP virtual ~w_mesh();

			//Initialize mesh
			W_EXP void load(_In_ std::shared_ptr<w_graphics_device> pDevice);
                        
			//create vertex buffer
			W_EXP HRESULT create_vertex_buffer(_In_ const void* const pVertices,
				_In_ const UINT pVerticesSize);
                        
			//create pixel buffer
			W_EXP HRESULT create_index_buffer(_In_ const unsigned short* const pIndices,
				_In_ const UINT pIndicesSize);
                        
			//Render mesh
			W_EXP void render();
                        
			//Release all resources
			W_EXP virtual ULONG release() override;

#pragma region Getters

#pragma endregion

#pragma region Setters
		
			
#pragma endregion
			
		private:
                    typedef		system::w_object                        _super;
                       std::shared_ptr<w_graphics_device>                       _gDevice;
		
		};
	}
}

#endif
