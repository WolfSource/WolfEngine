#pragma once

#include <Python.h>
#include <string>
#include <boost/python.hpp>
#include <stdexcept>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <tuple>
#include <string>
#include <iterator>
namespace python_wrapper {
#ifdef USE_PY_FINALIZE
#undef USE_PY_FINALIZE
#endif
std::string load_py_file(const std::filesystem::path& /* python file */);
void run_simple_script(const std::string& /* script */);
void run_simple_script_from_file(const std::filesystem::path& /* python file */);
template<typename T> [[nodiscard("No ignore return value!")]]
T run_function_and_get_result(const std::string& /* script */, const std::string& /* function name */);
template<typename T> [[nodiscard("No ignore return value!")]]
T run_function_from_file_script_and_get_result(const std::filesystem::path& /* python file */,
                                               const std::string& /* function name */);
template<typename... ARGS> [[nodiscard("No ignore return value!")]]
boost::python::tuple call_function(const std::string& /* function name */,
                                   const std::string& /* script */,
                                   ARGS&&... /* arguments */);
/*
 * helper functions
 */
/*
 * convert boost python tuple to standard tuple
 */
template<typename... RETTYPE>
std::tuple<RETTYPE...> convertBPTuple2StdTuple(const boost::python::tuple&);

const std::string default_module_name{"__main__"};
const std::string default_dictionary_name{"__dict__"};
const std::string bad_python_script{"Bad python script! Maybe syntax error"};
const std::string bad_filename{"Bad filename! File not found"};
#ifdef USE_PY_FINALIZE
const std::string bad_python_finalization{"Python finalization failed"};
#endif // USE_PY_FINALIZE
class PythonRunner { // a class wrapper used by Boost.Python
    std::string script_; // python script
    std::string module_name_;
    std::string dictionary_name_;
    /*
     * prohibit copy operations
     */
    PythonRunner(const PythonRunner&) =delete;
    PythonRunner& operator=(const PythonRunner&) =delete;
public:
    ~PythonRunner() =default;
    PythonRunner() : script_{},
                     module_name_{default_module_name},
                     dictionary_name_{default_dictionary_name}
    {
    }
    explicit PythonRunner(const std::string& s,
                          const std::string& m = default_module_name,
                          const std::string& d = default_dictionary_name) :
        script_(s), module_name_{m}, dictionary_name_{d}
    {
    }
    std::string script() const
    {
        return script_;
    }
    PythonRunner& set_script(const std::string& s)
    {
        script_ = s;
        return *this;
    }

    PythonRunner& set_module(const std::string& m)
    {
        module_name_ = m;
        return *this;
    }

    PythonRunner& set_dictionary(const std::string& d)
    {
        dictionary_name_ = d;
        return *this;
    }
    /**
     * run simple python script: no input, no output
     */
    /*
     * run an inline python script: no input parameter, no output parameter
     */
    void run_simple_script(const std::string& /* script */ = {});
    /*
     * run a python script from a file: no input parameter, no output parameter
     */
    void run_simple_script_file(const std::filesystem::path& /* path file */);
    /*
     * run a function from inline python script: no input parameter, one output parameter
     */
    template<typename RETTYPE> [[nodiscard("No ignore return value!")]]
    RETTYPE run_function_and_get_result(const std::string& /* function name */,
                                         const std::string& /* script */ = {});
    /*
     * run a function from a script file : no input parameter, one output parameter
     */
    template<typename RETTYPE> [[nodiscard("No ignore return value!")]]
    RETTYPE run_function_from_script_file_and_get_result(const std::filesystem::path& path_file,
                                                         const std::string& func_name);
    /*
     * run a function from inline python script: variadic number of input parameter, variadic number of output parameter
     */
    template<typename... ARGS>
    [[nodiscard("No ignore return value!")]]
    boost::python::tuple call_function(const std::string& func_name,
                                       const std::string& /* script */ = {},
                                       ARGS&&... args);
    /*
     * run a function from a script file: variadic number of input parameter, variadic number of output parameter
     */
    template<typename... ARGS>
    [[nodiscard("No ignore return value!")]]
    boost::python::tuple call_function_from_script_file(const std::filesystem::path& path_file,
                                                        const std::string& func_name,
                                                        ARGS&&... args);
    /*
     * run a function from inline python script and return the value of single variable
     */
    template<typename T>
    [[nodiscard("No ignore return value!")]]
    T run_and_get_value(const std::string& /* variable name */,
                        const std::string& /* script */ = {});

    /*
     * run a function from a script file and return the value of single variable
     */
    template<typename T>
    [[nodiscard("No ignore return value!")]]
    T run_file_script_and_get_value(const std::filesystem::path& /* python file */,
                                    const std::string& /* variable name */);
}; // of PythonRunner

void PythonRunner::run_simple_script(const std::string& script)
{
    if (!script.empty())
        script_ = script;
    Py_Initialize();
    auto run_return_val = PyRun_SimpleString(script_.c_str());
    if (run_return_val < 0)
        throw std::runtime_error{bad_python_script};
#ifdef USE_PY_FINALIZE
    if (Py_FinalizeEx() < 0) {
        throw std::runtime_error{bad_python_finalization};
    }
#endif // USE_PY_FINALIZE
}

void PythonRunner::run_simple_script_file(const std::filesystem::path &p_file)
{
    script_ = load_py_file(p_file);
    run_simple_script();
}

template<typename RETTYPE> [[nodiscard("No ignore return value!")]]
RETTYPE PythonRunner::run_function_and_get_result(const std::string& func_name,
                                            const std::string& script)
{
    if (!script.empty())
        script_ = script;
    Py_Initialize();
    namespace bp = boost::python;
    RETTYPE val{};
    try {
        bp::object main = bp::import(module_name_.c_str());
        bp::object global(main.attr(dictionary_name_.c_str()));
        bp::object result = bp::exec(script_.c_str(), global, global);
        bp::object func = global[func_name.c_str()]; // Create a reference to it.
        val = bp::extract<RETTYPE>(func());
        return val;
    }
    catch(bp::error_already_set&)
    {
        PyErr_Print();
        throw;
#ifdef USE_PY_FINALIZE
        if (Py_FinalizeEx() < 0) {
            PyErr_Print();
            throw std::runtime_error{bad_python_finalization};
        }
#endif // USE_PY_FINALIZE
    }
#ifdef USE_PY_FINALIZE
    if (Py_FinalizeEx() < 0) {
        PyErr_Print();
        throw std::runtime_error{bad_python_finalization};
    }
#endif // USE_PY_FINALIZE
    return val;
}

template<typename RETTYPE> [[nodiscard("No ignore return value!")]]
RETTYPE PythonRunner::run_function_from_script_file_and_get_result(const std::filesystem::path& path_file,
                                                             const std::string& func_name)
{
    script_ = load_py_file(path_file);
    Py_Initialize();
    namespace bp = boost::python;
    RETTYPE ret{};
    try {
        bp::object main = bp::import(module_name_.c_str());
        bp::object global(main.attr(dictionary_name_.c_str()));
        std::wstring path_as_wide_str = path_file.c_str();
        std::string path_as_str;
        std::copy(path_as_wide_str.begin(), path_as_wide_str.end(), std::back_inserter(path_as_str));
        bp::object result = bp::exec_file(path_as_str.c_str(), global, global);
        bp::object func = global[func_name.c_str()]; // Create a reference to it.
        ret = bp::extract<RETTYPE>(func());
        return ret;
    }
    catch(bp::error_already_set&)
    {
        PyErr_Print();
        throw;
#ifdef USE_PY_FINALIZE
        if (Py_FinalizeEx() < 0) {
            PyErr_Print();
            throw std::runtime_error{bad_python_finalization};
        }
#endif // USE_PY_FINALIZE
    }
#ifdef USE_PY_FINALIZE
    if (Py_FinalizeEx() < 0) {
        PyErr_Print();
        throw std::runtime_error{bad_python_finalization};
    }
#endif // USE_PY_FINALIZE
    return ret;
}

template<typename... ARGS>
[[nodiscard]] boost::python::tuple PythonRunner::call_function(
        const std::string& func_name, const std::string& script, ARGS&&... args)
{
    if (!script.empty())
        script_ = script;

    Py_Initialize();
    namespace bp = boost::python;
    bp::tuple ret{};
    try {
        bp::object main = bp::import(module_name_.c_str());
        bp::object global(main.attr(dictionary_name_.c_str()));

        bp::object result = bp::exec(script_.c_str(), global, global);
        bp::object func = global[func_name.c_str()]; // Create a reference to it.
        ret = bp::call<bp::tuple>(func.ptr(), args...);
    }
    catch(bp::error_already_set&)
    {
        PyErr_Print();
        throw;
#ifdef USE_PY_FINALIZE
        if (Py_FinalizeEx() < 0) {
            PyErr_Print();
            throw std::runtime_error{bad_python_finalization};
        }
#endif // USE_PY_FINALIZE
    }
#ifdef USE_PY_FINALIZE
    if (Py_FinalizeEx() < 0) {
        PyErr_Print();
        throw std::runtime_error{bad_python_finalization};
    }
#endif // USE_PY_FINALIZE
    return ret;
}

template<typename... ARGS>
[[nodiscard("No ignore return value!")]]
boost::python::tuple PythonRunner::call_function_from_script_file(const std::filesystem::path& path_file,
                                                                  const std::string& func_name,
                                                                  ARGS&&... args)
{
    script_ = load_py_file(path_file);
    Py_Initialize();
    namespace bp = boost::python;
    bp::tuple ret{};
    try {
        bp::object main = bp::import(module_name_.c_str());
        bp::object global(main.attr(dictionary_name_.c_str()));
        bp::object result = bp::exec_file(path_file.filename().c_str(), global, global);
        bp::object func = global[func_name.c_str()]; // Create a reference to it.
        ret = bp::extract<bp::tuple>(func());
        return ret;
    }
    catch(bp::error_already_set&)
    {
        PyErr_Print();
        throw;
#ifdef USE_PY_FINALIZE
        if (Py_FinalizeEx() < 0) {
            PyErr_Print();
            throw std::runtime_error{bad_python_finalization};
        }
#endif // USE_PY_FINALIZE
    }
#ifdef USE_PY_FINALIZE
    if (Py_FinalizeEx() < 0) {
        PyErr_Print();
        throw std::runtime_error{bad_python_finalization};
    }
#endif // USE_PY_FINALIZE
    return ret;
}

template<typename T>
[[nodiscard("No ignore return value!")]]
T PythonRunner::run_and_get_value(const std::string& var_name, const std::string& script)
{
    if (!script.empty())
        script_ = script;

    Py_Initialize();
    namespace bp = boost::python;
    try {
        boost::python::object main = boost::python::import("__main__");
        boost::python::object global(main.attr("__dict__"));
        boost::python::exec(script_.c_str(), global);
        T val = boost::python::extract<T>(global[var_name]);

        return val;
    }
    catch(bp::error_already_set&)
    {
        PyErr_Print();
        throw;
#ifdef USE_PY_FINALIZE
        if (Py_FinalizeEx() < 0) {
            PyErr_Print();
            throw std::runtime_error{bad_python_finalization};
        }
#endif // USE_PY_FINALIZE
    }
#ifdef USE_PY_FINALIZE
    if (Py_FinalizeEx() < 0) {
        PyErr_Print();
        throw std::runtime_error{bad_python_finalization};
    }
#endif // USE_PY_FINALIZE
}

template<typename T>
[[nodiscard("No ignore return value!")]]
T PythonRunner::run_file_script_and_get_value(const std::filesystem::path& path_file,
                                              const std::string& var_name)
{
    script_ = load_py_file(path_file);
    Py_Initialize();
    namespace bp = boost::python;
    try {
        boost::python::object main = boost::python::import("__main__");
        boost::python::object global(main.attr("__dict__"));
        bp::object result = bp::exec_file(path_file.filename().c_str(), global, global);
        T val = boost::python::extract<T>(global[var_name]);

        return val;
    }
    catch(bp::error_already_set&)
    {
        PyErr_Print();
        throw;
#ifdef USE_PY_FINALIZE
        if (Py_FinalizeEx() < 0) {
            PyErr_Print();
            throw std::runtime_error{bad_python_finalization};
        }
#endif // USE_PY_FINALIZE
    }
#ifdef USE_PY_FINALIZE
    if (Py_FinalizeEx() < 0) {
        PyErr_Print();
        throw std::runtime_error{bad_python_finalization};
    }
#endif // USE_PY_FINALIZE
}
[[nodiscard("No ignore return value!")]]
std::string load_py_file(const std::filesystem::path& path_file)
{
    if (!std::filesystem::exists(path_file))
        throw std::runtime_error{"bad filename!"};
    auto path_file_as_str = path_file.c_str();
    std::ifstream f{path_file_as_str};
    std::string content;
    char ch;
    while (f.get(ch))
        content.push_back(ch);

    return content;
}

void run_simple_script(const std::string& script)
{
    PythonRunner pr;
    pr.set_script(script);
    pr.run_simple_script();
}

void run_simple_script_from_file(const std::filesystem::path& python_file)
{
    PythonRunner pr;
    pr.run_simple_script_file(python_file);
}

template<typename RETTYPE>
[[nodiscard("No ignore return value!")]] RETTYPE run_function_and_get_result(const std::string& script, const std::string& func_name)
{
    PythonRunner pr{script};
    auto val = pr.run_function_and_get_result<RETTYPE>(func_name);
    return val;
}

template<typename RETTYPE>
[[nodiscard("No ignore return value!")]] RETTYPE run_function_from_file_script_and_get_result(const std::filesystem::path& path_file,
                                                             const std::string& func_name)
{
    PythonRunner pr;
    auto val = pr.run_function_from_script_file_and_get_result<RETTYPE>(path_file, func_name);
    return val;
}

template<typename... ARGS>
[[nodiscard("No ignore return value!")]]
boost::python::tuple call_function(const std::string& func_name,
                                   const std::string& script,
                                   ARGS&&... args)
{
    PythonRunner pr;
    auto val = pr.call_function(func_name, script, args...);
    return val;
}

template<typename... ARGS>
[[nodiscard("No ignore return value!")]]
boost::python::tuple call_function_from_script_file(const std::filesystem::path& path_file,
                                                    const std::string& func_name,
                                                    ARGS&&... args)
{
    PythonRunner pr;
    auto val = pr.call_function(path_file, func_name, args...);
    return val;
}

template<typename T>
[[nodiscard("No ignore return value!")]]
T run_and_get_value(const std::string& var_name, const std::string& script)
{
    PythonRunner pr;
    auto val = pr.run_and_get_value<T>(var_name, script);
    return val;
}

template<typename T>
[[nodiscard("No ignore return value!")]]
T run_file_script_and_get_value(const std::filesystem::path& path_file,
                                              const std::string& var_name)
{
    PythonRunner pr;
    auto val = pr.run_file_script_and_get_value<T>(path_file, var_name);
    return val;
}

template<typename... RETTYPE>
std::tuple<RETTYPE...> convertBPTuple2StdTuple(const boost::python::tuple& pt)
{
    std::tuple<RETTYPE...> t;
    for (int i = 0; i < boost::python::len(pt); ++i) {
        auto a = boost::python::extract<decltype(pt[i])>(pt[i]);
        std::tuple_cat(t, a);
    }

    return t;
}

} // of python_wrapper
