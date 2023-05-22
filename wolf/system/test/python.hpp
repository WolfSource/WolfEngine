#if defined(WOLF_TEST) && defined(WOLF_SYSTEM_PYTHON)

#include <boost/test/included/unit_test.hpp>
#include <system/w_leak_detector.hpp>
#include <wolf/wolf.hpp>

#include <system/script/w_python.hpp>

using w_python = wolf::system::python::w_python;
static std::once_flag flag1, flag2;

BOOST_AUTO_TEST_CASE(python_simple_script_test) {
  constexpr auto _test_name = "'python_simple_script_test'";

  //TODO(pooya): we have memory leak in python, so we need to check it

  std::cout << "entering test case " << _test_name << std::endl;

  boost::leaf::try_handle_all(
      []() -> boost::leaf::result<void> {
        auto py = w_python(PYTHON_HOME);

        auto _requested_vars = std::unordered_map<std::string, std::any>();
        BOOST_LEAF_CHECK(py.run("print ('Hello Wolf from Python')", _requested_vars));
        return {};
      },
      [&](const w_trace &p_trace) {
        BOOST_ERROR(wolf::format("{} got an error : {}", _test_name, p_trace.to_string()));
      },
      [] { BOOST_ERROR(wolf::format("{} got an error", _test_name)); });

  std::cout << "leaving test case " << _test_name << std::endl;
}

BOOST_AUTO_TEST_CASE(python_get_variable_from_script_test) {
  constexpr auto _test_name = "'python_get_variable_from_script_test'";

  std::cout << "entering test case " << _test_name << std::endl;

  boost::leaf::try_handle_all(
      []() -> boost::leaf::result<void> {
        auto py = w_python(PYTHON_HOME);
        auto _requested_vars = std::unordered_map<std::string, std::any>{{"x", (int)0}};
        BOOST_LEAF_CHECK(py.run("x = 10", _requested_vars));
        auto x = std::any_cast<int>(_requested_vars["x"]);
        BOOST_CHECK(x == 10);

        return {};
      },
      [](const w_trace &p_trace) {
        BOOST_ERROR(wolf::format("{} got an error : {}", _test_name, p_trace.to_string()));
      },
      [] { BOOST_ERROR(wolf::format("{} got an error", _test_name)); });

  std::cout << "leaving test case " << _test_name << std::endl;
}

    //void apply_simple_script_from_file()
    //{
    //    std::cout << "Test case #2\n";
    //    python_wrapper::PythonRunner pr; // using object
    //    pr.run_simple_script_file("D:/script.py");
    //    python_wrapper::run_simple_script_from_file("D:/script.py"); // using global function
    //    std::cout << "Done\n";
    //}

    //void apply_run_function_from_script_file_and_get_result()
    //{
    //    std::cout << "Test case #3\n";
    //    python_wrapper::PythonRunner pr; // using object
    //    auto val = pr.run_function_from_script_file_and_get_result<int>("D:/simple_math.py", "return_42");
    //    std::cout << val << '\n';
    //   // python_wrapper::run_simple_script_from_file("D:/script.py"); // using global function
    //    std::cout << "Done\n";
    //}
    //void apply_call_function()
    //{
    //    namespace bp = boost::python;
    //    std::cout << "Test case #7\n";
    //    std::string script =
    //        "def get_and_return(a, b, c):  \n"
    //        "   return a, b, c \n"
    //        "def greet():\n"
    //        "   print('Hello ...........')";
    //    python_wrapper::PythonRunner pr; // using object
    //    pr.set_script(script);
    //    auto ret = pr.call_function("get_and_return", {}, 7, 2, 3.14);

    //    if (bp::len(ret) != 3)
    //        throw std::invalid_argument("bad");
    //    std::cout << bp::extract<int>(ret[0]) << '\t'
    //              << bp::extract<int>(ret[1]) << '\t'
    //              << bp::extract<double>(ret[2]) << '\n';

    //    std::cout << "Done\n";
    //}


#endif // WOLF_TEST && WOLF_SYSTEM_PYTHON
