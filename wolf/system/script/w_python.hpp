#pragma once

#include <wolf.hpp>
#include <unordered_map>
#include <any>
#include <Python.h>
#include <boost/python.hpp>

namespace wolf::system::python {

//std::string load_py_file(const std::filesystem::path& /* python file */);
//
//void run_simple_script_from_file(const std::filesystem::path& /* python file */);
//template <typename T>
//[[nodiscard("No ignore return value!")]] T run_function_and_get_result(
//    const std::string& /* script */, const std::string& /* function name */);
//template <typename T>
//[[nodiscard("No ignore return value!")]] T run_function_from_file_script_and_get_result(
//    const std::filesystem::path& /* python file */, const std::string& /* function name */);
//template <typename... ARGS>
//[[nodiscard("No ignore return value!")]] boost::python::tuple call_function(
//    const std::string& /* function name */, const std::string& /* script */,
//    ARGS&&... /* arguments */);

///*
// * convert boost python tuple to standard tuple
// */
//template <typename... RETTYPE>
//std::tuple<RETTYPE...> convertBPTuple2StdTuple(const boost::python::tuple&);

// an wrapper over boost.python
class w_python {
 public:
  // default constructor
  W_API explicit w_python(const std::string_view& p_python_home_path);
  // default destructor
  W_API ~w_python();

  /*
   * set python module
   * @param p_module_name, the module name
   */
  W_API void set_module(const std::string_view& p_module_name);

  /*
   * set python dictionary
   * @param p_dic, the dictionary
   */
  W_API void set_dictionary(const std::string& p_dic);
  /*
   * get python script
   * @return the script
   */
  W_API std::string get_script() const;

  /*
   * run an inline python script
   * @param p_script: python script
   * @return 0 if success, otherwise return error code
   */
  W_API boost::leaf::result<int> run(_In_ const std::string_view& p_script,
                                     std::unordered_map<std::string, std::any>& p_requested_vars);

  ///*
  // * run a python script from a file: no input parameter, no output parameter
  // */
  // void run_simple_script_file(const std::filesystem::path& /* path file */);
  ///*
  // * run a function from inline python script: no input parameter, one output parameter
  // */
  // template <typename RETTYPE>
  //[[nodiscard]] RETTYPE run_function_and_get_result(
  //    const std::string& /* function name */, const std::string& /* script */ = {});
  ///*
  // * run a function from a script file : no input parameter, one output parameter
  // */
  // template <typename RETTYPE>
  //[[nodiscard]] RETTYPE run_function_from_script_file_and_get_result(
  //    const std::filesystem::path& path_file, const std::string& func_name);
  ///*
  // * run a function from inline python script: variadic number of input parameter, variadic number
  // * of output parameter
  // */
  // template <typename... ARGS>
  //[[nodiscard]] boost::python::tuple call_function(
  //    const std::string& func_name, const std::string& /* script */ = {}, ARGS&&... args);
  ///*
  // * run a function from a script file: variadic number of input parameter, variadic number of
  // * output parameter
  // */
  // template <typename... ARGS>
  //[[nodiscard]] boost::python::tuple call_function_from_script_file(
  //    const std::filesystem::path& path_file, const std::string& func_name, ARGS&&... args);
  ///*
  // * run a function from inline python script and return the value of single variable
  // */
  // template <typename T>
  //[[nodiscard]] T run_and_get_value(
  //    const std::string& /* variable name */, const std::string& /* script */ = {});

  ///*
  // * run a function from a script file and return the value of single variable
  // */
  // template <typename T>
  //[[nodiscard("No ignore return value!")]] T run_file_script_and_get_value(
  //    const std::filesystem::path& /* python file */, const std::string& /* variable name */);

 private:
  // copy constructor
  w_python(const w_python&) = delete;
  // copy operator
  w_python& operator=(const w_python&) = delete;

  void _release();
  static std::string _parse_python_exception();

  std::string _script;
  std::string _module_name = "__main__";
  std::string _dictionary_name = "__dict__";

  boost::python::pointer_wrapper<boost::python::api::object> _global =
      boost::python::ptr(boost::python::api::object());
};  // class w_python

// template <typename RETTYPE>
//[[nodiscard("No ignore return value!")]] RETTYPE w_python::run_function_and_get_result(
//     const std::string& func_name, const std::string& script) {
//   if (!script.empty())
//     script_ = script;
//   Py_Initialize();
//   namespace bp = boost::python;
//   RETTYPE val{};
//   try {
//     bp::object main = bp::import(module_name_.c_str());
//     bp::object global(main.attr(dictionary_name_.c_str()));
//     bp::object result = bp::exec(script_.c_str(), global, global);
//     bp::object func = global[func_name.c_str()];  // Create a reference to it.
//     val = bp::extract<RETTYPE>(func());
//     return val;
//   } catch (bp::error_already_set&) {
//     PyErr_Print();
//     throw;
// #ifdef USE_PY_FINALIZE
//     if (Py_FinalizeEx() < 0) {
//       PyErr_Print();
//       throw std::runtime_error{bad_python_finalization};
//     }
// #endif  // USE_PY_FINALIZE
//   }
// #ifdef USE_PY_FINALIZE
//   if (Py_FinalizeEx() < 0) {
//     PyErr_Print();
//     throw std::runtime_error{bad_python_finalization};
//   }
// #endif  // USE_PY_FINALIZE
//   return val;
// }
//
// template <typename RETTYPE>
//[[nodiscard("No ignore return value!")]] RETTYPE
// w_python::run_function_from_script_file_and_get_result(const std::filesystem::path& path_file,
//                                                        const std::string& func_name) {
//   script_ = load_py_file(path_file);
//   Py_Initialize();
//   namespace bp = boost::python;
//   RETTYPE ret{};
//   try {
//     bp::object main = bp::import(module_name_.c_str());
//     bp::object global(main.attr(dictionary_name_.c_str()));
//     std::wstring path_as_wide_str = path_file.c_str();
//     std::string path_as_str;
//     std::copy(path_as_wide_str.begin(), path_as_wide_str.end(), std::back_inserter(path_as_str));
//     bp::object result = bp::exec_file(path_as_str.c_str(), global, global);
//     bp::object func = global[func_name.c_str()];  // Create a reference to it.
//     ret = bp::extract<RETTYPE>(func());
//     return ret;
//   } catch (bp::error_already_set&) {
//     PyErr_Print();
//     throw;
// #ifdef USE_PY_FINALIZE
//     if (Py_FinalizeEx() < 0) {
//       PyErr_Print();
//       throw std::runtime_error{bad_python_finalization};
//     }
// #endif  // USE_PY_FINALIZE
//   }
// #ifdef USE_PY_FINALIZE
//   if (Py_FinalizeEx() < 0) {
//     PyErr_Print();
//     throw std::runtime_error{bad_python_finalization};
//   }
// #endif  // USE_PY_FINALIZE
//   return ret;
// }
//
// template <typename... ARGS>
//[[nodiscard]] boost::python::tuple w_python::call_function(const std::string& func_name,
//                                                                const std::string& script,
//                                                                ARGS&&... args) {
//   if (!script.empty())
//     script_ = script;
//
//   Py_Initialize();
//   namespace bp = boost::python;
//   bp::tuple ret{};
//   try {
//     bp::object main = bp::import(module_name_.c_str());
//     bp::object global(main.attr(dictionary_name_.c_str()));
//
//     bp::object result = bp::exec(script_.c_str(), global, global);
//     bp::object func = global[func_name.c_str()];  // Create a reference to it.
//     ret = bp::call<bp::tuple>(func.ptr(), args...);
//   } catch (bp::error_already_set&) {
//     PyErr_Print();
//     throw;
// #ifdef USE_PY_FINALIZE
//     if (Py_FinalizeEx() < 0) {
//       PyErr_Print();
//       throw std::runtime_error{bad_python_finalization};
//     }
// #endif  // USE_PY_FINALIZE
//   }
// #ifdef USE_PY_FINALIZE
//   if (Py_FinalizeEx() < 0) {
//     PyErr_Print();
//     throw std::runtime_error{bad_python_finalization};
//   }
// #endif  // USE_PY_FINALIZE
//   return ret;
// }
//
// template <typename... ARGS>
//[[nodiscard("No ignore return value!")]] boost::python::tuple
// w_python::call_function_from_script_file(const std::filesystem::path& path_file,
//                                              const std::string& func_name, ARGS&&... args) {
//   script_ = load_py_file(path_file);
//   Py_Initialize();
//   namespace bp = boost::python;
//   bp::tuple ret{};
//   try {
//     bp::object main = bp::import(module_name_.c_str());
//     bp::object global(main.attr(dictionary_name_.c_str()));
//     bp::object result = bp::exec_file(path_file.filename().c_str(), global, global);
//     bp::object func = global[func_name.c_str()];  // Create a reference to it.
//     ret = bp::extract<bp::tuple>(func());
//     return ret;
//   } catch (bp::error_already_set&) {
//     PyErr_Print();
//     throw;
// #ifdef USE_PY_FINALIZE
//     if (Py_FinalizeEx() < 0) {
//       PyErr_Print();
//       throw std::runtime_error{bad_python_finalization};
//     }
// #endif  // USE_PY_FINALIZE
//   }
// #ifdef USE_PY_FINALIZE
//   if (Py_FinalizeEx() < 0) {
//     PyErr_Print();
//     throw std::runtime_error{bad_python_finalization};
//   }
// #endif  // USE_PY_FINALIZE
//   return ret;
// }
//
// template <typename T>
//[[nodiscard("No ignore return value!")]] T w_python::run_file_script_and_get_value(
//     const std::filesystem::path& path_file, const std::string& var_name) {
//   script_ = load_py_file(path_file);
//   Py_Initialize();
//   namespace bp = boost::python;
//   try {
//     boost::python::object main = boost::python::import("__main__");
//     boost::python::object global(main.attr("__dict__"));
//     bp::object result = bp::exec_file(path_file.filename().c_str(), global, global);
//     T val = boost::python::extract<T>(global[var_name]);
//
//     return val;
//   } catch (bp::error_already_set&) {
//     PyErr_Print();
//     throw;
// #ifdef USE_PY_FINALIZE
//     if (Py_FinalizeEx() < 0) {
//       PyErr_Print();
//       throw std::runtime_error{bad_python_finalization};
//     }
// #endif  // USE_PY_FINALIZE
//   }
// #ifdef USE_PY_FINALIZE
//   if (Py_FinalizeEx() < 0) {
//     PyErr_Print();
//     throw std::runtime_error{bad_python_finalization};
//   }
// #endif  // USE_PY_FINALIZE
// }
//[[nodiscard("No ignore return value!")]] std::string load_py_file(
//     const std::filesystem::path& path_file) {
//   if (!std::filesystem::exists(path_file))
//     throw std::runtime_error{"bad filename!"};
//   auto path_file_as_str = path_file.c_str();
//   std::ifstream f{path_file_as_str};
//   std::string content;
//   char ch;
//   while (f.get(ch))
//     content.push_back(ch);
//
//   return content;
// }
//
////void run_simple_script(const std::string& script) {
////  w_python pr;
////  pr.set_script(script);
////  pr.run_simple_script();
////}
//
// void run_simple_script_from_file(const std::filesystem::path& python_file) {
//  w_python pr;
//  pr.run_simple_script_file(python_file);
//}
//
// template <typename RETTYPE>
//[[nodiscard("No ignore return value!")]] RETTYPE run_function_and_get_result(
//    const std::string& script, const std::string& func_name) {
//  w_python pr{script};
//  auto val = pr.run_function_and_get_result<RETTYPE>(func_name);
//  return val;
//}
//
// template <typename RETTYPE>
//[[nodiscard("No ignore return value!")]] RETTYPE run_function_from_file_script_and_get_result(
//    const std::filesystem::path& path_file, const std::string& func_name) {
//  w_python pr;
//  auto val = pr.run_function_from_script_file_and_get_result<RETTYPE>(path_file, func_name);
//  return val;
//}
//
// template <typename... ARGS>
//[[nodiscard("No ignore return value!")]] boost::python::tuple call_function(
//    const std::string& func_name, const std::string& script, ARGS&&... args) {
//  w_python pr;
//  auto val = pr.call_function(func_name, script, args...);
//  return val;
//}
//
// template <typename... ARGS>
//[[nodiscard("No ignore return value!")]] boost::python::tuple call_function_from_script_file(
//    const std::filesystem::path& path_file, const std::string& func_name, ARGS&&... args) {
//  w_python pr;
//  auto val = pr.call_function(path_file, func_name, args...);
//  return val;
//}
//
// template <typename T>
//[[nodiscard("No ignore return value!")]] T run_and_get_value(const std::string& var_name,
//                                                             const std::string& script) {
//  w_python pr;
//  auto val = pr.run_and_get_value<T>(var_name, script);
//  return val;
//}
//
// template <typename T>
//[[nodiscard("No ignore return value!")]] T run_file_script_and_get_value(
//    const std::filesystem::path& path_file, const std::string& var_name) {
//  w_python pr;
//  auto val = pr.run_file_script_and_get_value<T>(path_file, var_name);
//  return val;
//}
//
// template <typename... RETTYPE>
// std::tuple<RETTYPE...> convertBPTuple2StdTuple(const boost::python::tuple& pt) {
//  std::tuple<RETTYPE...> t;
//  for (int i = 0; i < boost::python::len(pt); ++i) {
//    auto a = boost::python::extract<decltype(pt[i])>(pt[i]);
//    std::tuple_cat(t, a);
//  }
//
//  return t;
//}

}  // namespace wolf::system::python
