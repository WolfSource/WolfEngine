/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website          : http://WolfSource.io
    Name             : w_game_py.h
    Description      : The python exporter for w_game class and relative structs
    Comment          :
 */

#ifdef __PYTHON__

#ifndef __W_GAME_PY_H__
#define __W_GAME_PY_H__

#include <map>
#include <functional>

namespace pywolf
{
    class wrapper_game : public wolf::framework::w_game
    {
    public:
        wrapper_game(_In_z_ const std::wstring& pRunningDirectory, _In_z_ const std::wstring& pAppName) :
            _super(pRunningDirectory, pAppName),
            _on_pre_init_callback(nullptr),
            _on_post_init_callback(nullptr),
            _on_load_callback(nullptr),
            _on_update_callback(nullptr),
            _on_pre_render_callback(nullptr),
            _on_post_render_callback(nullptr)
        {
        }
        
        bool run(_In_ std::map<int, w_window_info> pOutputWindowsInfo) { return _super::run(pOutputWindowsInfo); }
        void exit(_In_ const int& pExitCode = 0) { return _super::exit(pExitCode); }
        ULONG release() override { return _super::release(); }

        void initialize(_In_ std::map<int, w_window_info> pOutputWindowsInfo) override
        {
            if (_on_pre_init_callback) _on_pre_init_callback();
            _super::initialize(pOutputWindowsInfo);
            if (_on_post_init_callback) _on_post_init_callback();
        }
        void load() override
        {
            if (_on_load_callback) _on_load_callback();
            _super::load();
        }
        void update(_In_ const wolf::system::w_game_time& pGameTime) override
        {
            _super::update(pGameTime);
        }
        HRESULT render(_In_ const wolf::system::w_game_time& pGameTime) override
        {
            return _super::render(pGameTime);
        }
        void on_window_resized(_In_ uint32_t pIndex) override
        {
            return _super::on_window_resized(pIndex);
        }
        void on_device_lost() override
        {
            return _super::on_device_lost();
        }
        
#pragma region Setters
        void set_pre_init_callback(std::function<void(void)> pFunc) { this->_on_pre_init_callback = pFunc; }
        void set_post_init_callback(std::function<void(void)> pFunc) { this->_on_post_init_callback = pFunc; }
        void set_load_callback(std::function<void(void)> pFunc) { this->_on_load_callback = pFunc; }
        void set_update_callback(std::function<void(void)> pFunc) { this->_on_update_callback = pFunc; }
        void set_pre_render_callback(std::function<void(void)> pFunc) { this->_on_pre_render_callback = pFunc; }
        void set_post_render_callback(std::function<void(bool)> pFunc) { this->_on_post_render_callback = pFunc; }
#pragma endregion
        
    private:
        typedef wolf::framework::w_game _super;
        std::function<void(void)> _on_pre_init_callback;
        std::function<void(void)> _on_post_init_callback;
        std::function<void(void)> _on_load_callback;
        std::function<void(void)> _on_update_callback;
        std::function<void(void)> _on_pre_render_callback;
        std::function<void(bool)> _on_post_render_callback;
    };

    class py_game
    {
    public:
        py_game(_In_z_ const std::wstring& pRunningDirectory, _In_z_ const std::wstring& pAppName) :
            _wrapper(new wrapper_game(pRunningDirectory, pAppName))
        {
        }
        ~py_game()
        {
            release();
        }

        bool run(boost::python::tuple pOutputWindowsInfo)
        {
            auto _len = len(pOutputWindowsInfo);
            if (!this->_wrapper || (_len % 2) != 0) return false;

            std::map<int, w_window_info> _output_windows_info;
            for (auto i = 0; i < _len; i += 2)
            {
                boost::python::extract<int> _k(pOutputWindowsInfo[i]);
                boost::python::extract<w_window_info> _v(pOutputWindowsInfo[i + 1]);
                if (_k.check() && _v.check())
                {
                    _output_windows_info[_k()] = _v();
                }
            }
            return this->_wrapper->run(_output_windows_info);
        }
        void exit(_In_ const int& pExitCode) 
        {
            if (!this->_wrapper) return; 
            this->_wrapper->exit(pExitCode); 
        }
        ULONG release()
        {
            if (!this->_wrapper) return 1;
            return this->_wrapper->release();
        }
        
#pragma region Setters
        void set_pre_init_callback(std::function<void(void)> pFunc)
        {
            if (!this->_wrapper) return;
            this->_wrapper->set_pre_init_callback(pFunc);
        }
        void set_post_init_callback(std::function<void(void)> pFunc)
        {
            if (!this->_wrapper) return;
            this->_wrapper->set_post_init_callback(pFunc);
        }
        void set_load_callback(std::function<void(void)> pFunc)
        {
            if (!this->_wrapper) return;
            this->_wrapper->set_load_callback(pFunc);
        }
        void set_update_callback(std::function<void(void)> pFunc)
        {
            if (!this->_wrapper) return;
            this->_wrapper->set_update_callback(pFunc);
        }
        void set_pre_render_callback(std::function<void(void)> pFunc)
        {
            if (!this->_wrapper) return;
            this->_wrapper->set_pre_render_callback(pFunc);
        }
        void set_post_render_callback(std::function<void(bool)> pFunc)
        {
            if (!this->_wrapper) return;
            this->_wrapper->set_post_render_callback(pFunc);
        }
#pragma endregion
        
#pragma region Python Wrappers
        static void py_set_pre_init_callback(py_game& pSelf, boost::python::object pObject) { pSelf.set_pre_init_callback(pObject); }
        static void py_set_post_init_callback(py_game& pSelf, boost::python::object pObject) { pSelf.set_post_init_callback(pObject); }
        static void py_set_load_callback(py_game& pSelf, boost::python::object pObject) { pSelf.set_load_callback(pObject); }
        static void py_set_update_callback(py_game& pSelf, boost::python::object pObject) { pSelf.set_update_callback(pObject); }
        static void py_set_pre_render_callback(py_game& pSelf, boost::python::object pObject) { pSelf.set_pre_render_callback(pObject); }
        static void py_set_post_render_callback(py_game& pSelf, boost::python::object pObject) { pSelf.set_post_render_callback(pObject); }
#pragma endregion
        
    private:
        wrapper_game *  _wrapper;
    };

    static void w_game_py_export()
    {
        using namespace boost::python;
        using namespace wolf::framework;

        //export w_game class
        class_<py_game>("w_game", init<std::wstring, std::wstring>())
            .def("run", &py_game::run, "This will run the main loop cycle of the game")
            .def("set_pre_init_callback", &py_game::py_set_pre_init_callback, "set a call back function which rises before initializing graphics device(s)")
            .def("set_post_init_callback", &py_game::py_set_post_init_callback, "set a call back function which rises after initializing graphics device(s)")
            .def("set_load_callback", &py_game::py_set_load_callback, "set a call back function which rises on loading")
            .def("set_update_callback", &py_game::py_set_update_callback, "set a call back function which rises on updating")
            .def("set_pre_render_callback", &py_game::py_set_pre_render_callback, "set a call back function which rises before rendering")
            .def("set_post_render_callback", &py_game::py_set_post_render_callback, "set a call back function which rises after rendering")
        ;
    }
}

#endif//__W_GAME_PY_H__

#endif//__PYTHON__
