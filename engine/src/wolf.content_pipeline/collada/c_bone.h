/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_bone.h
	Description		 : The class of collada's bone
	Comment          :
*/

#ifndef __C_BONE_H__
#define __C_BONE_H__

#include <vector>
#include "c_obj.h"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "c_animation.h"

namespace wolf
{
	namespace content_pipeline
	{
		namespace collada
		{
			struct c_weight_bone
			{
				int bone_index;
				float weight;
			};

			class c_bone : public c_obj
			{
			public:
				c_bone();
				c_bone(c_bone* pParentBone, glm::mat4x4 pInitialMatrix, const std::string& pSID, const std::string& pName);
				virtual ~c_bone();

				void		find_child_with_sid(const std::string& pSID, _Inout_ c_bone* pBone);
				void		find_child_with_name(const std::string& pName, _Inout_ c_bone* pBone);
				glm::mat4x4 calaculate_matrices(float pTime, _Out_ bool pHasAnimation);
				glm::mat4x4 calaculate_blended_matrices(float pTime, c_bone* pBone, float pLinearAmout, _Out_ bool pHasAnimation);
			
				ULONG release();

#pragma region Getters

				void get_flat(_Inout_ c_bone* pBone);

#pragma endregion

#pragma region Setters

				void set_inverse_bind_poses(std::vector<glm::mat4x4>& pMats, std::vector<std::string>& pBoneNames);

#pragma endregion

				std::vector<c_bone*>		children;
				std::vector<c_bone*>		flat_bones;
				c_bone*						parent;
				glm::mat4x4					initial_matrix;
				glm::vec4					position;
				glm::vec3					rotation;
				glm::vec3					scale;
				glm::vec3					animated_rotation;
				glm::vec3					animated_position;
				glm::mat4x4					animated_matrix;
				glm::mat4x4					inverse_bond_matrix;
				glm::mat4x4					rotation_matrix;
				glm::mat4x4					world_transform;
				int							index;
				std::vector<c_animation>	animations;

			private:
				bool						_is_released;
			};
		}
	}
}

#endif
