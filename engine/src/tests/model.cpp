#include "pch.h"
#include "model.h"

using namespace wolf::system;

#define COMPUE_SHADER_LOCAL_SIZE_X_ 1

model::model()
{
}

model::~model()
{
    release();
}

void model::load()
{
    
}

ULONG model::release()
{
    if (this->get_is_released()) return 0;


    return _super::release();
}