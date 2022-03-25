use anyhow::Ok;
use luajit::ffi::{
    lauxlib::luaL_loadbuffer,
    luajit::{
        lua_State, lua_close, lua_newstate, lua_pcall, lua_pushcclosure, lua_setfield, size_t,
        LUA_GLOBALSINDEX, LUA_MULTRET,
    },
    lualib::luaL_openlibs,
};
use std::{
    alloc::{self, Layout},
    ffi::c_void,
    mem,
    os::raw::c_ulong,
    ptr::{self},
    sync::Once,
};
use wolf_macros::STR_i8_PTR;

static ONCE_LOAD_LIBS: Once = Once::new();

#[inline(always)]
unsafe extern "C" fn lua_allocator(
    _ud: *mut c_void,
    ptr: *mut c_void,
    osize: size_t,
    nsize: size_t,
) -> *mut c_void {
    const SIZE_OF_USIZE: usize = mem::size_of::<usize>();

    let new_size: usize = nsize.try_into().unwrap_or(0);
    let old_size: usize = osize.try_into().unwrap_or(0);

    let new_layout = Layout::from_size_align_unchecked(new_size, SIZE_OF_USIZE);
    let old_layout = Layout::from_size_align_unchecked(old_size, SIZE_OF_USIZE);

    if new_size == 0 {
        // free memory
        if !ptr.is_null() {
            alloc::dealloc(ptr as *mut u8, old_layout);
        }
        return ptr::null_mut();
    }

    // get old layout from osize
    if ptr.is_null() {
        alloc::alloc(new_layout) as *mut c_void
    } else {
        let allocated_ptr = alloc::realloc(ptr as *mut u8, old_layout, new_size) as *mut c_void;
        if new_size < old_size {
            // unexpected memory allocation
            println!("unexpected memory allocation happened in lua_allocator");
            alloc::handle_alloc_error(new_layout);
        }
        allocated_ptr
    }
}

#[allow(clippy::enum_clike_unportable_variant)]
#[derive(std::cmp::PartialEq)]
pub enum LuaReturnCode {
    LuaOk = 0,
    // 2 => a runtime error
    LuaErrRun = luajit::ffi::luajit::LUA_ERRRUN as isize,
    // 3 => syntax error during pre-compilation
    LuaErrSyntax = luajit::ffi::luajit::LUA_ERRSYNTAX as isize,
    // 4 => memory allocation error. For such errors, Lua does not call the error handler function
    LuaErrMem = luajit::ffi::luajit::LUA_ERRMEM as isize,
    // 5 => while running the error handler function
    LuaErrErr = luajit::ffi::luajit::LUA_ERRERR as isize,
    // isize::MAX => undefined error
    LuaUndefined = isize::MAX,
}

impl std::fmt::Debug for LuaReturnCode {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        match *self {
            LuaReturnCode::LuaOk => write!(f, "LuaOk"),
            LuaReturnCode::LuaErrRun => write!(f, "LuaErrRun"),
            LuaReturnCode::LuaErrSyntax => write!(f, "LuaErrSyntax"),
            LuaReturnCode::LuaErrMem => write!(f, "LuaErrMem"),
            LuaReturnCode::LuaErrErr => write!(f, "LuaErrErr"),
            LuaReturnCode::LuaUndefined => write!(f, "LuaUndefined"),
        }
    }
}

impl LuaReturnCode {
    fn from_lua_code(v: i32) -> Self {
        let i = v as u32;
        match i {
            luajit::ffi::luajit::LUA_OK => LuaReturnCode::LuaOk,
            luajit::ffi::luajit::LUA_ERRRUN => LuaReturnCode::LuaErrRun,
            luajit::ffi::luajit::LUA_ERRSYNTAX => LuaReturnCode::LuaErrSyntax,
            luajit::ffi::luajit::LUA_ERRMEM => LuaReturnCode::LuaErrMem,
            luajit::ffi::luajit::LUA_ERRERR => LuaReturnCode::LuaErrErr,
            _ => LuaReturnCode::LuaUndefined,
        }
    }
}

pub struct Lua {
    pub lua_state: *mut lua_State,
}

impl Drop for Lua {
    fn drop(&mut self) {
        unsafe {
            if !self.lua_state.is_null() {
                lua_close(self.lua_state);
            }
        }
    }
}

impl Lua {
    #[allow(clippy::new_without_default)]
    pub fn new() -> Self {
        // convert module name
        let lua_state_u = unsafe { lua_newstate(Some(lua_allocator), ptr::null_mut()) };
        let l = Self {
            lua_state: lua_state_u,
        };
        ONCE_LOAD_LIBS.call_once(|| unsafe {
            luaL_openlibs(l.lua_state);
        });
        l
    }

    #[allow(trivial_casts)]
    pub async fn load(
        &self,
        p_module_name: &str,
        p_source_code: &str,
    ) -> anyhow::Result<LuaReturnCode> {
        // check source code
        if p_source_code.is_empty() {
            let e = anyhow::anyhow!("lua source code was not provided").context("lua::load");
            return anyhow::Result::Err(e);
        }
        // convert module name from String to CString
        let (_, module_name) = STR_i8_PTR!(p_module_name, "lua::load p_module_name")?;
        let (len, source_code) = STR_i8_PTR!(p_source_code, "lua::load p_source_code")?;

        // return index
        let ret =
            unsafe { luaL_loadbuffer(self.lua_state, source_code, len as c_ulong, module_name) };
        Ok(LuaReturnCode::from_lua_code(ret))
    }

    pub async fn exec(&self) -> anyhow::Result<LuaReturnCode> {
        let ret = unsafe { lua_pcall(self.lua_state, 0, LUA_MULTRET, 0) };
        Ok(LuaReturnCode::from_lua_code(ret))
    }

    pub async fn bind<F>(
        &self,
        p_lua_fn_name: &str,
        p_callback: unsafe extern "C" fn(L: *mut lua_State) -> i32,
    ) -> anyhow::Result<()> {
        unsafe {
            lua_pushcclosure(self.lua_state, Some(p_callback), 0);
        }
        self.set_global(p_lua_fn_name).await
    }

    pub async fn set_global(&self, p_lua_fn_name: &str) -> anyhow::Result<()> {
        let (_len, name) = STR_i8_PTR!(p_lua_fn_name, "lua::set_global")?;
        unsafe {
            lua_setfield(self.lua_state, LUA_GLOBALSINDEX, name);
        }
        Ok(())
    }
}

// #[cxx::bridge]
// pub mod ffi {
//     #[repr(u8)]
//     #[derive(Debug, Clone)]
//     enum LuaType {
//         L_NIL = 0,
//         L_BOOLEAN,
//         L_LIGHTUSERDATA, //Not implemented yet
//         L_NUMBER,
//         L_STRING,
//         L_TABLE,
//         L_FUNCTION,
//         L_USERDATA, //Not implemented yet
//         L_THREAD,   //Not implemented yet
//     }

//     #[derive(Debug, Clone)]
//     pub struct LuaValue {
//         t: LuaType, //type
//         v: Vec<u8>, //value in bytes
//     }

//     unsafe extern "C++" {
//         include!("wolf_system/src/script/cxx/luaJIT/luaJIT.hpp");

//         //pub type c_void;
//         pub type lua_State;
//         type lua_CFunction = super::lua_CFunction;

//         pub type luaJIT;

//         #[must_use]
//         pub fn load(self: &luaJIT, p_module_name: &str, p_source_code: &str) -> i32;

//         #[must_use]
//         pub fn exec(self: &luaJIT) -> i32;

//         #[must_use]
//         pub fn bind(self: &luaJIT, p_lua_fn_name: &str, p_lua_c_fun: lua_CFunction) -> i32;

//         #[must_use]
//         pub fn call(
//             self: &luaJIT,
//             p_name: &str,
//             p_params: &[LuaValue],
//             p_results: &mut Vec<LuaValue>,
//         ) -> i32;

//         #[must_use]
//         pub fn get_global(self: &luaJIT, p_name: &str, p_value: &mut LuaValue) -> i32;

//         #[must_use]
//         pub fn set_global(self: &luaJIT, p_name: &str, p_value: &LuaValue) -> i32;

//         #[must_use]
//         pub fn set_lua_path(self: &luaJIT, p_env_path: &str) -> i32;

//         #[must_use]
//         pub fn New() -> UniquePtr<luaJIT>;
//     }
// }

// impl Default for ffi::LuaValue {
//     fn default() -> Self {
//         Self::new()
//     }
// }

// impl ffi::LuaValue {
//     #[allow(clippy::missing_const_for_fn)]
//     #[must_use]
//     pub fn new() -> Self {
//         Self {
//             t: ffi::LuaType::L_NIL,
//             v: Vec::<u8>::new(),
//         }
//     }

//     #[inline]
//     #[must_use]
//     pub const fn get_type(&self) -> ffi::LuaType {
//         self.t
//     }

//     #[inline]
//     #[must_use]
//     pub fn from_bool(p: bool) -> Self {
//         Self {
//             t: ffi::LuaType::L_BOOLEAN,
//             v: vec![u8::from(p); 1],
//         }
//     }

//     #[inline]
//     pub fn as_bool(&self) -> anyhow::Result<bool> {
//         const SIZE: usize = std::mem::size_of::<bool>();
//         if self.t == ffi::LuaType::L_BOOLEAN && self.v.len() == SIZE {
//             Ok(self.v[0] != 0)
//         } else {
//             anyhow::bail!("could not cast lua::ffi::LuaValue to bool")
//         }
//     }

//     #[inline]
//     #[must_use]
//     pub fn from_number(p: f64) -> Self {
//         const SIZE: usize = std::mem::size_of::<f64>();
//         let mut value = Self {
//             t: ffi::LuaType::L_NUMBER,
//             v: vec![0_u8; SIZE],
//         };
//         let bytes = p.to_ne_bytes();
//         value.v.copy_from_slice(&bytes);
//         value
//     }

//     #[inline]
//     pub fn as_number(&self) -> anyhow::Result<f64> {
//         const SIZE: usize = std::mem::size_of::<f64>();
//         if self.t == ffi::LuaType::L_NUMBER && self.v.len() == SIZE {
//             let v_res: anyhow::Result<&[u8; SIZE]> = self.v.as_slice().try_into().map_err(|e| {
//                 anyhow::anyhow!("could not cast lua::ffi::LuaValue to f64. error: {}", e)
//             });
//             let v = v_res?;
//             let d = f64::from_ne_bytes(*v);
//             Ok(d)
//         } else {
//             anyhow::bail!("could not cast lua::ffi::LuaValue to f64")
//         }
//     }

//     #[inline]
//     #[must_use]
//     pub fn from_string(p: &str) -> Self {
//         let size: usize = p.len();
//         let mut value = Self {
//             t: ffi::LuaType::L_STRING,
//             v: vec![0_u8; size],
//         };
//         let bytes = p.as_bytes();
//         value.v.copy_from_slice(bytes);
//         value
//     }

//     #[inline]
//     pub fn as_string(&self) -> anyhow::Result<String> {
//         if self.t == ffi::LuaType::L_STRING {
//             let d = from_utf8(self.v.as_slice())?;
//             Ok(d.to_string())
//         } else {
//             anyhow::bail!("could not cast lua::ffi::LuaValue to String")
//         }
//     }
// }

// #[repr(transparent)]
// #[allow(non_camel_case_types)]
// pub struct lua_CFunction(pub extern "C" fn(p_arg: ffi::lua_State) -> i32);
// unsafe impl cxx::ExternType for lua_CFunction {
//     type Id = cxx::type_id!("lua_CFunction");
//     type Kind = cxx::kind::Trivial;
// }

//     pub async fn bind(
//         &self,
//         p_lua_func_name: &str,
//         p_lua_callback_func: ffi::lua_CFunction,
//     ) -> anyhow::Result<()> {
//         let res_bind = self.l.bind(p_lua_func_name, p_lua_callback_func);
//         let ret = match res_bind {
//             0 => Ok(()),
//             _ => anyhow::bail!(
//                 "could not bind lua function for lua module: {}",
//                 self.module_name,
//             ),
//         };
//         ret
//     }

//     pub async fn call(
//         &self,
//         p_name: &str,
//         p_params: Option<&[ffi::LuaValue]>,
//     ) -> anyhow::Result<Vec<ffi::LuaValue>> {
//         let params = p_params.unwrap_or(&[]);
//         let mut outs = Vec::new();

//         let res_call = self.l.call(p_name, params, &mut outs);
//         let ret = match res_call {
//             0 => Ok(outs),
//             _ => anyhow::bail!(
//                 "could not execute function {} from lua module: {}",
//                 p_name,
//                 self.module_name,
//             ),
//         };
//         ret
//     }

//     pub async fn get_global(&self, p_name: &str) -> anyhow::Result<ffi::LuaValue> {
//         let mut value = ffi::LuaValue::new();
//         let res_get = self.l.get_global(p_name, &mut value);
//         let ret = match res_get {
//             0 => anyhow::Result::Ok(value),
//             _ => anyhow::Result::Err(anyhow::anyhow!(
//                 "could not get global variable {} from lua module {}",
//                 p_name,
//                 self.module_name
//             )),
//         };
//         ret
//     }

//     pub async fn set_global(&self, p_name: &str, p_value: &ffi::LuaValue) -> anyhow::Result<()> {
//         let res_g = self.l.set_global(p_name, p_value);
//         let ret = match res_g {
//             0 => Ok(()),
//             _ => Err(anyhow::anyhow!(
//                 "could not set global variable {} for lua module {}",
//                 p_name,
//                 self.module_name
//             )),
//         };
//         ret
//     }

//     pub async fn set_local_path(&self, p_env_path: &str) -> anyhow::Result<()> {
//         let res_p = self.l.set_lua_path(p_env_path);
//         let ret = match res_p {
//             0 => Ok(()),
//             _ => Err(anyhow::anyhow!(
//                 "could not set local path {} for lua module {}",
//                 p_env_path,
//                 self.module_name
//             )),
//         };
//         ret
//     }
// }

#[tokio::main]
#[test]
async fn test() {
    let lua = Lua::new();
    let ret = lua
        .load(
            "wolf_module",
            r#"
        b = true
        i = 7
        d = 17.7
        s = "hello from lua"

        function f0 ()
            print("hello from function f0")
        end

        function f1 ()
            return 1.1
        end

        function f2 (arg1, arg2)
            if (arg1 > arg2) then
                return arg1, "arg1 is greater"
            else
                return arg2, "arg2 is greater"
            end
        end
        "#,
        )
        .await;
    println!("{:?}", ret);
    assert!(ret.is_ok() && ret.unwrap() == LuaReturnCode::LuaOk);

    // ret = lua.exec().await;
    // assert!(ret.is_ok() && ret.unwrap() == LuaReturnCode::LuaOk);

    //     let mut bv = lua.get_global("b").await.unwrap();
    //     let mut iv = lua.get_global("i").await.unwrap();
    //     let mut dv = lua.get_global("d").await.unwrap();
    //     let mut sv = lua.get_global("s").await.unwrap();

    //     println!("lua b has {:?} value", bv.as_bool());
    //     println!("lua i has {:?} value", iv.as_number());
    //     println!("lua d has {:?} value", dv.as_number());
    //     println!("lua s has {:?} value", sv.as_string());

    //     let b_new = ffi::LuaValue::from_bool(false);
    //     let i_new = ffi::LuaValue::from_number(70.0);
    //     let d_new = ffi::LuaValue::from_number(70.7);
    //     let s_new = ffi::LuaValue::from_string("hello from rust");

    //     lua.set_global("b", &b_new).await.unwrap();
    //     lua.set_global("i", &i_new).await.unwrap();
    //     lua.set_global("d", &d_new).await.unwrap();
    //     lua.set_global("s", &s_new).await.unwrap();

    //     bv = lua.get_global("b").await.unwrap();
    //     iv = lua.get_global("i").await.unwrap();
    //     dv = lua.get_global("d").await.unwrap();
    //     sv = lua.get_global("s").await.unwrap();

    //     println!("lua b has {:?} value", bv.as_bool());
    //     println!("lua i has {:?} value", iv.as_number());
    //     println!("lua d has {:?} value", dv.as_number());
    //     println!("lua s has {:?} value", sv.as_string());

    //     //call void function
    //     let _f0_void = lua.call("f0", None).await.unwrap();

    //     let f1_ret = lua.call("f1", None).await.unwrap();
    //     println!("lua function f1 returns:");
    //     for iter in &f1_ret {
    //         println!("{:?}", iter.as_number());
    //     }

    //     //call f2 function
    //     let f2_ret = lua.call("f2", Some(&[d_new, i_new])).await.unwrap();
    //     println!("lua function f2 returns:");
    //     for iter in &f2_ret {
    //         let t = iter.get_type();
    //         match t {
    //             ffi::LuaType::L_NUMBER => {
    //                 println!("{:?}", iter.as_number());
    //             }
    //             ffi::LuaType::L_STRING => {
    //                 println!("{:?}", iter.as_string());
    //             }
    //             _ => {}
    //         }
    //     }
}
