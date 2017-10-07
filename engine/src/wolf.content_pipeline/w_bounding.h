/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_bounding.h
	Description		 : The bounding class
	Comment          :
*/

#ifndef __W_BOUNDING_H__
#define __W_BOUNDING_H__

#include "w_cpipeline_export.h"
#include <vector>
#include <glm/vec3.hpp>
#include <msgpack.hpp>

namespace wolf
{
	namespace content_pipeline
	{
        enum w_containment_type { DISJOINT, INTERSECTS, CONTAINS };

        //forward declaration
        struct w_bounding_sphere;

        struct w_bounding_box
        {
            float           min[3];
            float           max[3];
            
            float	        position[3];
            float	        rotation[3];
            
            //used for rendering and masked occulusion culling
            std::vector<float>      vertices;

            WCP_EXP void generate_vertices_indices();
            WCP_EXP void merge(_In_ _In_ const w_bounding_box& pAdditional);
            WCP_EXP bool intersects(_In_ const w_bounding_box& pBox);
            WCP_EXP w_containment_type contains(_In_ const glm::vec3& pPoint);
            WCP_EXP w_containment_type contains(_In_ const w_bounding_box& pBox);
            WCP_EXP w_containment_type contains(_In_ const w_bounding_sphere& pSphere);

            MSGPACK_DEFINE(min, max, position, rotation);
        };

        struct w_bounding_sphere
        {
            float center[3];
            float radius;

            WCP_EXP void merge(_In_ const w_bounding_sphere& pAdditional);
            WCP_EXP void create_from_bounding_box(_In_ const w_bounding_box& pBox);
            WCP_EXP bool intersects(_In_ const w_bounding_sphere& pSphere);
            WCP_EXP bool intersects(_In_ const w_bounding_box& pBox);

            MSGPACK_DEFINE(center, radius);
        };
	}
}

#endif
