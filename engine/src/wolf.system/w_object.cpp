#include "w_system_pch.h"
#include "w_object.h"

#ifdef __GNUC__
#pragma GCC visibility push(hidden)
#endif

namespace wolf
{
    namespace system
    {
        class w_object_pimp
        {
        public:
            
            bool get_is_released()
            {
                return this->_is_released;
            }
            
            ULONG release()
            {
                if (this->_is_released) return 0;
                this->_is_released = true;
                return 1;
            }

        private:
            bool _is_released;
        };
    }
}

#ifdef __GNUC__
#pragma GCC visibility pop
#endif

using namespace wolf::system;

w_object::w_object() : _pimp(new w_object_pimp())
{
    
}

ULONG w_object::release()
{
    if (!this->_pimp || (this->_pimp && this->_pimp->get_is_released())) return 0;
    
    this->tag = nullptr;
    this->name.clear();
    
    ULONG _hr = 1;
    _hr = this->_pimp->release();
    SAFE_DELETE(this->_pimp);
    return _hr;
    
}

bool w_object::get_is_released()
{
    return (!this->_pimp) || (this->_pimp && this->_pimp->get_is_released());
}

void w_object::set_class_name(const char* pClassName)
{
    this->name = pClassName;
}
