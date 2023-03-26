#pragma once

#if defined(WOLF_TEST) && defined(WOLF_SYSTEM_LUAJIT)

#include <wolf.hpp>
#include <system/w_leak_detector.hpp>
#include <boost/test/included/unit_test.hpp>

#include <system/luajit/w_lua.hpp>

using w_lua = wolf::system::luajit::w_lua;
using w_lib = wolf::system::luajit::w_lib;


BOOST_AUTO_TEST_CASE(lua_run_from_buffer_test) {
  boost::leaf::try_handle_all(
          []() -> boost::leaf::result<void> {
              w_lua lua;
              lua.open_libraries(w_lib::base);

              BOOST_LEAF_AUTO(result, lua.run_from_buffer("a = 'hello world'"));

              BOOST_CHECK(true);
              return {};
          },
          [](const w_trace &p_trace) {
              BOOST_ERROR("lua_test got an error: " + p_trace.to_string());
          },
          [] {
              BOOST_ERROR("lua_test got an error");
          });
}

BOOST_AUTO_TEST_CASE(lua_run_from_buffer_bad_script_test) {
  boost::leaf::try_handle_all(
          []() -> boost::leaf::result<void> {
              w_lua lua;
              lua.open_libraries(w_lib::base);

              BOOST_LEAF_AUTO(result, lua.run_from_buffer("bad script"));

              BOOST_ERROR("running bad scripts must return error");
              return {};
          },
          [](const w_trace &p_trace) {
              BOOST_CHECK(true);
          },
          [] {
              BOOST_CHECK(true);
          });
}

BOOST_AUTO_TEST_CASE(lua_run_from_buffer_return_value_test) {
  boost::leaf::try_handle_all(
          []() -> boost::leaf::result<void> {
              w_lua lua;
              lua.open_libraries(w_lib::base);

              auto result = lua.run_from_buffer<int, std::string>("return 1234, 'lua'");
              if (!result) {
                return result.error();
              }

              auto [first, second] = result.value();

              BOOST_REQUIRE(first == 1234);
              BOOST_REQUIRE(second == "lua");

              return {};
          },
          [](const w_trace &p_trace) {
              BOOST_ERROR("lua_test got an error: " + p_trace.to_string());
          },
          [] {
              BOOST_ERROR("lua_test got an error");
          });
}

BOOST_AUTO_TEST_CASE(lua_call_function_one_return_value_test) {
  boost::leaf::try_handle_all(
          []() -> boost::leaf::result<void> {
              w_lua lua;
              lua.open_libraries(w_lib::base);

              BOOST_LEAF_CHECK(lua.run_from_buffer("function f (a, b, c, d) return a + b + c + d end"));
              BOOST_LEAF_AUTO(result, lua.call_function<int>("f", 1, 2, 3, 4));
              auto value = std::get<0>(result);

              BOOST_REQUIRE(value == 10);
              return {};
          },
          [](const w_trace &p_trace) {
              BOOST_ERROR("lua_test got an error: " + p_trace.to_string());
          },
          [] {
              BOOST_ERROR("lua_test got an error");
          });
}

BOOST_AUTO_TEST_CASE(lua_call_function_two_return_value_test) {
  boost::leaf::try_handle_all(
          []() -> boost::leaf::result<void> {
              w_lua lua;
              lua.open_libraries(w_lib::base);

              BOOST_LEAF_CHECK(lua.run_from_buffer("function f (a, b, c, d) return a, b end"));
              auto f = lua.call_function<int, int>("f", 1, 2, 3, 4);
              if (!f) {
                return f.error();
              }
              auto [first, second] = f.value();
              BOOST_REQUIRE(first == 1);
              BOOST_REQUIRE(second == 2);

              return {};
          },
          [](const w_trace &p_trace) {
              BOOST_ERROR("lua_test got an error: " + p_trace.to_string());
          },
          [] {
              BOOST_ERROR("lua_test got an error");
          });
}

BOOST_AUTO_TEST_CASE(lua_call_function_nonexistent_test) {
  boost::leaf::try_handle_all(
          []() -> boost::leaf::result<void> {
              w_lua lua;
              lua.open_libraries(w_lib::base);

              BOOST_LEAF_AUTO(result, lua.call_function<int>("f", "some_value"));

              BOOST_ERROR("calling nonexistent functions must return error");
              return {};
          },
          [](const w_trace &p_trace) {
              BOOST_CHECK(true);
          },
          [] {
              BOOST_CHECK(true);
          });
}

BOOST_AUTO_TEST_CASE(lua_call_function_unmatched_types_test) {
  boost::leaf::try_handle_all(
          []() -> boost::leaf::result<void> {
              w_lua lua;
              lua.open_libraries(w_lib::base);

              BOOST_LEAF_CHECK(lua.run_from_buffer("function f () return 'hello world' end"));
              BOOST_LEAF_AUTO(result, lua.call_function<int>("f"));

              BOOST_ERROR("calling functions with mismatched return types must return error");

              return {};
          },
          [](const w_trace &p_trace) {
              BOOST_CHECK(true);
          },
          [] {
              BOOST_CHECK(true);
          });
}

BOOST_AUTO_TEST_CASE(lua_bind_to_function_lambda_test) {
  boost::leaf::try_handle_all(
          []() -> boost::leaf::result<void> {
              w_lua lua;
              lua.open_libraries(w_lib::base);

              auto lambda = []() { return 1234; };
              BOOST_LEAF_CHECK(lua.bind_to_function("cpp_function", lambda));

              BOOST_LEAF_AUTO(result, lua.call_function<int>("cpp_function"));
              auto value = std::get<0>(result);

              BOOST_REQUIRE(value == 1234);
              return {};
          },
          [](const w_trace &p_trace) {
              BOOST_ERROR("lua_test got an error: " + p_trace.to_string());
          },
          [] {
              BOOST_ERROR("lua_test got an error");
          });
}

int free_function() {
  return 1234;
}

BOOST_AUTO_TEST_CASE(lua_bind_to_function_free_test) {
  boost::leaf::try_handle_all(
          []() -> boost::leaf::result<void> {
              w_lua lua;
              lua.open_libraries(w_lib::base);

              BOOST_LEAF_CHECK(lua.bind_to_function("cpp_function", free_function));
              BOOST_LEAF_AUTO(result, lua.call_function<int>("cpp_function"));
              auto value = std::get<0>(result);

              BOOST_REQUIRE(value == 1234);
              return {};
          },
          [](const w_trace &p_trace) {
              BOOST_ERROR("lua_test got an error: " + p_trace.to_string());
          },
          [] {
              BOOST_ERROR("lua_test got an error");
          });
}

struct bind_member_function {
    int member() {
      return 1234;
    }
};

BOOST_AUTO_TEST_CASE(lua_bind_to_function_member_test) {
  boost::leaf::try_handle_all(
          []() -> boost::leaf::result<void> {
              w_lua lua;
              lua.open_libraries(w_lib::base);

              bind_member_function var;
              BOOST_LEAF_CHECK(lua.bind_to_function("cpp_function", &bind_member_function::member, var));

              BOOST_LEAF_AUTO(result, lua.call_function<int>("cpp_function"));
              auto value = std::get<0>(result);

              BOOST_REQUIRE(value == 1234);
              return {};
          },
          [](const w_trace &p_trace) {
              BOOST_ERROR("lua_test got an error: " + p_trace.to_string());
          },
          [] {
              BOOST_ERROR("lua_test got an error");
          });
}

BOOST_AUTO_TEST_CASE(lua_load_file_nonexistent_test) {
  boost::leaf::try_handle_all(
          []() -> boost::leaf::result<void> {
              w_lua lua;
              lua.open_libraries(w_lib::base);

              BOOST_LEAF_CHECK(lua.load_file("/no_such_file.lua"));

              BOOST_ERROR("loading nonexistent files must return error");
              return {};
          },
          [](const w_trace &p_trace) {
              BOOST_CHECK(true);
          },
          [] {
              BOOST_CHECK(true);
          });
}

BOOST_AUTO_TEST_CASE(lua_run_without_load_test) {
  boost::leaf::try_handle_all(
          []() -> boost::leaf::result<void> {
              w_lua lua;
              lua.open_libraries(w_lib::base);

              BOOST_LEAF_AUTO(result, lua.run());

              BOOST_ERROR("running scripts without loading one must return error");
              return {};
          },
          [](const w_trace &p_trace) {
              BOOST_CHECK(true);
          },
          [] {
              BOOST_CHECK(true);
          });
}

BOOST_AUTO_TEST_CASE(lua_load_run_test) {
  boost::leaf::try_handle_all(
          []() -> boost::leaf::result<void> {
              w_lua lua;
              lua.open_libraries(w_lib::base);

              BOOST_LEAF_CHECK(lua.load_from_buffer("a = 'hello world'"));
              BOOST_LEAF_AUTO(result, lua.run());

              BOOST_CHECK(true);
              return {};
          },
          [](const w_trace &p_trace) {
              BOOST_ERROR("lua_test got an error: " + p_trace.to_string());
          },
          [] {
              BOOST_ERROR("lua_test got an error");
          });
}

BOOST_AUTO_TEST_CASE(lua_load_run_file_test) {
  boost::leaf::try_handle_all(
          []() -> boost::leaf::result<void> {
              w_lua lua;
              lua.open_libraries(w_lib::base);

              std::filesystem::path file("./test.lua");
              std::ofstream ofs(file);
              ofs << "return 'hello world'" << std::endl;
              ofs.close();

              BOOST_LEAF_CHECK(lua.load_file(file));
              BOOST_LEAF_AUTO(result, lua.run<std::string>());

              BOOST_REQUIRE(std::get<0>(result) == "hello world");

              return {};
          },
          [](const w_trace &p_trace) {
              BOOST_ERROR("lua_test got an error: " + p_trace.to_string());
          },
          [] {
              BOOST_ERROR("lua_test got an error");
          });
}


BOOST_AUTO_TEST_CASE(lua_load_run_multiple_test) {
  boost::leaf::try_handle_all(
          []() -> boost::leaf::result<void> {
              w_lua lua;
              lua.open_libraries(w_lib::base);

              BOOST_LEAF_CHECK(lua.load_from_buffer("a = 'hello world'"));
              BOOST_LEAF_AUTO(result1, lua.run());
              BOOST_LEAF_AUTO(result2, lua.run());
              BOOST_LEAF_AUTO(result3, lua.run());

              BOOST_CHECK(true);
              return {};
          },
          [](const w_trace &p_trace) {
              BOOST_ERROR("lua_test got an error: " + p_trace.to_string());
          },
          [] {
              BOOST_ERROR("lua_test got an error");
          });
}

BOOST_AUTO_TEST_CASE(lua_load_run_return_test) {
  boost::leaf::try_handle_all(
          []() -> boost::leaf::result<void> {
              w_lua lua;
              lua.open_libraries(w_lib::base);

              BOOST_LEAF_CHECK(lua.load_from_buffer("return 'hello world'"));
              BOOST_LEAF_AUTO(result, lua.run<std::string>());
              auto value = std::get<0>(result);

              BOOST_REQUIRE(value == "hello world");
              return {};
          },
          [](const w_trace &p_trace) {
              BOOST_ERROR("lua_test got an error: " + p_trace.to_string());
          },
          [] {
              BOOST_ERROR("lua_test got an error");
          });
}

BOOST_AUTO_TEST_CASE(lua_load_run_params_test) {
  boost::leaf::try_handle_all(
          []() -> boost::leaf::result<void> {
              w_lua lua;
              lua.open_libraries(w_lib::base);

              BOOST_LEAF_CHECK(lua.load_from_buffer("local a,b = ... return a,b"));

              auto run_result = lua.run<std::string, int>("lua", 1234);
              if (!run_result) {
                return run_result.error();
              }
              auto [first, second] = run_result.value();
              BOOST_REQUIRE(first == "lua");
              BOOST_REQUIRE(second == 1234);
              return {};
          },
          [](const w_trace &p_trace) {
              BOOST_ERROR("lua_test got an error: " + p_trace.to_string());
          },
          [] {
              BOOST_ERROR("lua_test got an error");
          });
}

BOOST_AUTO_TEST_CASE(lua_set_get_global_vars_test) {
  boost::leaf::try_handle_all(
          []() -> boost::leaf::result<void> {
              w_lua lua;
              lua.open_libraries(w_lib::base);

              BOOST_LEAF_CHECK(lua.set_global_variable("string_var", "hello world"));
              BOOST_LEAF_CHECK(lua.set_global_variable("int_var", 1234));

              std::string string_var;
              int int_var;

              BOOST_LEAF_CHECK(lua.get_global_variable("string_var", string_var));
              BOOST_REQUIRE(string_var == "hello world");

              BOOST_LEAF_CHECK(lua.get_global_variable("int_var", int_var));
              BOOST_REQUIRE(int_var == 1234);
              return {};
          },
          [](const w_trace &p_trace) {
              BOOST_ERROR("lua_test got an error: " + p_trace.to_string());
          },
          [] {
              BOOST_ERROR("lua_test got an error");
          });
}

BOOST_AUTO_TEST_CASE(lua_get_state_test) {
  boost::leaf::try_handle_all(
          []() -> boost::leaf::result<void> {
              w_lua lua;
              BOOST_LEAF_ASSIGN(auto &state, lua.get_state());

              BOOST_CHECK(true);
              return {};
          },
          [](const w_trace &p_trace) {
              BOOST_ERROR("lua_test got an error: " + p_trace.to_string());
          },
          [] {
              BOOST_ERROR("lua_test got an error");
          });
}

#endif
