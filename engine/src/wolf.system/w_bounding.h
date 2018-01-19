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

#include "w_system_export.h"
#include <vector>
#include <array>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <msgpack.hpp>

namespace wolf
{
	namespace system
	{
        enum w_containment_type { DISJOINT, INTERSECTS, CONTAINS };

        //forward declaration
        struct w_bounding_sphere;

        struct w_bounding_box
        {
			float           min[3] = { 0,0,0 };
            float           max[3] = { 0,0,0 };
            
			float	        position[3] = { 0,0,0 };
            float	        rotation[3] = { 0,0,0 };;
            
            //used for rendering and masked occulusion culling
            std::vector<float>      vertices;

			WSYS_EXP static w_bounding_box create_from_bounding_sphere(_In_ const w_bounding_sphere& pBoundingSphere);

			WSYS_EXP void generate_vertices_indices();
			WSYS_EXP void merge(_In_ _In_ const w_bounding_box& pAdditional);
			WSYS_EXP bool intersects(_In_ const w_bounding_box& pBox);
			WSYS_EXP w_containment_type contains(_In_ const glm::vec3& pPoint);
			WSYS_EXP w_containment_type contains(_In_ const w_bounding_box& pBox);
			WSYS_EXP w_containment_type contains(_In_ const w_bounding_sphere& pSphere);
			WSYS_EXP void get_corners(_Inout_ std::array<glm::vec3, 8>& pCorners);

            MSGPACK_DEFINE(min, max, position, rotation);
        };

        struct w_bounding_sphere
        {
            float center[3];
            float radius;

			WSYS_EXP static w_bounding_sphere create_from_bounding_box(_In_ const w_bounding_box& pBoundingBox);

			WSYS_EXP void merge(_In_ const w_bounding_sphere& pAdditional);
			WSYS_EXP bool intersects(_In_ const w_bounding_sphere& pSphere);
			WSYS_EXP bool intersects(_In_ const w_bounding_box& pBox);

            MSGPACK_DEFINE(center, radius);
        };

		struct w_bounding_frustum
		{
			WSYS_EXP std::array<glm::vec4, 6> get_plans() const;
			WSYS_EXP void update(_In_ const glm::mat4& pMatrix);
			WSYS_EXP bool intersects(_In_ const w_bounding_sphere& pSphere);
			WSYS_EXP bool intersects(_In_ const w_bounding_box& pBox);

			MSGPACK_DEFINE(_planes);

		private:
			enum w_frustum_side { LEFT = 0, RIGHT = 1, TOP = 2, BOTTOM = 3, BACK = 4, FRONT = 5 };
			float _planes[6][4];//6 plans with 4 floats(w,x,y,z)
		};
	}
}

#endif
