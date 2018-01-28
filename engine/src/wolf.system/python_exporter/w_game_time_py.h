/*
Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
Website			 : http://WolfSource.io
Name			 : w_game_time_py.h
Description		 : The python exporter for w_game_time class
Comment          :
*/

namespace pywolf
{
    static void w_game_time_py_export()
    {
        using namespace wolf::system;
        using namespace boost::python;

        //export w_game_time class
        class_<w_game_time>("w_game_time", init<>())
            .def_readonly("TICKS_PER_SECOND", &w_game_time::py_TICKS_PER_SECOND, "Total ticks per one second")
            .def("ticks_to_seconds", &w_game_time::py_ticks_to_seconds, "Convert ticks to seconds")
            .def("seconds_to_ticks", &w_game_time::py_seconds_to_ticks, "Convert seconds to ticks")
            .def("reset", &w_game_time::reset, "reset")
            .def("tick", &w_game_time::py_tick, "call this method for updating game time object")
            .def("get_elapsed_ticks", &w_game_time::get_elapsed_ticks, "get elapsed ticks value")
            .def("get_elapsed_seconds", &w_game_time::get_elapsed_seconds, "get elapsed seconds value")
            .def("get_total_ticks", &w_game_time::get_total_ticks, "get total ticks value")
            .def("get_total_seconds", &w_game_time::get_total_seconds, "get total seconds value")
            .def("get_frame_count", &w_game_time::get_frame_count, "get frame count value")
            .def("get_frames_per_second", &w_game_time::get_frames_per_second, "get frames per second value")
            .def("get_fixed_time_step", &w_game_time::get_fixed_time_step, "get fixed time step value")
            .def("set_fixed_time_step", &w_game_time::set_fixed_time_step, "set fixed time step value")
            .def("set_target_elapsed_ticks", &w_game_time::set_target_elapsed_ticks, "set target elapsed ticks value")
            .def("set_target_elapsed_seconds", &w_game_time::set_target_elapsed_seconds, "set target elapsed seconds value")
            ;
    }
}
