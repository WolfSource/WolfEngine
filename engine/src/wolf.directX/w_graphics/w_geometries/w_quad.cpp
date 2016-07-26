#include "w_directX_pch.h"
#include "w_quad.h"

using namespace std;
using namespace DirectX;
using namespace wolf::graphics;

class w_quad::Impl
{
public:
	Impl()
	{
	}

	virtual ~Impl()
	{
	}

	HRESULT load_mesh(_In_ ID3D11Device1* pDevice, _Inout_ w_mesh* pMesh, bool pLoadDeafultTexture2D, std::wstring pTexture2DPath, SHADER_TYPE pShaderType)
	{
		const UINT _verticesSize = 4;
		const wolf::content_pipeline::vertex_declaration_structs::vertex_position_normal_uv _vertices[_verticesSize] =
		{
			{ glm::vec3(1.0f,  1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
			{ glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f) },
			{ glm::vec3(-1.0f,  1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
			{ glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f) }
		};

		const UINT _indicesSize = 6;
		const unsigned short _indices [] = { 0, 1, 2, 0, 3, 1 };

		//pMesh->set_isSprite(pIsSprite);
		//if (pIsSprite)
		//{
		//	return pMesh->load(pDevice, L"Shaders\\SpriteVS.cso", L"Shaders\\SpritePS.cso", pTexture2DPath, pLoadDeafultTexture2D, _vertices, _verticesSize, _indices, _indicesSize);
		//}

		auto _hr = S_FALSE;
		std::wstring _v_shader_path = L"", _p_shader_path = L"";
		switch (pShaderType)
		{
		case SHADER_TYPE::TRANSITION_CROSS_X:
			_v_shader_path = L"Shaders\\quad_vs.cso";
			_p_shader_path = L"Shaders\\transition_crossX_ps.cso";
			break;
		case SHADER_TYPE::TRANSITION_CROSS_Y:
			_v_shader_path = L"Shaders\\quad_vs.cso";
			_p_shader_path = L"Shaders\\transition_crossY_ps.cso";
			break;
		default:
			_v_shader_path = L"Shaders\\quad_vs.cso";
			_p_shader_path = L"Shaders\\quad_ps.cso";
			break;
		}

		return pMesh->load(
			pDevice,
			_v_shader_path,
			_p_shader_path,
			pTexture2DPath,
			pLoadDeafultTexture2D,
			_vertices,
			_verticesSize,
			_indices,
			_indicesSize);
	}
};

w_quad::w_quad() : w_mesh(), _impl(new Impl())
{
	_super::set_class_name(typeid(this).name());
}

w_quad::~w_quad()
{
	release();
}

HRESULT w_quad::load(_In_ ID3D11Device1* pDevice, bool pLoadDeafultTexture2D, std::wstring pTexture2DPath, SHADER_TYPE pShaderType)
{		
	return this->_impl->load_mesh(pDevice, (w_mesh*)this, pLoadDeafultTexture2D, pTexture2DPath, pShaderType);
}

ULONG w_quad::release()
{
	if (_super::is_released()) return 0;

	this->_impl = nullptr;

	return w_mesh::release();
}