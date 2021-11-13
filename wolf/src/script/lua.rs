#[cxx::bridge]
pub mod ffi {

    #[repr(u8)]
    #[derive(Debug, Clone, Copy)]
    enum LuaType {
        L_NIL = 0,
        L_BOOLEAN,
        L_LIGHTUSERDATA, //Not implemented yet
        L_DOUBLE,
        L_STRING,
        L_TABLE,
        L_FUNCTION,
        L_USERDATA, //Not implemented yet
        L_THREAD,   //Not implemented yet
        L_INTEGER,
    }

    #[derive(Default, Debug)]
    pub struct lua_in_value<'a> {
        t: LuaType,  //type
        v: &'a [u8], //value in bytes
    }

    #[derive(Default, Debug)]
    pub struct lua_inout_value<'a> {
        t: LuaType,      //type
        v: &'a mut [u8], //value in native endians bytes
    }

    unsafe extern "C++" {
        include!("wolf/src/script/cxx/luaJIT/luaJIT.hpp");

        //pub type c_void;
        pub type lua_State;
        type lua_CFunction = super::lua_CFunction;

        pub type luaJIT;
        pub fn load(self: &luaJIT, p_module_name: &str, p_source_code: &str) -> i32;
        pub fn exec(self: &luaJIT) -> i32;
        pub fn bind(self: &luaJIT, p_lua_fn_name: &str, p_lua_c_fun: lua_CFunction) -> i32;
        pub fn call(
            self: &luaJIT,
            p_lua_fn_name: &str,
            p_params: &[lua_in_value],
            p_results: &mut Vec<lua_in_value>,
        ) -> i32;
        pub fn get_global_var(self: &luaJIT, p_name: &String, p_value: &mut lua_inout_value)
            -> i32;
        pub fn get_global_str(self: &luaJIT, p_name: &String, p_value: &mut String) -> i32;
        pub fn set_global_var(self: &luaJIT, p_name: &str, p_value: &lua_in_value) -> i32;
        pub fn set_lua_path(self: &luaJIT, p_env_path: &str) -> i32;

        pub fn New() -> UniquePtr<luaJIT>;
    }
}

impl Default for ffi::LuaType {
    fn default() -> Self {
        ffi::LuaType::L_NIL
    }
}

impl<'a> ffi::lua_in_value<'a> {
    pub fn new() -> Self {
        ffi::lua_in_value::default()
    }
}

#[repr(transparent)]
#[allow(non_camel_case_types)]
pub struct lua_CFunction(pub extern "C" fn(p_arg: ffi::lua_State) -> i32);
unsafe impl cxx::ExternType for lua_CFunction {
    type Id = cxx::type_id!("lua_CFunction");
    type Kind = cxx::kind::Trivial;
}

pub struct Lua {
    l: cxx::UniquePtr<ffi::luaJIT>,
    module_name: String,
}

impl Lua {
    pub fn new() -> Self {
        Self {
            l: ffi::New(),
            module_name: String::new(),
        }
    }

    pub fn load(&mut self, p_module_name: &String, p_source_code: &String) -> anyhow::Result<()> {
        let res = self.l.load(p_module_name, p_source_code);
        let ret = match res {
            0 => {
                self.module_name = p_module_name.clone();
                Ok(())
            }
            _ => anyhow::bail!(
                "Could not load lua module: {} with the following source code: {}",
                p_module_name,
                p_source_code
            ),
        };
        ret
    }

    pub fn exec(&self) -> anyhow::Result<()> {
        let res = self.l.exec();
        let ret = match res {
            0 => Ok(()),
            _ => anyhow::bail!("Could not execute lua module: {}", self.module_name,),
        };
        ret
    }

    pub fn get_global_var_bool(&self, p_name: &String) -> anyhow::Result<bool> {
        const TRACE: &str = "Lua::get_global_var_bool";
        const SIZE: usize = std::mem::size_of::<bool>();
        let mut b = [0u8; SIZE];
        let mut value = ffi::lua_inout_value {
            t: ffi::LuaType::L_BOOLEAN,
            v: &mut b,
        };
        let res = self.l.get_global_var(&p_name, &mut value);
        let ret = match res {
            0 => {
                let v_res: anyhow::Result<&mut [u8; SIZE]> = value.v.try_into().map_err(|e| {
                    anyhow::anyhow!("could not cast value because of {}", e).context(TRACE)
                });
                let v = v_res?;
                anyhow::Result::Ok(v[0] != 0)
            }
            _ => anyhow::Result::Err(
                anyhow::anyhow!(
                    "Could not get global variable {} from lua module {}",
                    p_name,
                    self.module_name
                )
                .context(TRACE),
            ),
        };
        ret
    }

    pub fn get_global_var_double(&self, p_name: &String) -> anyhow::Result<f64> {
        const TRACE: &str = "Lua::get_global_var_double";
        const SIZE: usize = std::mem::size_of::<f64>();
        let mut b = [0u8; SIZE];
        let mut value = ffi::lua_inout_value {
            t: ffi::LuaType::L_DOUBLE,
            v: &mut b,
        };
        let res = self.l.get_global_var(&p_name, &mut value);
        let ret = match res {
            0 => {
                let v_res: anyhow::Result<&mut [u8; SIZE]> = value.v.try_into().map_err(|e| {
                    anyhow::anyhow!("could not cast value because of {}", e).context(TRACE)
                });
                let v = v_res?;
                let d = f64::from_ne_bytes(*v);
                anyhow::Result::Ok(d)
            }
            _ => anyhow::Result::Err(
                anyhow::anyhow!(
                    "Could not get global variable {} from lua module {}",
                    p_name,
                    self.module_name
                )
                .context(TRACE),
            ),
        };
        ret
    }

    pub fn get_global_var_string(&self, p_name: &String) -> anyhow::Result<String> {
        const TRACE: &str = "Lua::get_global_var_string";
        let mut str = String::new();
        let res = self.l.get_global_str(&p_name, &mut str);
        let ret = match res {
            0 => anyhow::Result::Ok(str),
            _ => anyhow::Result::Err(
                anyhow::anyhow!(
                    "Could not get global string {} from lua module {}",
                    p_name,
                    self.module_name
                )
                .context(TRACE),
            ),
        };
        ret
    }

    pub fn get_global_var_int(&self, p_name: &String) -> anyhow::Result<i32> {
        let d = self.get_global_var_double(p_name)?;
        anyhow::Result::Ok(d as i32)
    }

    pub fn set_global_var_as_bool(&self, p_variable: &bool) -> anyhow::Result<()> {
        Ok(())
    }

    pub fn set_global_var_as_int(&self, p_variable: &i32) -> anyhow::Result<()> {
        Ok(())
    }

    pub fn set_global_var_as_double(&self, p_variable: &f64) -> anyhow::Result<()> {
        Ok(())
    }

    pub fn set_global_var_as_string(&self, p_variable: &String) -> anyhow::Result<()> {
        Ok(())
    }
}

#[test]
fn test() {
    let mut lua = Lua::new();
    let ret = lua
        .load(
            &"wolf_lua".to_string(),
            &r#"
b0 = true
b1 = true
i = 7
d = 17.0
s = "hello wolf from lua"

"#
            .to_string(),
        )
        .and_then(|_| lua.exec())
        .and_then(|_| {
            let bool_var_name_0 = "b0".to_string();
            let bool_var_name_1 = "b1".to_string();
            let int_var_name = "i".to_string();
            let double_var_name = "d".to_string();
            let string_var_name = "s".to_string();

            let b0 = lua.get_global_var_bool(&bool_var_name_0)?;
            let b1 = lua.get_global_var_bool(&bool_var_name_1)?;
            let i = lua.get_global_var_int(&int_var_name)?;
            let d = lua.get_global_var_double(&double_var_name)?;
            let s = lua.get_global_var_string(&string_var_name)?;

            println!("lua {} variable has {} value", bool_var_name_0, b0);
            println!("lua {} variable has {} value", bool_var_name_1, b1);
            println!("lua {} variable has {} value", int_var_name, i);
            println!("lua {} variable has {} value", double_var_name, d);
            println!("lua {} variable has \"{}\" value", string_var_name, s);

            Ok(())
        });
    println!("result of Lua is {:?}", ret);
}
