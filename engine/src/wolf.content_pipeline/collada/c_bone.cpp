#include "w_cpipeline_pch.h"
#include "c_bone.h"

using namespace wolf::content_pipeline::collada;

c_bone::c_bone()
{
	//Make sure world matrix is identity
}

c_bone::c_bone(c_bone* pParentBone, glm::mat4x4 pInitialMatrix, const std::string& pSID, const std::string& pName) :
	parent(pParentBone),
	initial_matrix(pInitialMatrix)
{
	this->position = glm::vec4_from_mat4x4_row(initial_matrix, 3);
	this->c_sid = pSID;
	this->c_name = pName;
}

c_bone::~c_bone()
{

}

void c_bone::find_child_with_sid(const std::string& pSID, _Inout_ c_bone* pBone)
{
	if (this->c_sid == pSID)
	{
		pBone = this;
		return;
	}
	if (this->children.size())
	{
		std::for_each(this->children.begin(), this->children.end(), [pSID, pBone](_In_ c_bone* pChild)
		{
			pChild->find_child_with_sid(pSID, pBone);
			if (pBone)
			{
				return;
			}
		});
	}

	pBone = nullptr;
}

void c_bone::find_child_with_name(const std::string& pName, _Inout_ c_bone* pBone)
{
	if (this->c_name == pName)
	{
		pBone = this;
		return;
	}

	if (this->children.size())
	{
		std::for_each(this->children.begin(), this->children.end(), [pName, pBone](_In_ c_bone* pChild)
		{
			pChild->find_child_with_name(pName, pBone);
			if (pBone)
			{
				return;
			}
		});
	}

	pBone = nullptr;
}

glm::mat4x4 c_bone::calaculate_matrices(float pTime, _Out_ bool pHasAnimation)
{
	pHasAnimation = true;
	auto _rotation = this->rotation;
	auto _transform = glm::transform_from_mat4x4(initial_matrix);

	if (this->animations.size() > 0)
	{
		std::for_each(this->animations.begin(), this->animations.end(), [&](c_animation pAnim)
		{
			auto _key = pAnim.get_key(pTime, pHasAnimation);
			//if (!hasAnim) continue;
			switch (pAnim.dimension_type)
			{
			case DIMENTION_TYPE::X:
				switch (pAnim.transform_type)
				{
				case TRANSFORM_TYPE::TRANSLATION:
					_transform.x = _key;
					break;
				case TRANSFORM_TYPE::ROTATION:
					_rotation.x = _key;
					break;
				}
				break;
			case DIMENTION_TYPE::Y:
				switch (pAnim.transform_type)
				{
				case TRANSFORM_TYPE::TRANSLATION:
					_transform.y = _key;
					break;
				case TRANSFORM_TYPE::ROTATION:
					_rotation.y = _key;
					break;
				}
				break;
			case DIMENTION_TYPE::Z:
				if (pAnim.transform_type == TRANSFORM_TYPE::TRANSLATION)
				{
					_transform.z = _key;
				}
				else if (pAnim.transform_type == TRANSFORM_TYPE::ROTATION)
				{
					_rotation.z = _key;
				}
				break;
			}
		});
	}
	this->animated_rotation = _rotation;
	this->animated_position = _transform;

	auto _radian_rotation_vec3 = glm::vec3(glm::radians(_rotation.x), glm::radians(_rotation.y), glm::radians(_rotation.z));
	return glm::make_wpv_mat(this->scale, _radian_rotation_vec3, _transform);
}

glm::mat4x4 c_bone::calaculate_blended_matrices(float pTime, c_bone* pBone, float pLinearAmout, _Out_ bool pHasAnimation)
{
	pHasAnimation = true;
	auto _rotation = this->rotation;
	auto _transform = glm::transform_from_mat4x4(initial_matrix);

	if (this->animations.size() > 0)
	{
		std::for_each(this->animations.begin(), this->animations.end(), [&](c_animation pAnim)
		{
			auto _key = pAnim.get_key(pTime, pHasAnimation);

			switch (pAnim.dimension_type)
			{
			case DIMENTION_TYPE::X:
				if (pAnim.transform_type == TRANSFORM_TYPE::TRANSLATION)
				{
					_transform.x = _key;
				}
				else if (pAnim.transform_type == TRANSFORM_TYPE::ROTATION)
				{
					_rotation.x = _key;
				}
				break;
			case DIMENTION_TYPE::Y:
				if (pAnim.transform_type == TRANSFORM_TYPE::TRANSLATION)
				{
					_transform.y = _key;
				}
				else if (pAnim.transform_type == TRANSFORM_TYPE::ROTATION)
				{
					_rotation.y = _key;
				}
				break;
			case DIMENTION_TYPE::Z:
				if (pAnim.transform_type == TRANSFORM_TYPE::TRANSLATION)
				{
					_transform.z = _key;
				}
				else if (pAnim.transform_type == TRANSFORM_TYPE::ROTATION)
				{
					_rotation.z = _key;
				}
				break;
			}
		});
	}

	auto _lerp_rotation = glm::lerp(_rotation, pBone->animated_rotation, pLinearAmout);
	auto _lerp_transform = glm::lerp(_transform, pBone->animated_position, pLinearAmout);

	auto _radian_rotation_vec3 = glm::vec3(glm::radians(_lerp_rotation.x), glm::radians(_lerp_rotation.y), glm::radians(_lerp_rotation.z));
	return glm::make_wpv_mat(this->scale, _radian_rotation_vec3, _lerp_transform);
}

ULONG c_bone::release()
{
	if (this->get_is_released()) return 0;
	
	//release children
	std::for_each(children.begin(), children.end(), [](c_bone* pValue)
	{
		SAFE_RELEASE(pValue);
	});
	children.clear();

	//release flat bones
	std::for_each(flat_bones.begin(), flat_bones.end(), [](c_bone* pValue)
	{
		SAFE_RELEASE(pValue);
	});
	flat_bones.clear();

	SAFE_RELEASE(parent);
	
	//release animations
	std::for_each(animations.begin(), animations.end(), [](c_animation& pValue)
	{
		pValue.release();
	});
	animations.clear();

	return _super::release();
}

#pragma region Getters

void c_bone::get_flat(_Inout_ c_bone* pBone)
{
	auto _animation_size = this->animations.size();
	std::vector<c_animation> _anims;

	if (_animation_size > 0)
	{
		auto _size_of_c_animation = sizeof(c_animation);
		for (size_t i = 0; i < _animation_size; ++i)
		{
			c_animation _anim;

			//deep copy
			std::memcpy(&_anim, &(this->animations.at(i)), _size_of_c_animation);

			_anims.push_back(_anim);
		}
	}
	if (pBone)
	{
		pBone->release();
	}
	pBone = new c_bone();
	pBone->animations = _anims;
	pBone->c_id = this->c_id;
	pBone->c_sid = this->c_sid + "- cloned";
	pBone->c_name = this->c_name;
	pBone->initial_matrix = this->initial_matrix;
	pBone->world_transform = this->world_transform;
	pBone->inverse_bond_matrix = this->inverse_bond_matrix;
	pBone->position = this->position;
	pBone->scale = this->scale;
	pBone->rotation = this->rotation;
	pBone->index = this->index;
	pBone->rotation_matrix = this->rotation_matrix;
}

#pragma endregion

#pragma region Setters

void c_bone::set_inverse_bind_poses(std::vector<glm::mat4x4>& pMats, std::vector<std::string>& pBoneNames)
{
	for (size_t i = 0; i < pBoneNames.size(); ++i)
	{
		auto _bone_name = pBoneNames[i];
		
		c_bone* _child_bone = nullptr;
		find_child_with_name(_bone_name, _child_bone);
		if (_child_bone == nullptr)
		{
			//try again with sid
			find_child_with_sid(_bone_name, _child_bone);
		}

		if (_child_bone == nullptr)
		{
			continue;
		}

		_child_bone->inverse_bond_matrix = glm::transpose(pMats[i]);
	}
}

#pragma endregion