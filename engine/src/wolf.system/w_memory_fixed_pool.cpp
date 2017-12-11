#include "w_system_pch.h"
#include "w_memory_fixed_pool.h"

using namespace wolf;
using namespace wolf::system;

#ifdef __GNUC__
#pragma GCC visibility push(hidden)
#endif

namespace wolf
{
    namespace system
    {
        class w_memory_fixed_pool_pimp
        {
        public:
            w_memory_fixed_pool_pimp() : _pool(nullptr)
            {
                _pool = new tbb::fixed_pool()
            }

            ULONG release()
            {
                return 0;
            }

        private:
           

        };
    }
}

#ifdef __GNUC__
#pragma GCC visibility pop(hidden)
#endif

w_memory_fixed_pool::w_memory_fixed_pool() : _pimp(new w_memory_fixed_pool_pimp())
{

}

w_memory_fixed_pool::~w_memory_fixed_pool()
{
    release();
}

ULONG w_memory_fixed_pool::release()
{
    if (_super::get_is_released()) return 1;

    SAFE_RELEASE(this->_pimp);

    return _super::release();
}