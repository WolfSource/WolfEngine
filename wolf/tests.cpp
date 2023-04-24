/*
        Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
        https://github.com/WolfEngine/WolfEngine
*/

#include <wolf/system/w_leak_detector.hpp>
#include <wolf/wolf.hpp>
#include "wolf/system/script/w_python.hpp"
#define BOOST_TEST_MODULE wolf
#include <boost/test/included/unit_test.hpp>
#include <iostream>
using namespace boost::python;
using namespace python_wrapper; using namespace python_testcase;
void python_execution_test_case();

BOOST_AUTO_TEST_CASE(wolf_test) {

    const wolf::system::w_leak_detector _detector = {};

    std::cout << "entering test case 'wolf_test'" << std::endl;
    const auto _wolf_version = wolf::w_init();

    python_execution_test_case();

    BOOST_TEST(_wolf_version.empty() == false);
    std::cout << "leaving test case 'wolf_test'" << std::endl;
}

void python_execution_test_case()
{
    try {
        apply_simple_script();
        apply_simple_script_from_file();
        apply_run_function_from_script_file_and_get_result();
        apply_call_function();
    }
    catch(const std::runtime_error& err)
    {
        std::cout << err.what() << '\n';
    }
}

#pragma region system tests

//#include <wolf/system/test/compress.hpp>
//#include <wolf/system/test/coroutine.hpp>
//#include <wolf/system/test/gamepad.hpp>
//#include <wolf/system/test/gametime.hpp>
//#include <wolf/system/test/log.hpp>
////#include <wolf/system/test/postgresql.hpp>
//#include <wolf/system/test/process.hpp>
//#include <wolf/system/test/signal_slot.hpp>
//#include <wolf/system/test/tcp.hpp>
//#include <wolf/system/test/trace.hpp>
//#include <wolf/system/test/ws.hpp>
//#include <wolf/system/test/lua.hpp>

#pragma endregion

#pragma region stream tests

//#include <wolf/stream/test/ffmpeg_stream.hpp>
//#include <wolf/stream/test/rist.hpp>

#pragma endregion

#pragma region media tests

//#include <wolf/media/test/avframe.hpp>
//#include <wolf/media/test/ffmpeg.hpp>
//#include <wolf/media/test/image.hpp>
//#include <wolf/media/test/openal.hpp>

#pragma endregion

#pragma region ml tests

//#include <wolf/ml/test/w_image_processor_test.hpp>
//#include <wolf/ml/test/w_ocr_engine_test.hpp>
//#include <wolf/ml/test/w_referee_test.hpp>
//#include <wolf/ml/test/w_soccer_test.hpp>
//#include <wolf/ml/test/w_utilities_test.hpp>

#pragma endregion
