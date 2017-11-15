#include "w_render_pch.h"
#include "w_bounding_frustum.h"

using namespace std;
using namespace wolf::framework;

w_bounding_frustum::w_bounding_frustum()
{
}

w_bounding_frustum::w_bounding_frustum(_In_ const wolf::content_pipeline::w_bounding_box& pBoundingBox)
{
}

w_bounding_frustum::~w_bounding_frustum()
{
    release();
}

ULONG w_bounding_frustum::release()
{
    if (_super::get_is_released()) return 1;
	
    
	return  _super::release();
}
