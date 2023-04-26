#pragma once

#include <system/script/w_python.hpp>
#include <iostream>
#include <boost/python.hpp>

namespace python_testcase {
    void apply_simple_script()
    {
        std::cout << "Test case #1 ...\n";
        python_wrapper::PythonRunner pr; // using object
        pr.run_simple_script();
        python_wrapper::run_simple_script("print ('Hello, Boost.python')"); // using global function
        std::cout << "Done\n";
    }
    void apply_simple_script_from_file()
    {
        std::cout << "Test case #2\n";
        python_wrapper::PythonRunner pr; // using object
        pr.run_simple_script_file("D:/script.py");
        python_wrapper::run_simple_script_from_file("D:/script.py"); // using global function
        std::cout << "Done\n";
    }

    void apply_run_function_from_script_file_and_get_result()
    {
        std::cout << "Test case #3\n";
        python_wrapper::PythonRunner pr; // using object
        auto val = pr.run_function_from_script_file_and_get_result<int>("D:/simple_math.py", "return_42");
        std::cout << val << '\n';
       // python_wrapper::run_simple_script_from_file("D:/script.py"); // using global function
        std::cout << "Done\n";
    }
    void apply_call_function()
    {
        namespace bp = boost::python;
        std::cout << "Test case #7\n";
        std::string script =
            "def get_and_return(a, b, c):  \n"
            "   return a, b, c \n"
            "def greet():\n"
            "   print('Hello ...........')";
        python_wrapper::PythonRunner pr; // using object
        pr.set_script(script);
        auto ret = pr.call_function("get_and_return", {}, 7, 2, 3.14);

        if (bp::len(ret) != 3)
            throw std::invalid_argument("bad");
        std::cout << bp::extract<int>(ret[0]) << '\t'
                  << bp::extract<int>(ret[1]) << '\t'
                  << bp::extract<double>(ret[2]) << '\n';

        std::cout << "Done\n";
    }

    // Note: add required test case here
} // of python_testcase
