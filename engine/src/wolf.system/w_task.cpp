#include "w_system_pch.h"
#include "w_task.h"

using namespace wolf::system;

std::future<W_RESULT> w_task::_deferred;

void w_task::execute_async(
	_In_ const std::function<W_RESULT(void)>& pTaskWork, 
    _In_ const std::function<void(W_RESULT)>& pCallBack)
{

#if defined(__WIN32) || defined(__UWP)

	concurrency::create_task(pTaskWork).then([pCallBack](W_RESULT pHR)
	{
		if (pCallBack)
		{
			pCallBack(pHR);
		}
	});

#else

	std::async(std::launch::async, [pTaskWork, pCallBack]()
	{
		auto _hr = pTaskWork();
		if (pCallBack)
		{
			pCallBack(_hr);
		}
		return _hr;
	});

#endif
}

void w_task::execute_deferred(_In_ const std::function<W_RESULT(void)>& pTaskWork)
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