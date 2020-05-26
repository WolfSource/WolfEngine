/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_bounding.h
	Description		 : The bounding class
	Comment          :
*/

#pragma once

#include "w_system_export.h"
#include <array>
#include "glm_extension.h"

#if __cplusplus <= 201402L
#include "msgpack.hpp"
#endif

#include "python_exporter/w_boost_python_helper.h"

namespace wolf::system
{
	enum w_containment_type { DISJOINT, INTERSECTS, CONTAINS };

	//forward declaration
	struct w_bounding_sphere;

	struct w_bounding_box
	{
		float           min[3] = { 0 };
		float           max[3] = { 0 };

		float	        position[3] = { 0 };
		float	        rotation[3] = { 0 };

		//used for rendering to masked occulusion culling buffer
		float			vertices[108] = { 0 };

		//create bounding box from bounding sphere
		WSYS_EXP static w_bounding_box create_from_bounding_sphere(_In_ const w_bounding_sphere& pBoundingSphere);

		WSYS_EXP void generate_vertices();
		WSYS_EXP void merge(_In_ const w_bounding_box& pAdditional);
		WSYS_EXP bool intersects(_In_ const w_bounding_box& pBox);
		WSYS_EXP w_containment_type contains(_In_ const glm::vec3& pPoint);
		WSYS_EXP w_containment_type contains(_In_ const w_bounding_box& pBox);
		WSYS_EXP w_containment_type contains(_In_ const w_bounding_sphere& pSphere);
		WSYS_EXP void get_corners(_Inout_ std::array<glm::vec3, 8> & pCorners);
		WSYS_EXP glm::vec3 get_center() const;
        
#if __cplusplus <= 201402L
		MSGPACK_DEFINE(min, max, position, rotation);
#endif
        
#ifdef __PYTHON__

		//min
		glm::w_vec3 py_get_min() { return glm::w_vec3(this->min[0], this->min[1], this->min[2]); }
		void py_set_min(_In_ glm::w_vec3 pValue) { this->min[0] = pValue.get_x(); this->min[1] = pValue.get_y(); this->min[2] = pValue.get_z(); }

		//max
		glm::w_vec3 py_get_max() { return glm::w_vec3(this->max[0], this->max[1], this->max[2]); }
		void py_set_max(_In_ glm::w_vec3 pValue) { this->max[0] = pValue.get_x(); this->max[1] = pValue.get_y(); this->max[2] = pValue.get_z(); }

		//position
		glm::w_vec3 py_get_position() { return glm::w_vec3(this->position[0], this->position[1], this->position[2]); }
		void py_set_position(_In_ glm::w_vec3 pValue) { this->position[0] = pValue.get_x(); this->position[1] = pValue.get_y(); this->position[2] = pValue.get_z(); }

		//rotation
		glm::w_vec3 py_get_rotation() { return glm::w_vec3(this->rotation[0], this->rotation[1], this->rotation[2]); }
		void py_set_rotation(_In_ glm::w_vec3 pValue) { this->rotation[0] = pValue.get_x(); this->rotation[1] = pValue.get_y(); this->rotation[2] = pValue.get_z(); }

		//vertices
		boost::python::list py_get_vertices() { return boost_wrap_array(this->vertices, 108); }

		//corners
		boost::python::list py_get_corners()
		{
			boost::python::list _list;
			//get corners
			std::array<glm::vec3, 8> _corners;
			get_corners(_corners);
			//append to list
			for (auto _c : _corners)
			{
				_list.append(boost::python::make_tuple(_c.x, _c.y, _c.z));
			}
			return _list;
		}
		w_containment_type py_contains_point(_In_ boost::python::tuple pPoint)
		{
			boost::python::extract<float> _x(pPoint[0]);
			boost::python::extract<float> _y(pPoint[1]);
			boost::python::extract<float> _z(pPoint[2]);
			return contains(glm::vec3(
				_x.check() ? _x() : 0.0f,
				_y.check() ? _y() : 0.0f,
				_z.check() ? _z() : 0.0f));
		}
		w_containment_type py_contains_bounding_box(_In_ const w_bounding_box& pBox) { return contains(pBox); }
#endif
	};

	struct w_bounding_sphere
	{
		float center[3];
		float radius;

		WSYS_EXP static w_bounding_sphere create_from_bounding_box(_In_ const w_bounding_box& pBoundingBox);

		WSYS_EXP void merge(_In_ const w_bounding_sphere& pAdditional);
		WSYS_EXP bool intersects(_In_ const w_bounding_sphere& pSphere);
		WSYS_EXP bool intersects(_In_ const w_bounding_box& pBox);
		WSYS_EXP w_containment_type contains(_In_ const glm::vec3& pPoit);

#if __cplusplus <= 201402L
		MSGPACK_DEFINE(center, radius);
#endif
        
#ifdef __PYTHON__
		//center
		glm::w_vec3 py_get_center() { return glm::w_vec3(this->center[0], this->center[1], this->center[2]); }
		void py_set_center(_In_ glm::w_vec3 pValue) { this->center[0] = pValue.get_x(); this->center[1] = pValue.get_y(); this->center[2] = pValue.get_z(); }

		//radius
		float py_get_radius() { return this->radius; }
		void py_set_radius(_In_ float pValue) { this->radius = pValue; }

		bool intersects_bounding_sphere(_In_ const w_bounding_sphere& pSphere) { return intersects(pSphere); }
#endif
	};

	struct w_bounding_frustum
	{
		WSYS_EXP std::array<glm::vec4, 6> get_plans() const;
		WSYS_EXP void update(_In_ const glm::mat4& pMatrix);
		WSYS_EXP bool intersects(_In_ const glm::vec3& pPoint);
		WSYS_EXP bool intersects(_In_ const w_bounding_sphere& pSphere);
		WSYS_EXP bool intersects(_In_ const w_bounding_box& pBox);

#if __cplusplus <= 201402L
		MSGPACK_DEFINE(_planes);
#endif
        
#ifdef __PYTHON__
		boost::python::list py_get_plans()
		{
			using namespace boost::python;
			list _list;
			for (size_t i = 0; i < 6; ++i)
			{
				auto _t = make_tuple(_planes[i][0], _planes[i][1], _planes[i][2], _planes[i][3]);
				_list.append(_t);
			}
			return _list;
		}
		void py_update(boost::python::tuple pMatrix)
		{
			glm::mat4 _mat4;
			boost_extract_tuple(
				_mat4[0][0], _mat4[0][1], _mat4[0][2], _mat4[0][3],
				_mat4[1][0], _mat4[1][1], _mat4[1][2], _mat4[1][3],
				_mat4[2][0], _mat4[2][1], _mat4[2][2], _mat4[2][3],
				_mat4[3][0], _mat4[3][1], _mat4[3][2], _mat4[3][3],
				pMatrix);

			update(_mat4);
		}

		bool py_intersects_bounding_sphere(_In_ const w_bounding_sphere& pSphere) { return intersects(pSphere); }
		bool py_intersects_bounding_box(_In_ const w_bounding_box& pBox) { return intersects(pBox); }
#endif

	private:
		float _planes[6][4];//6 plans with 4 floats(w,x,y,z)
	};
}

#include "python_exporter/py_bounding.h"

