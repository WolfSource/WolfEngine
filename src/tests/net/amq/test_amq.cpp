#include <wolf.h>
#include <net/w_net.h>
#include <log/w_log.h>
#include <io/w_io.h>

int main()
{
	W_RESULT _ret = W_SUCCESS;
	w_mem_pool _mem_pool = nullptr;

	if (wolf_init() != W_SUCCESS)
	{
		_ret = W_FAILURE;
		goto exit;
	}

	//initialize memory pool
	w_mem_pool_init(&_mem_pool);
	if (!_mem_pool)
	{
		_ret = W_FAILURE;
		goto exit;
	}

	/*char* _path = nullptr;
	w_io_dir_get_current_exe(_mem_pool, &_path);
	if (!_path)
	{
		wolf_fini();
		return W_FAILURE;
	}
	w_log_config _c = {};
	_c.app_name = "fiber_server";
	_c.log_to_std_out = true;
	_c.log_dir_path = _path;

	if (w_log_init(_mem_pool, &_c) != W_SUCCESS)
	{
		wolf_fini();
		return W_FAILURE;
	}

	constexpr auto _port = 7777;
	LOG_P(W_LOG_DEBUG, "starting fiber server on port: %d", _port);*/

exit:
	wolf_fini();
	return _ret;
}