/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website          : http://WolfSource.io
    Name             : w_game_py.h
    Description      : The python exporter for w_game class
    Comment          :
 */

#ifdef __PYTHON__

#ifndef __W_GAME_PY_H__
#define __W_GAME_PY_H__

#include <map>
#include <functional>
#include <w_task.h>
#include <mutex>

static std::once_flag _once;

namespace pywolf
{
	class wrapper_game : public wolf::framework::w_game
	{
	public:
		wrapper_game(
			_In_z_ const std::wstring& pContentPath, 
			_In_z_ const std::wstring& pLogPath, 
			_In_z_ const std::wstring& pAppName) :
			_super(pContentPath, pLogPath, pAppName),
			_on_pre_init_callback(nullptr),
			_on_post_init_callback(nullptr),
			_on_load_callback(nullptr),
			_on_update_callback(nullptr),
			_on_pre_render_callback(nullptr),
			_on_post_render_callback(nullptr),
			_on_window_resized_callback(nullptr),
			_on_device_lost_callback(nullptr)
		{
		}

		bool run(_In_ std::map<int, w_window_info>& pOutputWindowsInfo)
		{
			return _super::run(pOutputWindowsInfo);
		}
		
		ULONG release() override 
		{
			if (_super::get_is_released()) return 1;
			
			this->_on_pre_init_callback = nullptr;
			this->_on_post_init_callback = nullptr;
			this->_on_load_callback = nullptr;
			this->_on_update_callback = nullptr;
			this->_on_pre_render_callback = nullptr;
			this->_on_post_render_callback = nullptr;
			this->_on_window_resized_callback = nullptr;
			this->_on_device_lost_callback = nullptr;

			return _super::release(); 
		}

		void initialize(_In_ std::map<int, w_window_info> pOutputWindowsInfo) override
		{
			if (this->_on_pre_init_callback) this->_on_pre_init_callback();
			_super::initialize(pOutputWindowsInfo);
			if (this->_on_post_init_callback) this->_on_post_init_callback();
		}
		void load() override
		{
			if (this->_on_load_callback) this->_on_load_callback();
			_super::load();
		}
		void update(_In_ const wolf::system::w_game_time& pGameTime) override
		{
			if (this->_on_update_callback) this->_on_update_callback(pGameTime);
			_super::update(pGameTime);
		}
		HRESULT render(_In_ const wolf::system::w_game_time& pGameTime) override
		{
			if (this->_on_pre_render_callback) this->_on_pre_render_callback(pGameTime);
			auto _hr = _super::render(pGameTime);
			if (this->_on_post_render_callback) this->_on_post_render_callback(_hr == S_OK);
			return _hr;
		}
		void on_window_resized(_In_ const uint32_t& pGraphicsDeviceIndex) override
		{
			_super::on_window_resized(pGraphicsDeviceIndex);
			if (this->_on_window_resized_callback) this->_on_window_resized_callback(pGraphicsDeviceIndex);
		}
		void on_device_lost() override
		{
			_super::on_device_lost();
			if (this->_on_device_lost_callback) this->_on_device_lost_callback();
		}
		
#pragma region Setters
		void set_pre_init_callback(_In_ std::function<void(void)>& pFunc) { this->_on_pre_init_callback = pFunc; }
		void set_post_init_callback(_In_ std::function<void(void)>& pFunc) { this->_on_post_init_callback = pFunc; }
		void set_load_callback(_In_ std::function<void(void)>& pFunc) { this->_on_load_callback = pFunc; }
		void set_update_callback(_In_ std::function<void(wolf::system::w_game_time)>& pFunc) { this->_on_update_callback = pFunc; }
		void set_pre_render_callback(_In_ std::function<void(wolf::system::w_game_time)>& pFunc) { this->_on_pre_render_callback = pFunc; }
		void set_post_render_callback(_In_ std::function<void(bool)>& pFunc) { this->_on_post_render_callback = pFunc; }
		void set_window_resized_callback(_In_ std::function<void(uint32_t)>& pFunc) { this->_on_window_resized_callback = pFunc; }
		void set_device_lost_callback(_In_ std::function<void(void)>& pFunc) { this->_on_device_lost_callback = pFunc; }
#pragma endregion

	private:
		typedef wolf::framework::w_game _super;
		std::function<void(void)> _on_pre_init_callback;
		std::function<void(void)> _on_post_init_callback;
		std::function<void(void)> _on_load_callback;
		std::function<void(wolf::system::w_game_time)> _on_update_callback;
		std::function<void(wolf::system::w_game_time)> _on_pre_render_callback;
		std::function<void(bool)> _on_post_render_callback;
		std::function<void(uint32_t)> _on_window_resized_callback;
		std::function<void(void)> _on_device_lost_callback;
	};

	class py_game
	{
	public:
		py_game(
			_In_z_ const std::wstring& pContentPath,
			_In_z_ const std::wstring& pLogPath,
			_In_z_ const std::wstring& pAppName) :
			_wrapper(new wrapper_game(pContentPath, pLogPath, pAppName))
		{
		}
		~py_game()
		{
			exit();
		}

		void run(_In_ boost::python::tuple& pOutputWindowsInfo)
		{
			auto _len = len(pOutputWindowsInfo);
			if (!this->_wrapper || (_len % 2) != 0) return;

			std::map<int, w_window_info> _output_windows_info;
			//nedded just for initialize time
			std::call_once(_once, [&]()
			{
				for (auto i = 0; i < _len; i += 2)
				{
					boost::python::extract<int> _k(pOutputWindowsInfo[i]);
					boost::python::extract<w_window_info> _v(pOutputWindowsInfo[i + 1]);
					if (_k.check() && _v.check())
					{
						_output_windows_info[_k()] = _v();
					}
				}
			});
			this->_wrapper->run(_output_windows_info);
			_output_windows_info.clear();
		}
		void exit()
		{
			if (!this->_wrapper) return;
			this->_wrapper->release();
			SAFE_DELETE(this->_wrapper);
		}

#pragma region Setters
		void set_pre_init_callback(_In_ std::function<void(void)> pFunc)
		{
			if (!this->_wrapper) return;
			this->_wrapper->set_pre_init_callback(pFunc);
		}
		void set_post_init_callback(_In_ std::function<void(void)> pFunc)
		{
			if (!this->_wrapper) return;
			this->_wrapper->set_post_init_callback(pFunc);
		}
		void set_load_callback(_In_ std::function<void(void)> pFunc)
		{
			if (!this->_wrapper) return;
			this->_wrapper->set_load_callback(pFunc);
		}
		void set_update_callback(_In_ std::function<void(wolf::system::w_game_time)> pFunc)
		{
			if (!this->_wrapper) return;
			this->_wrapper->set_update_callback(pFunc);
		}
		void set_pre_render_callback(_In_ std::function<void(wolf::system::w_game_time)> pFunc)
		{
			if (!this->_wrapper) return;
			this->_wrapper->set_pre_render_callback(pFunc);
		}
		void set_post_render_callback(_In_ std::function<void(bool)> pFunc)
		{
			if (!this->_wrapper) return;
			this->_wrapper->set_post_render_callback(pFunc);
		}
		void set_window_resized_callback(_In_ std::function<void(uint32_t)> pFunc)
		{
			if (!this->_wrapper) return;
			this->_wrapper->set_window_resized_callback(pFunc);
		}
		void set_device_lost_callback(_In_ std::function<void(void)> pFunc)
		{
			if (!this->_wrapper) return;
			this->_wrapper->set_device_lost_callback(pFunc);
		}
#pragma endregion

#pragma region Python Wrappers
		static void py_set_pre_init_callback(_In_ py_game& pSelf, _In_ boost::python::object pObject) { pSelf.set_pre_init_callback(pObject); }
		static void py_set_post_init_callback(_In_ py_game& pSelf, _In_ boost::python::object pObject) { pSelf.set_post_init_callback(pObject); }
		static void py_set_load_callback(_In_ py_game& pSelf, _In_ boost::python::object pObject) { pSelf.set_load_callback(pObject); }
		static void py_set_update_callback(_In_ py_game& pSelf, _In_ boost::python::object pObject) { pSelf.set_update_callback(pObject); }
		static void py_set_pre_render_callback(_In_ py_game& pSelf, _In_ boost::python::object pObject) { pSelf.set_pre_render_callback(pObject); }
		static void py_set_post_render_callback(_In_ py_game& pSelf, _In_ boost::python::object pObject) { pSelf.set_post_render_callback(pObject); }
		static void py_set_window_resized_callback(_In_ py_game& pSelf, _In_ boost::python::object pObject) { pSelf.set_window_resized_callback(pObject); }
		static void py_set_device_lost_callback(_In_ py_game& pSelf, _In_ boost::python::object pObject) { pSelf.set_device_lost_callback(pObject); }
#pragma endregion

	private:
		wrapper_game * _wrapper;
	};

	static void w_game_py_export()
	{
		using namespace boost::python;
		using namespace wolf::framework;

		//export w_game class
		class_<py_game>("w_game", init<std::wstring, std::wstring, std::wstring>())
			.def("run", &py_game::run, "This will run the main loop cycle of the game")
			.def("exit", &py_game::exit, "release all resources and exit")
			.def("set_pre_init_callback", &py_game::py_set_pre_init_callback, "set a callback function which rises before initializing graphics device(s)")
			.def("set_post_init_callback", &py_game::py_set_post_init_callback, "set a callback function which rises after initializing graphics device(s)")
			.def("set_load_callback", &py_game::py_set_load_callback, "set a callback function which rises on loading")
			.def("set_update_callback", &py_game::py_set_update_callback, "set a callback function which rises on updating")
			.def("set_pre_render_callback", &py_game::py_set_pre_render_callback, "set a callback function which rises before rendering")
			.def("set_post_render_callback", &py_game::py_set_post_render_callback, "set a callback function which rises after rendering")
			.def("set_window_resized_callback", &py_game::py_set_window_resized_callback, "set a callback function which rises on window resizing")
			.def("set_device_lost_callback", &py_game::py_set_device_lost_callback, "set a callback function which rises on device losting")
			;
	}
}

#endif//__W_GAME_PY_H__

#endif//__PYTHON__
