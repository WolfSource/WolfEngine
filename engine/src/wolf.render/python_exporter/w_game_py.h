/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website          : http://WolfSource.io
    Name             : w_game.h
    Description      : The python exporter for w_game class and relative structs
    Comment          :
 */

#ifdef __PYTHON__

#ifndef __W_GAME_PY_H__
#define __W_GAME_PY_H__

#include <map>

namespace pywolf
{
    class wrapper_game : public wolf::framework::w_game
    {
    public:
        wrapper_game(_In_z_ const std::wstring& pRunningDirectory, _In_z_ const std::wstring& pAppName) :
            _super(pRunningDirectory, pAppName) {}
        
        bool run(_In_ std::map<int, w_window_info> pOutputWindowsInfo) { return _super::run(pOutputWindowsInfo); }
        void exit(_In_ const int& pExitCode = 0) { return _super::exit(pExitCode); }
        ULONG release() override { return _super::release(); }

        void initialize(_In_ std::map<int, w_window_info> pOutputWindowsInfo) override { _super::initialize(pOutputWindowsInfo); }
        void load() override { _super::load(); }
        void update(_In_ const wolf::system::w_game_time& pGameTime) override { _super::update(pGameTime); }
        HRESULT render(_In_ const wolf::system::w_game_time& pGameTime) override { return _super::render(pGameTime); }
        void on_window_resized(_In_ uint32_t pIndex) override { return _super::on_window_resized(pIndex); }
        void on_device_lost() override { return _super::on_device_lost(); }

    private:
        typedef wolf::framework::w_game _super;
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
            this->_wrapper->release();
        }
    private:
        wrapper_game * _wrapper;
    };

    static void w_game_py_export()
    {
        using namespace boost::python;
        using namespace wolf::framework;

        //export w_game class
        class_<py_game>("w_game", init<std::wstring, std::wstring>())
            .def("run", &py_game::run, "//This will run the main loop cycle of the game")
            ;
    }
}

#endif//__W_GAME_PY_H__

#endif//__PYTHON__