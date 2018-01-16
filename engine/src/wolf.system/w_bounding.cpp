#include "w_system_pch.h"
#include "w_bounding.h"
#include <glm/glm.hpp>

using namespace wolf::system;

#pragma region bounding box

w_bounding_box w_bounding_box::create_from_bounding_sphere(_In_ const w_bounding_sphere& pBoundingSphere)
{
	const size_t x = 0;
	const size_t y = 1;
	const size_t z = 2;

	w_bounding_box _box;

	_box.min[x] = pBoundingSphere.center[x] - pBoundingSphere.radius;
	_box.min[y] = pBoundingSphere.center[y] - pBoundingSphere.radius;
	_box.min[z] = pBoundingSphere.center[z] - pBoundingSphere.radius;
	_box.max[x] = pBoundingSphere.center[x] + pBoundingSphere.radius;
	_box.max[y] = pBoundingSphere.center[y] + pBoundingSphere.radius;
	_box.max[z] = pBoundingSphere.center[z] + pBoundingSphere.radius;

	return _box;
}

void w_bounding_box::generate_vertices_indices()
{
    if (vertices.size()) vertices.clear();
    vertices =
    {
        min[0], min[1], min[2],
        min[0], max[1], min[2],
        max[0], max[1], min[2],
        max[0], max[1], min[2],
        max[0], min[1], min[2],
        min[0], min[1], min[2],
        min[0], min[1], max[2],
        max[0], min[1], max[2],
        max[0], max[1], max[2],
        max[0], max[1], max[2],
        min[0], max[1], max[2],
        min[0], min[1], max[2],
        min[0], min[1], min[2],
        max[0], min[1], min[2],
        max[0], min[1], max[2],
        max[0], min[1], max[2],
        min[0], min[1], max[2],
        min[0], min[1], min[2],
        max[0], min[1], min[2],
        max[0], max[1], min[2],
        max[0], max[1], max[2],
        max[0], max[1], max[2],
        max[0], min[1], max[2],
        max[0], min[1], min[2],
        max[0], max[1], min[2],
        min[0], max[1], min[2],
        min[0], max[1], max[2],
        min[0], max[1], max[2],
        max[0], max[1], max[2],
        max[0], max[1], min[2],
        min[0], max[1], min[2],
        min[0], min[1], min[2],
        min[0], min[1], max[2],
        min[0], min[1], max[2],
        min[0], max[1], max[2],
        min[0], max[1], min[2]
    };
}

void w_bounding_box::merge(_In_ _In_ const w_bounding_box& pAdditional)
{
    this->min[0] = std::min(this->min[0], pAdditional.min[0]);
    this->min[1] = std::min(this->min[1], pAdditional.min[1]);
    this->min[2] = std::min(this->min[2], pAdditional.min[2]);

    this->max[0] = std::max(this->max[0], pAdditional.max[0]);
    this->max[1] = std::max(this->max[1], pAdditional.max[1]);
    this->max[2] = std::max(this->max[2], pAdditional.max[2]);
}

bool w_bounding_box::intersects(_In_ const w_bounding_box& pBox)
{
    if ((this->max[0] < pBox.min[0]) || (this->min[0] > pBox.max[0])) return false;
    if ((this->max[1] < pBox.min[1]) || (this->min[1] > pBox.max[1])) return false;

    return ((this->max[2] >= pBox.min[2]) && (this->min[2] <= pBox.max[2]));
}

w_containment_type w_bounding_box::contains(_In_ const glm::vec3& pPoint)
{
    if ((this->min[0] <= pPoint[0]) && (pPoint[0] <= this->max[0]) &&
        (this->min[1] <= pPoint[1]) && (pPoint[1] <= this->max[1]) &&
        (this->min[2] <= pPoint[2]) && (pPoint[2] <= this->max[2]))
    {
        return w_containment_type::CONTAINS;
    }
    return w_containment_type::DISJOINT;
}

w_containment_type w_bounding_box::contains(_In_ const w_bounding_box& pBox)
{
    auto _result = w_containment_type::DISJOINT;
    if ((this->max[0] >= pBox.min[0]) && (this->min[0] <= pBox.max[0]) &&
        (this->max[1] >= pBox.min[1]) && (this->min[1] <= pBox.max[1]) &&
        (this->max[2] >= pBox.min[2]) && (this->min[2] <= pBox.max[2]))
    {
        _result = (
            (this->min[0] <= pBox.min[0]) && (pBox.max[0] <= this->max[0]) &&
            (this->min[2] <= pBox.min[2]) && (pBox.max[2] <= this->max[2]) &&
            (this->min[2] <= pBox.min[2]) && (pBox.max[2] <= this->max[2])) ? w_containment_type::CONTAINS : w_containment_type::INTERSECTS;
    }
    return _result;
}

w_containment_type w_bounding_box::contains(_In_ const w_bounding_sphere& pSphere)
{
    glm::vec3 _box_min(this->min[0], this->min[1], this->min[2]);
    glm::vec3 _box_max(this->max[0], this->max[1], this->max[2]);
    glm::vec3 _sphere_center(pSphere.center[0], pSphere.center[1], pSphere.center[2]);

    auto _clamped = glm::clamp(_sphere_center, _box_min, _box_max);
    auto _dis = _sphere_center - _clamped;
    auto _num = _dis.x * _dis.x + _dis.y * _dis.y + _dis.z * _dis.z;

    if (_num > (pSphere.radius * pSphere.radius))
    {
        return w_containment_type::DISJOINT;
    }
    if (
        ((_box_min.x + pSphere.radius) <= _sphere_center.x) &&
        (_sphere_center.x <= (_box_max.x - pSphere.radius)) &&
        ((_box_max.x - _box_min.x) > pSphere.radius) &&
        ((_box_min.x + pSphere.radius) <= _sphere_center.y) &&
        (_sphere_center.y <= (_box_max.y - pSphere.radius)) &&
        ((_box_max.y - _box_min.y) > pSphere.radius) &&
        ((_box_min.z + pSphere.radius) <= _sphere_center.z) &&
        (_sphere_center.z <= (_box_max.z - pSphere.radius)) &&
        ((_box_max.x - _box_min.x) > pSphere.radius))
    {
        return w_containment_type::CONTAINS;
    }
    return w_containment_type::INTERSECTS;
}

void w_bounding_box::get_corners(_Inout_ std::array<glm::vec3, 8>& pCorners)
{
    pCorners[0].x = this->min[0];
    pCorners[0].y = this->max[1];
    pCorners[0].z = this->max[2];
    pCorners[1].x = this->max[0];
    pCorners[1].y = this->max[1];
    pCorners[1].z = this->max[2];
    pCorners[2].x = this->max[0];
    pCorners[2].y = this->min[1];
    pCorners[2].z = this->max[2];
    pCorners[3].x = this->min[0];
    pCorners[3].y = this->min[1];
    pCorners[3].z = this->max[2];
    pCorners[4].x = this->min[0];
    pCorners[4].y = this->max[1];
    pCorners[4].z = this->min[2];
    pCorners[5].x = this->max[0];
    pCorners[5].y = this->max[1];
    pCorners[5].z = this->min[2];
    pCorners[6].x = this->max[0];
    pCorners[6].y = this->min[1];
    pCorners[6].z = this->min[2];
    pCorners[7].x = this->min[0];
    pCorners[7].y = this->min[1];
    pCorners[7].z = this->min[2];
}

#pragma endregion

#pragma region bounding sphere

w_bounding_sphere w_bounding_sphere::create_from_bounding_box(_In_ const w_bounding_box& pBoundingBox)
{
	w_bounding_sphere _sphere;

	glm::vec3 _in_min(pBoundingBox.min[0], pBoundingBox.min[1], pBoundingBox.min[2]);
	glm::vec3 _in_max(pBoundingBox.max[0], pBoundingBox.max[1], pBoundingBox.max[2]);

	//apply lerp to calculate center
	const float _lerp_amount = 0.5f;
	_sphere.center[0] = _in_min.x + ((_in_max.x - _in_min.x) * _lerp_amount);
	_sphere.center[1] = _in_min.y + ((_in_max.y - _in_min.y) * _lerp_amount);
	_sphere.center[2] = _in_min.z + ((_in_max.z - _in_min.z) * _lerp_amount);

	auto _num = glm::distance(_in_min, _in_max);
	_sphere.radius = _num * 0.5f;

	return _sphere;
}

void w_bounding_sphere::merge(_In_ const w_bounding_sphere& pAdditional)
{
    glm::vec3 _addi_center = glm::vec3(
        pAdditional.center[0],
        pAdditional.center[1],
        pAdditional.center[2]);

    glm::vec3 _orig_center = glm::vec3(
        this->center[0],
        this->center[1],
        this->center[2]);

    auto _sub_vec = _addi_center - _orig_center;
    float _sub_len = _sub_vec.length();

    if ((this->radius + pAdditional.radius) >= _sub_len)
    {
        if (this->radius - pAdditional.radius >= _sub_len) return;
        if ((pAdditional.radius - this->radius) >= _sub_len)
        {
            std::memcpy(&this->center[0], &pAdditional.center[0], 3 * sizeof(float));
            this->radius = pAdditional.radius;
        }
    }
    auto _vector = _sub_vec * (1.0f / _sub_len);
    float _min = std::min(-this->radius, _sub_len - pAdditional.radius);
    float _max = (std::max(this->radius, _sub_len + pAdditional.radius) - _min) * 0.5f;

    this->radius = _max;
    auto _center = _orig_center + (_vector * (_max + _min));
    this->center[0] = _center.x;
    this->center[1] = _center.y;
    this->center[2] = _center.z;
}

bool w_bounding_sphere::intersects(_In_ const w_bounding_sphere& pSphere)
{
    auto _dis_x = this->center[0] - pSphere.center[0];
    auto _dis_y = this->center[1] - pSphere.center[1];
    auto _dis_z = this->center[2] - pSphere.center[2];

    auto _num = _dis_x * _dis_x + _dis_y * _dis_y + _dis_z * _dis_z;
    if ((this->radius * this->radius + 2.0f * this->radius * pSphere.radius + pSphere.radius * pSphere.radius) <= _num)
    {
        return false;
    }
    return true;
}

bool w_bounding_sphere::intersects(_In_ const w_bounding_box& pBox)
{
    glm::vec3 _sphere_center(this->center[0], this->center[1], this->center[2]);
    glm::vec3 _box_min(pBox.min[0], pBox.min[1], pBox.min[2]);
    glm::vec3 _box_max(pBox.max[0], pBox.max[1], pBox.max[2]);

    auto _clamped = glm::clamp(_sphere_center, _box_min, _box_max);
    auto _dis = _sphere_center - _clamped;

    auto _num = _dis.x * _dis.x + _dis.y * _dis.y + _dis.z * _dis.z;
    return _num <= this->radius * this->radius;
}

#pragma endregion

#pragma region bounding frustum

std::array<glm::vec4, 6> w_bounding_frustum::get_plans() const
{
	const size_t x = 0;
	const size_t y = 1;
	const size_t z = 2;
	const size_t w = 3;

	std::array<glm::vec4, 6> _array = 
	{
		glm::vec4(this->_planes[LEFT][x], this->_planes[LEFT][y], this->_planes[LEFT][z], this->_planes[LEFT][w]),
		glm::vec4(this->_planes[RIGHT][x], this->_planes[RIGHT][y], this->_planes[RIGHT][z], this->_planes[RIGHT][w]),
		glm::vec4(this->_planes[TOP][x], this->_planes[TOP][y], this->_planes[TOP][z], this->_planes[TOP][w]),
		glm::vec4(this->_planes[BOTTOM][x], this->_planes[BOTTOM][y], this->_planes[BOTTOM][z], this->_planes[BOTTOM][w]),
		glm::vec4(this->_planes[BACK][x], this->_planes[BACK][y], this->_planes[BACK][z], this->_planes[BACK][w]),
		glm::vec4(this->_planes[FRONT][x], this->_planes[FRONT][y], this->_planes[FRONT][z], this->_planes[FRONT][w])
	};

	return _array;
}

void w_bounding_frustum::update(_In_ const glm::mat4& pMatrix)
{
	const size_t x = 0;
	const size_t y = 1;
	const size_t z = 2;
	const size_t w = 3;

	this->_planes[LEFT][x] = pMatrix[0].w + pMatrix[0].x;
	this->_planes[LEFT][y] = pMatrix[1].w + pMatrix[1].x;
	this->_planes[LEFT][z] = pMatrix[2].w + pMatrix[2].x;
	this->_planes[LEFT][w] = pMatrix[3].w + pMatrix[3].x;

	this->_planes[RIGHT][x] = pMatrix[0].w - pMatrix[0].x;
	this->_planes[RIGHT][y] = pMatrix[1].w - pMatrix[1].x;
	this->_planes[RIGHT][z] = pMatrix[2].w - pMatrix[2].x;
	this->_planes[RIGHT][w] = pMatrix[3].w - pMatrix[3].x;

	this->_planes[TOP][x] = pMatrix[0].w + pMatrix[0].y;
	this->_planes[TOP][y] = pMatrix[1].w + pMatrix[1].y;
	this->_planes[TOP][z] = pMatrix[2].w + pMatrix[2].y;
	this->_planes[TOP][w] = pMatrix[3].w + pMatrix[3].y;

	this->_planes[BOTTOM][x] = pMatrix[0].w - pMatrix[0].y;
	this->_planes[BOTTOM][y] = pMatrix[1].w - pMatrix[1].y;
	this->_planes[BOTTOM][z] = pMatrix[2].w - pMatrix[2].y;
	this->_planes[BOTTOM][w] = pMatrix[3].w - pMatrix[3].y;

	this->_planes[BACK][x] = pMatrix[0].w - pMatrix[0].z;
	this->_planes[BACK][y] = pMatrix[1].w - pMatrix[1].z;
	this->_planes[BACK][z] = pMatrix[2].w - pMatrix[2].z;
	this->_planes[BACK][w] = pMatrix[3].w - pMatrix[3].z;

	this->_planes[FRONT][x] = pMatrix[0].w + pMatrix[0].z;
	this->_planes[FRONT][y] = pMatrix[1].w + pMatrix[1].z;
	this->_planes[FRONT][z] = pMatrix[2].w + pMatrix[2].z;
	this->_planes[FRONT][w] = pMatrix[3].w + pMatrix[3].z;

	for (size_t i = 0; i < 6; ++i)
	{
		auto _norm = glm::normalize(
			glm::vec4(
				this->_planes[i][x],
				this->_planes[i][y],
				this->_planes[i][z],
				this->_planes[i][w]));

		this->_planes[i][x] = _norm.x;
		this->_planes[i][y] = _norm.y;
		this->_planes[i][z] = _norm.z;
		this->_planes[i][w] = _norm.w;
	}
}

bool w_bounding_frustum::intersects(_In_ const w_bounding_sphere& pSphere)
{
	const size_t x = 0;
	const size_t y = 1;
	const size_t z = 2;
	const size_t w = 3;

	for (auto i = 0; i < 6; ++i)
	{
		if ((_planes[i][x] * pSphere.center[x]) +
			(_planes[i][y] * pSphere.center[y]) +
			(_planes[i][z] * pSphere.center[z]) + _planes[i][w] <= -pSphere.radius)
		{
			return false;
		}
	}
	return true;
}

bool w_bounding_frustum::intersects(_In_ const w_bounding_box& pBox)
{
	return intersects(w_bounding_sphere::create_from_bounding_box(pBox));
}

#pragma endregion
