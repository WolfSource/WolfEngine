#include "w_fiber.hpp"
#include <unordered_map>
#include <boost/fiber/all.hpp>

typedef struct
{
    boost::fibers::fiber*    fibers;
    size_t                   number_of_fibers;
} fibers_info;

std::unordered_map<const char*, fibers_info*> s_schedulers;

W_RESULT w_fiber_scheduler_init(_In_z_ const char* pScheduleName, _In_ const int pNumberOfFibers)
{
    auto _fibers_info =  (fibers_info*)w_alloc(sizeof(fibers_info));
    _fibers_info->fibers = (boost::fibers::fiber*)w_alloc(pNumberOfFibers * sizeof( boost::fibers::fiber));
    _fibers_info->number_of_fibers = pNumberOfFibers;
    
    s_schedulers[pScheduleName] = _fibers_info;
    return W_PASSED;
}

scheduler_info*  w_fiber_get_scheduler_info(_In_ const int pSchedulerInfo)
{
    return nullptr;
}
