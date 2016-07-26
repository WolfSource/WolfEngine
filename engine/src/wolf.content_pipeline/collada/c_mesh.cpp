#include "w_cpipeline_pch.h"
#include "c_mesh.hpp"

using namespace std;
using namespace wolf::system;
using namespace wolf::content_pipeline::collada;

c_mesh::c_mesh()
{

}

c_mesh::~c_mesh()
{
}

ULONG c_mesh::release()
{
	if (is_released()) return 0;
	
	if (this->vertices.size() > 0)
	{
		this->vertices.clear();
	}

	if (this->indices.size() > 0)
	{
		this->indices.clear();
	}

	if (this->effects.size() > 0)
	{
		this->effects.clear();
	}

	if (this->texture_infos.size() > 0)
	{
		this->texture_infos.clear();
	}
	SAFE_DELETE(materail);

	return _super::release();
}