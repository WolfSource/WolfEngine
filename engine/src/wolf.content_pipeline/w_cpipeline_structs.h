/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_cpipeline_structs.h
	Description		 : The structures
	Comment          :
*/

#ifndef __W_CPIPELINE_STRUCTS_H__
#define __W_CPIPELINE_STRUCTS_H__

#include <msgpack.hpp>

namespace wolf
{
    namespace content_pipeline
    {
        struct w_vertex_data
        {
            float		    position[3];
            float		    normal[3];
            float		    uv[2];
            float		    blend_weight[4];
            int	            blend_indices[4];
            float		    tangent[3];
            float		    binormal[3];
            float		    color[4];
            uint32_t	        vertex_index;

            MSGPACK_DEFINE(position, normal, uv, vertex_index);
        };
    }
}

#endif
