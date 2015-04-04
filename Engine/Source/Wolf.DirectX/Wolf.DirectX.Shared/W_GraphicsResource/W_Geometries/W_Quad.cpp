#include "Wolf.DirectX.Shared.PCH.h"
#include "W_Quad.h"

using namespace std;
using namespace DirectX;
using namespace Wolf::Graphics;
using namespace Wolf::Graphics::VertexTypes;

class W_Quad::Impl
{
public:
	Impl()
	{
	}

	virtual ~Impl()
	{
	}

	HRESULT LoadMesh(_Inout_ W_Mesh* pMesh, bool pIsSprite)
	{
		const UINT verticesSize = 4;
		const VertexPositionNormalTexture vertices[verticesSize] =
		{
			{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) }
		};

		const UINT indicesSize = 6;
		const unsigned short Indices[] = { 0, 1, 2, 0, 3, 1 };

		pMesh->SetIsSprite(pIsSprite);
		if (pIsSprite)
		{
			return pMesh->Load(L"Shaders\\SpriteVS.cso", L"Shaders\\SpritePS.cso", vertices, verticesSize, Indices, indicesSize);
		}

		return pMesh->Load(L"Shaders\\QuadVS.cso", L"Shaders\\QuadPS.cso", vertices, verticesSize, Indices, indicesSize);
	}
	
};

W_Quad::W_Quad(const shared_ptr<W_GraphicsDevice>& pGDevice) : W_Mesh(pGDevice), impl(new Impl())
{
	this->Name = typeid(this).name();
}

W_Quad::~W_Quad()
{
	Release();
}

HRESULT W_Quad::Load(bool pIsSprite)
{		
	return this->impl->LoadMesh((W_Mesh*)this, pIsSprite);
}

ULONG W_Quad::Release()
{
	if (this->IsReleased()) return 0;

	return W_Mesh::Release();
}