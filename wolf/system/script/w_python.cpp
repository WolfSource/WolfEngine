#include "w_python.hpp"

using w_python = wolf::system::python::w_python;

w_python::w_python(const std::string_view& p_python_home_path) {
  auto _python_home_path = std::wstring(p_python_home_path.begin(), p_python_home_path.end());
  Py_SetPythonHome(_python_home_path.data());
  Py_Initialize();
}

w_python::~w_python() { _release(); }

void w_python::set_module(const std::string_view& p_module_name) {
  this->_module_name = p_module_name;
}

void w_python::set_dictionary(const std::string& p_dic) { this->_dictionary_name = p_dic; }

std::string w_python::get_script() const { return this->_script; }

boost::leaf::result<int> w_python::run(
    _In_ const std::string_view& p_script,
    std::unordered_map<std::string, std::any>& p_requested_vars) {
  if (p_script.empty()) {
    return W_FAILURE(std::errc::invalid_argument, "empty python script!");
  }

  this->_script = p_script;

  try {
    auto _main = boost::python::import("__main__");
    auto _global = boost::python::object(_main.attr("__dict__"));
    boost::python::exec(this->_script.c_str(), _global);

    for (auto& var : p_requested_vars) {
      if (var.first.empty()) {
        continue;
      }
      auto _value = _global[var.first];

      if (var.second.type() == typeid(bool)) {
        var.second = gsl::narrow_cast<bool>(boost::python::extract<bool>(_value));
      } else if (var.second.type() == typeid(int)) {
        var.second = gsl::narrow_cast<int>(boost::python::extract<int>(_value));
      } else if (var.second.type() == typeid(double)) {
        var.second = gsl::narrow_cast<double>(boost::python::extract<double>(_value));
      } else if (var.second.type() == typeid(std::string)) {
        var.second = gsl::narrow_cast<std::string>(boost::python::extract<std::string>(_value));
      } else {
        return W_FAILURE(std::errc::invalid_argument, "could not find variable: " + var.first +
                                                          " in python script: " + this->_script);
      }
    }
    return 0;

  } catch (boost::python::error_already_set const&) {
    return W_FAILURE(std::errc::operation_canceled,
                     "could not execute python code: " + this->_script +
                         " because: " + _parse_python_exception());
  } catch (...) {
    return W_FAILURE(std::errc::operation_canceled,
                     "could not execute python code: " + this->_script);
  }
}

void w_python::_release() { 
    
    Py_Finalize(); }

std::string w_python::_parse_python_exception() {
  PyObject *type_ptr = nullptr, *value_ptr = nullptr, *traceback_ptr = nullptr;
  // Fetch the exception info from the Python C API
  PyErr_Fetch(&type_ptr, &value_ptr, &traceback_ptr);

  namespace py = boost::python;

  // fallback error
  std::string ret("unfetchable python error");

  // If the fetch got a type pointer, parse the type into the exception string
  if (type_ptr != nullptr) {
    py::handle<> h_type(type_ptr);
    py::str type_pstr(h_type);
    // Extract the string from the boost::python object
    py::extract<std::string> e_type_pstr(type_pstr);
    // If a valid string extraction is available, use it
    //  otherwise use fallback
    if (e_type_pstr.check()) {
      ret = e_type_pstr();
    } else {
      ret = "unknown exception type";
    }
  }

  // Do the same for the exception value (the stringification of the exception)
  if (value_ptr != nullptr) {
    py::handle<> h_val(value_ptr);
    py::str a(h_val);
    py::extract<std::string> returned(a);
    if (returned.check()) {
      ret += ": " + returned();
    } else {
      ret += std::string(": unparseable python error: ");
    }
  }

  // parse lines from the traceback using the Python traceback module
  if (traceback_ptr != nullptr) {
    py::handle<> h_tb(traceback_ptr);

    // load the traceback module and the format_tb function
    py::object tb(py::import("traceback"));
    py::object fmt_tb(tb.attr("format_tb"));

    // call format_tb to get a list of traceback strings
    py::object tb_list(fmt_tb(h_tb));

    // join the traceback strings into a single string
    py::object tb_str(py::str("\n").join(tb_list));

    // extract the string, check the extraction, and fallback in necessary
    py::extract<std::string> returned(tb_str);
    if (returned.check()) {
      ret += ": " + returned();
    } else {
      ret += std::string(": unparseable python traceback");
    }
  }
  return ret;
}
