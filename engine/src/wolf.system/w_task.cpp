#include "w_system_pch.h"
#include "w_task.h"

using namespace wolf::system;

std::future<void> w_task::_deferred;

#if defined(__WIN32) || defined(__UWP)

void w_task::execute_async_ppl(_In_ const std::function<void(void)>& pTaskWork,
    _In_ const std::function<void(void)>& pCallBack)
{
	concurrency::create_task(pTaskWork).then(pCallBack);
}

#endif

void w_task::execute_async(_In_ const std::function<void(void)>& pTaskWork, 
    _In_ const std::function<void(void)>& pCallBack)
{
	_deferred = std::async(std::launch::async, [pTaskWork, pCallBack]()
	{
		pTaskWork();
		if (pCallBack)
		{
			pCallBack();
		}
	});
}

void w_task::execute_deferred(_In_ const std::function<void(void)>& pTaskWork)
{
	_deferred = std::async(std::launch::deferred, pTaskWork);
}

template<typename _REP, typename _PER>
std::future_status w_task::wait_for(_In_ const std::chrono::duration<_REP, _PER>& pTime)
{
	return _deferred.wait_for(pTime);
}

std::future_status w_task::wait_for(_In_ const long long pMilliSeconds)
{
	return _deferred.wait_for(std::chrono::milliseconds(pMilliSeconds));
}

void w_task::wait()
{
	_deferred.wait();
}

void w_task::get()
{
	_deferred.get();
}
