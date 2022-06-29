use anyhow::{anyhow, Result};
use rhai::{Engine, Identifier, RegisterNativeFunction, Variant};

pub struct Rhai {
    engine: Engine,
}

impl Default for Rhai {
    fn default() -> Self {
        Self::new()
    }
}

impl Rhai {
    #[must_use]
    pub fn new() -> Self {
        Self {
            engine: Engine::new(),
        }
    }

    /// # Errors
    ///
    /// TODO:
    pub fn run_return_void(&self, p_script: &str) -> Result<()> {
        const TRACE: &str = "WRhai::run_return_void";
        self.engine
            .run(p_script)
            .map_err(|e| anyhow!("{:?}", e).context(TRACE))
    }

    /// # Errors
    ///
    /// TODO:
    pub fn run_return_any<T>(&self, p_script: &str) -> Result<T>
    where
        T: Clone + Variant,
    {
        const TRACE: &str = "WRhai::run_return_any";
        self.engine
            .eval(p_script)
            .map_err(|e| anyhow!("{:?}", e).context(TRACE))
    }

    pub fn register_function<N, A, F>(&mut self, p_name: N, p_func: F)
    where
        N: AsRef<str> + Into<Identifier>,
        F: RegisterNativeFunction<A, ()>,
    {
        self.engine.register_fn(p_name, p_func);
    }
}
