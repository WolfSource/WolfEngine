#[cxx::bridge]
pub mod ffi {
    unsafe extern "C++" {
        include!("wolf/src/script/cxx/luaJIT/luaJIT.hpp");

        pub type lua_State;
        type lua_CFunction = super::lua_CFunction;

        type luaJIT;
        pub fn bind(self: &luaJIT, p_lua_func_name: String, p_lua_c_fun: lua_CFunction) -> ();
    }
}

#[repr(transparent)]
#[allow(non_camel_case_types)]
pub struct lua_CFunction(pub extern "C" fn(p_arg: ffi::lua_State) -> i32);
unsafe impl cxx::ExternType for lua_CFunction {
    type Id = cxx::type_id!("lua_CFunction");
    type Kind = cxx::kind::Trivial;
}

#[test]
fn test() {
    println!("luaJIT");
}
