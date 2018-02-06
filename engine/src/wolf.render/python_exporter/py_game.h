/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website          : http://WolfSource.io
    Name             : py_game.h
    Description      : The python exporter for w_game class
    Comment          :
 */

#ifdef __PYTHON__

#ifndef __PY_GAME_H__
#define __PY_GAME_H__

#include <python_exporter/w_boost_python_helper.h>
#include <map>
#include <functional>
#include <w_task.h>
#include <mutex>

static std::once_flag _once;

namespace pywolf
{
	using namespace boost::python;
	using namespace wolf::framework;

	class py_game : public w_game
	{
	public:
		py_game(
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

		bool run(_In_ boost::python::tuple& pOutputWindowsInfo)
		{
			auto _len = len(pOutputWindowsInfo);
			if ((_len % 2) != 0) return false;

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
			auto _result = _super::run(_output_windows_info);
			_output_windows_info.clear();
			return _result;
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
			//allow graphics devices to access within this dll for python
			for (size_t i = 0; i < this->graphics_devices.size(); ++i)
			{
				//wolf::logger.write(std::to_string(this->graphics_devices[i].use_count()));
				//pywolf::py_graphics_devices.push_back(this->graphics_devices[i]);
				//wolf::logger.write(std::to_string(this->graphics_devices[i].use_count()));
				//wolf::logger.write(std::to_string(pywolf::py_graphics_devices[i].use_count()));
			}
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
		void set_pre_init_callback(_In_ std::function<void(void)> pFunc) { this->_on_pre_init_callback = pFunc; }
		static void py_set_pre_init_callback(_In_ py_game& pGame, _In_ boost::python::object pFunc) { pGame.set_pre_init_callback(pFunc); }

		void set_post_init_callback(_In_ std::function<void(void)> pFunc) { this->_on_post_init_callback = pFunc; }
		static void py_set_post_init_callback(_In_ py_game& pGame, _In_ boost::python::object pFunc) { pGame.set_post_init_callback(pFunc); }

		void set_load_callback(_In_ std::function<void(void)> pFunc) { this->_on_load_callback = pFunc; }
		static void py_set_load_callback(_In_ py_game& pGame, _In_ boost::python::object pFunc) { pGame.set_load_callback(pFunc); }

		void set_update_callback(_In_ std::function<void(wolf::system::w_game_time)> pFunc) { this->_on_update_callback = pFunc; }
		static void py_set_update_callback(_In_ py_game& pGame, _In_ boost::python::object pFunc) { pGame.set_update_callback(pFunc); }

		void set_pre_render_callback(_In_ std::function<void(wolf::system::w_game_time)> pFunc) { this->_on_pre_render_callback = pFunc; }
		static void py_set_pre_render_callback(_In_ py_game& pGame, _In_ boost::python::object pFunc) { pGame.set_pre_render_callback(pFunc); }

		void set_post_render_callback(_In_ std::function<void(bool)> pFunc) { this->_on_post_render_callback = pFunc; }
		static void py_set_post_render_callback(_In_ py_game& pGame, _In_ boost::python::object pFunc) { pGame.set_post_render_callback(pFunc); }

		void set_window_resized_callback(_In_ std::function<void(uint32_t)> pFunc) { this->_on_window_resized_callback = pFunc; }
		static void py_set_window_resized_callback(_In_ py_game& pGame, _In_ boost::python::object pFunc) { pGame.set_window_resized_callback(pFunc); }

		void set_device_lost_callback(_In_ std::function<void(void)> pFunc) { this->_on_device_lost_callback = pFunc; }
		static void py_set_device_lost_callback(_In_ py_game& pGame, _In_ boost::python::object pFunc) { pGame.set_device_lost_callback(pFunc); }

		void set_fixed_time_step(_In_ const bool& pValue) { _super::set_fixed_time_step(pValue); }
		void set_fixed_fps(_In_ const double& pValue) { _super::set_fixed_fps(pValue); }
#pragma endregion

#pragma region Getters

		boost::shared_ptr<w_graphics_device> py_get_graphics_device(_In_ const size_t& pGraphicsDeviceIndex) const
		{
			auto _gDevice = get_graphics_device(pGraphicsDeviceIndex);
			if (_gDevice) return std_shared_ptr_to_boost_shared_ptr<w_graphics_device>(_gDevice);
			return nullptr;
		}

#pragma endregion

	private:
		typedef w_game _super;
		std::function<void(void)> _on_pre_init_callback;
		std::function<void(void)> _on_post_init_callback;
		std::function<void(void)> _on_load_callback;
		std::function<void(wolf::system::w_game_time)> _on_update_callback;
		std::function<void(wolf::system::w_game_time)> _on_pre_render_callback;
		std::function<void(bool)> _on_post_render_callback;
		std::function<void(uint32_t)> _on_window_resized_callback;
		std::function<void(void)> _on_device_lost_callback;
	};

	static void py_game_export()
	{
		//export w_game class
		class_<py_game, boost::noncopyable>("w_game", init<std::wstring, std::wstring, std::wstring>())
			.def("run", &py_game::run, "This will run the main loop cycle of the game")
			.def("exit", &py_game::release, "release all resources and exit")
			.def("set_pre_init_callback", &py_game::py_set_pre_init_callback, "set a callback function which rises before initializing graphics device(s)")
			.def("set_post_init_callback", &py_game::py_set_post_init_callback, "set a callback function which rises after initializing graphics device(s)")
			.def("set_load_callback", &py_game::py_set_load_callback, "set a callback function which rises on loading")
			.def("set_update_callback", &py_game::py_set_update_callback, "set a callback function which rises on updating")
			.def("set_pre_render_callback", &py_game::py_set_pre_render_callback, "set a callback function which rises before rendering")
			.def("set_post_render_callback", &py_game::py_set_post_render_callback, "set a callback function which rises after rendering")
			.def("set_window_resized_callback", &py_game::py_set_window_resized_callback, "set a callback function which rises on window resizing")
			.def("set_device_lost_callback", &py_game::py_set_device_lost_callback, "set a callback function which rises on device losting")
			.def("set_fixed_time_step", &py_game::set_fixed_time_step, "set fixed time step")
			.def("set_fixed_fps", &py_game::set_fixed_fps, "set fixed frame per seconds")
			.def("set_graphics_device_manager_configs", &py_game::set_graphics_device_manager_configs, "set configuration for graphics device manager")
			.def("get_number_of_graphics_devices", &py_game::get_number_of_graphics_devices, "Get number of available graphics devices")
			.def("get_graphics_device", &py_game::py_get_graphics_device, "Get the graphics device")
			;

		register_ptr_to_python< boost::shared_ptr<w_graphics_device>>();

	}
}

#endif//__PY_GAME_H__

#endif//__PYTHON__
