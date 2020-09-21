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

void w_bounding_box::generate_vertices()
{
	vertices[0] = min[0]; vertices[1] = min[1]; vertices[2] = min[2];
	vertices[3] = min[0]; vertices[4] = max[1]; vertices[5] = min[2];
	vertices[6] = max[0]; vertices[7] = max[1]; vertices[8] = min[2];

	vertices[9] = max[0]; vertices[10] = max[1]; vertices[11] = min[2];
	vertices[12] = max[0]; vertices[13] = min[1]; vertices[14] = min[2];
	vertices[15] = min[0]; vertices[16] = min[1]; vertices[17] = min[2];
	
	vertices[18] = min[0]; vertices[19] = min[1]; vertices[20] = max[2];
	vertices[21] = max[0]; vertices[22] = min[1]; vertices[23] = max[2];
	vertices[24] = max[0]; vertices[25] = max[1]; vertices[26] = max[2];
	
	vertices[27] = max[0]; vertices[28] = max[1]; vertices[29] = max[2];
	vertices[30] = min[0]; vertices[31] = max[1]; vertices[32] = max[2];
	vertices[33] = min[0]; vertices[34] = min[1]; vertices[35] = max[2];

	vertices[36] = min[0]; vertices[37] = min[1]; vertices[38] = min[2];
	vertices[39] = max[0]; vertices[40] = min[1]; vertices[41] = min[2];
	vertices[42] = max[0]; vertices[43] = min[1]; vertices[44] = max[2];
	
	vertices[45] = max[0]; vertices[46] = min[1]; vertices[47] = max[2];
	vertices[48] = min[0]; vertices[49] = min[1]; vertices[50] = max[2];
	vertices[51] = min[0]; vertices[52] = min[1]; vertices[53] = min[2];
	
	vertices[54] = max[0]; vertices[55] = min[1]; vertices[56] = min[2];
	vertices[57] = max[0]; vertices[58] = max[1]; vertices[59] = min[2];
	vertices[60] = max[0]; vertices[61] = max[1]; vertices[62] = max[2];

	vertices[63] = max[0]; vertices[64] = max[1]; vertices[65] = max[2];
	vertices[66] = max[0]; vertices[67] = min[1]; vertices[68] = max[2];
	vertices[69] = max[0]; vertices[70] = min[1]; vertices[71] = min[2];

	vertices[72] = max[0]; vertices[73] = max[1]; vertices[74] = min[2];
	vertices[75] = min[0]; vertices[76] = max[1]; vertices[77] = min[2];
	vertices[78] = min[0]; vertices[79] = max[1]; vertices[80] = max[2];

	vertices[81] = min[0]; vertices[82] = max[1]; vertices[83] = max[2];
	vertices[84] = max[0]; vertices[85] = max[1]; vertices[86] = max[2];
	vertices[87] = max[0]; vertices[88] = max[1]; vertices[89] = min[2];

	vertices[90] = min[0]; vertices[91] = max[1]; vertices[92] = min[2];
	vertices[93] = min[0]; vertices[94] = min[1]; vertices[95] = min[2];
	vertices[96] = min[0]; vertices[97] = min[1]; vertices[98] = max[2];
	
	vertices[99] = min[0]; vertices[100] = min[1]; vertices[101] = max[2];
	vertices[102] = min[0]; vertices[103] = max[1]; vertices[104] = max[2];
	vertices[105] = min[0]; vertices[106] = max[1]; vertices[107] = min[2];
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

glm::vec3 w_bounding_box::get_center() const
{
	//check with center
	glm::vec3 _center;
	glm::vec3 _min(this->min[0], this->min[1], this->min[2]);
	glm::vec3 _max(this->max[0], this->max[1], this->max[2]);

	//apply lerp to calculate center
	const float _lerp_amount = 0.5f;
	_center.x = _min.x + ((_max.x - _min.x) * _lerp_amount);
	_center.y = _min.y + ((_max.y - _min.y) * _lerp_amount);
	_center.z = _min.z + ((_max.z - _min.z) * _lerp_amount);

	return _center;
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

w_containment_type w_bounding_sphere::contains(_In_ const glm::vec3& pPoint)
{
	float _num3 = pPoint[0] - this->center[0];
	float _num2 = pPoint[1] - this->center[1];
	float _num1 = pPoint[2] - this->center[2];

	auto _distance_squared = _num3 * _num3 + _num2 * _num2 + _num1 * _num1;
	if (_distance_squared >= (this->radius* this->radius))
	{
		return w_containment_type::DISJOINT;
	}
	return w_containment_type::CONTAINS;
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
		glm::vec4(this->_planes[0][x], this->_planes[0][y], this->_planes[0][z], this->_planes[0][w]),
		glm::vec4(this->_planes[1][x], this->_planes[1][y], this->_planes[1][z], this->_planes[1][w]),
		glm::vec4(this->_planes[2][x], this->_planes[2][y], this->_planes[2][z], this->_planes[2][w]),
		glm::vec4(this->_planes[3][x], this->_planes[3][y], this->_planes[3][z], this->_planes[3][w]),
		glm::vec4(this->_planes[4][x], this->_planes[4][y], this->_planes[4][z], this->_planes[4][w]),
		glm::vec4(this->_planes[5][x], this->_planes[5][y], this->_planes[5][z], this->_planes[5][w])
	};

	return _array;
}

void w_bounding_frustum::update(_In_ const glm::mat4& pMatrix)
{
	const size_t x = 0;
	const size_t y = 1;
	const size_t z = 2;
	const size_t w = 3;

	this->_planes[0][x] = pMatrix[x][w] + pMatrix[x][z];
	this->_planes[0][y] = pMatrix[y][w] + pMatrix[y][z];
	this->_planes[0][z] = pMatrix[z][w] + pMatrix[z][z];
	this->_planes[0][w] = pMatrix[w][w] + pMatrix[w][z];

	this->_planes[1][x] = pMatrix[x][w] - pMatrix[x][z];
	this->_planes[1][y] = pMatrix[y][w] - pMatrix[y][z];
	this->_planes[1][z] = pMatrix[z][w] - pMatrix[z][z];
	this->_planes[1][w] = pMatrix[w][w] - pMatrix[w][z];

	this->_planes[2][x] = pMatrix[x][w] + pMatrix[x][x];
	this->_planes[2][y] = pMatrix[y][w] + pMatrix[y][x];
	this->_planes[2][z] = pMatrix[z][w] + pMatrix[z][x];
	this->_planes[2][w] = pMatrix[w][w] + pMatrix[w][x];

	this->_planes[3][x] = pMatrix[x][w] - pMatrix[x][x];
	this->_planes[3][y] = pMatrix[y][w] - pMatrix[y][x];
	this->_planes[3][z] = pMatrix[z][w] - pMatrix[z][x];
	this->_planes[3][w] = pMatrix[w][w] - pMatrix[w][x];

	this->_planes[4][x] = pMatrix[x][w] - pMatrix[x][y];
	this->_planes[4][y] = pMatrix[y][w] - pMatrix[y][y];
	this->_planes[4][z] = pMatrix[z][w] - pMatrix[z][y];
	this->_planes[4][w] = pMatrix[w][w] - pMatrix[w][y];

	this->_planes[5][x] = pMatrix[x][w] + pMatrix[x][y];
	this->_planes[5][y] = pMatrix[y][w] + pMatrix[y][y];
	this->_planes[5][z] = pMatrix[z][w] + pMatrix[z][y];
	this->_planes[5][w] = pMatrix[w][w] + pMatrix[w][y];
	
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

bool w_bounding_frustum::intersects(_In_ const glm::vec3& pPoint)
{
	glm::vec4 _plane, _point(pPoint, 1.0f);
	for (size_t i = 0; i < 6; ++i)
	{
		_plane.x = this->_planes[i][0];
		_plane.y = this->_planes[i][1];
		_plane.z = this->_planes[i][2];
		_plane.w = this->_planes[i][3];

		if (glm::dot(_plane, _point) < 0.0f)
		{
			return false;
		}
	}

	return true;
}

bool w_bounding_frustum::intersects(_In_ const w_bounding_box& pBoundingBox)
{
	using namespace glm;

	auto _b = pBoundingBox;

	std::array<glm::vec3, 8> _corners;
	_b.get_corners(_corners);

	auto _center = pBoundingBox.get_center();
	if (intersects(_center)) return true;

	//check with corners
	for (size_t j = 0; j < _corners.size(); ++j)
	{
		if (intersects(_corners[j]))
		{
			return true;
		}
	}
	
	return false;
}
bool w_bounding_frustum::intersects(_In_ const w_bounding_sphere& pBoundingSphere)
{
	return intersects(w_bounding_box::create_from_bounding_sphere(pBoundingSphere));
}

#pragma endregion
