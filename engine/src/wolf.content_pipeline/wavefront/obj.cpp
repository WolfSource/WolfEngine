#include "w_cpipeline_pch.h"
#include "obj.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <w_thread_pool.h>
#include <sstream>

using namespace wolf::system;
using namespace wolf::content_pipeline;
using namespace wolf::content_pipeline::wavefront;

void obj::write(
	_In_ std::vector<w_vertex_struct>& pVerticesData,
	_In_ std::vector<uint32_t>& pIndicesData,
	_In_z_ const std::string& pOutputFilePath)
{
	const std::string _trace_info = "w_wavefront_obj::write";

	w_thread_pool _thread_pool;
	_thread_pool.allocate(2);

	std::array<float, 3> _pos_floats;
	std::array<float, 3> _nor_floats;
	std::array<float, 2> _uv_floats;

	std::vector<std::array<float, 3>> _pos;
	std::vector<std::array<float, 3>> _nor;
	std::vector<std::array<float, 2>> _uv;

	std::stringstream _v;
	std::stringstream _vn;
	std::stringstream _vt;
	std::stringstream _f;

	_v << "#vertices" << std::endl;
	_vt << "#texture coords" << std::endl;
	_vn << "#normals" << std::endl;
	_f << "#faces" << std::endl;

	int _v_i[3], _vn_i[3], _vt_i[3], _face_index = 0;
	for (size_t i = 0; i < pIndicesData.size(); ++i)
	{
		auto _index = pIndicesData[i];
		auto _vertex = &(pVerticesData[_index]);
		if (!_vertex)
		{
			V(W_FAILED, "vertex does not have memory", _trace_info, 3);
			continue;
		}
		//vertex position in first thread
		_thread_pool.add_job_for_thread(0, [&]()
		{
			_pos_floats[0] = _vertex->position[0];
			_pos_floats[1] = _vertex->position[1];
			_pos_floats[2] = _vertex->position[2];


			auto _iter_pos = std::find(_pos.begin(), _pos.end(), _pos_floats);
			if (_iter_pos == _pos.end())
			{
				_pos.push_back(_pos_floats);
				_v_i[_face_index] = _pos.size();

				_v << "v " << _pos_floats[0] << " " << _pos_floats[1] << " " << _pos_floats[2] << std::endl;
			}
			else
			{
				_v_i[_face_index] = std::distance(_pos.begin(), _iter_pos) + 1;
			}
		});

		//write normal and uv in another thread
		_thread_pool.add_job_for_thread(1, [&]()
		{
			_uv_floats[0] = _vertex->uv[0];
			_uv_floats[1] = _vertex->uv[1];

			auto _iter_uv = std::find(_uv.begin(), _uv.end(), _uv_floats);
			if (_iter_uv == _uv.end())
			{
				_uv.push_back(_uv_floats);
				_vt_i[_face_index] = _uv.size();

				_vt << "vt " << _uv_floats[0] << " " << _uv_floats[1] << std::endl;
			}
			else
			{
				_vt_i[_face_index] = std::distance(_uv.begin(), _iter_uv) + 1;
			}


			_nor_floats[0] = _vertex->normal[0];
			_nor_floats[1] = _vertex->normal[1];
			_nor_floats[2] = _vertex->normal[2];
			auto _iter_nor = std::find(_nor.begin(), _nor.end(), _nor_floats);
			if (_iter_nor == _nor.end())
			{
				_nor.push_back(_nor_floats);
				_vn_i[_face_index] = _nor.size();

				_vn << "vn " << _nor_floats[0] << " " << _nor_floats[1] << " " << _nor_floats[2] << std::endl;
			}
			else
			{
				_vn_i[_face_index] = std::distance(_nor.begin(), _iter_nor) + 1;
			}
		});

		_thread_pool.wait_all();

		_face_index++;
		if (_face_index == 3)
		{
			_face_index = 0;
			_f << "f " << _v_i[0] << "/" << _vt_i[0] << "/" << _vn_i[0] << "\t" << 
				_v_i[1] << "/" << _vt_i[1] << "/" << _vn_i[1] << "\t" << 
				_v_i[2] << "/" << _vt_i[2] << "/" << _vn_i[2] << std::endl;
		}
	}

	_v.flush();
	_vn.flush();
	_vt.flush();
	_f.flush();
	
	wolf::system::io::write_text_file(
		pOutputFilePath.c_str(),
		("# Wavefront OBJ file\r\n# Created by the Wolf.Engine\r\n" +
			_v.str() + "\r\n" +
			_vt.str() + "\r\n" +
			_vn.str() + "\r\n" +
			_f.str() + "\r\n").c_str());

	//release resources
	_thread_pool.release();
	_pos.clear();
	_nor.clear();
	_uv.clear();

	_v.clear();
	_vn.clear();
	_vt.clear();
	_f.clear();
}

W_RESULT obj::read(
	_Inout_ std::vector<w_vertex_struct>& pVerticesData,
	_Inout_ std::vector<uint32_t>& pIndicesData,
	_Inout_ std::vector<float>& pJustVertexPosition,
	_In_z_ const std::string& pInputFilePath)
{
	const std::string _trace_info = "w_wavefront_obj::read";

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	//std::vector<tinyobj::material_t> materials;

	std::string _err;
	bool _hr = tinyobj::LoadObj(&attrib, &shapes, nullptr, &_err, pInputFilePath.c_str());
	if (!_hr)
	{
		V(W_FAILED, "error on loading object model: " + pInputFilePath + " with followinf error info: " + _err, _trace_info, 3);
		return W_FAILED;
	}

	pVerticesData.clear();
	pIndicesData.clear();
	pJustVertexPosition.clear();

	for (size_t s = 0; s < shapes.size(); s++)
	{
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
		{
			int fv = shapes[s].mesh.num_face_vertices[f];

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++)
			{
				w_vertex_struct _vertex;
				// access to vertex
				auto _idx = shapes[s].mesh.indices[index_offset + v];
				auto _vx = attrib.vertices[3 * _idx.vertex_index + 0];
				auto _vy = attrib.vertices[3 * _idx.vertex_index + 1];
				auto _vz = attrib.vertices[3 * _idx.vertex_index + 2];
				auto _nx = attrib.normals[3 * _idx.normal_index + 0];
				auto _ny = attrib.normals[3 * _idx.normal_index + 1];
				auto _nz = attrib.normals[3 * _idx.normal_index + 2];
				auto _tx = attrib.texcoords[2 * _idx.texcoord_index + 0];
				auto _ty = attrib.texcoords[2 * _idx.texcoord_index + 1];
				
				_vertex.vertex_index = (uint32_t)_idx.vertex_index;
				_vertex.position[0] = _vx;
				_vertex.position[1] = _vy;
				_vertex.position[2] = _vz;

				_vertex.normal[0] = _nx;
				_vertex.normal[1] = _ny;
				_vertex.normal[2] = _nz;

				_vertex.uv[0] = _tx;
				_vertex.uv[1] = _ty;

				pVerticesData.push_back(_vertex);
				pIndicesData.push_back(index_offset + v);

				pJustVertexPosition.push_back(_vx);
				pJustVertexPosition.push_back(_vy);
				pJustVertexPosition.push_back(_vz);
			}
			index_offset += fv;

			// per-face material
			//shapes[s].mesh.material_ids[f];
		}
	}

	return W_PASSED;
}