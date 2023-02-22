use rhai::{Engine, Identifier, RegisterNativeFunction, Variant};

#[derive(Debug, thiserror::Error)]
pub enum RhaiError {
    #[error("failed on creating rhai engine '{0}'")]
    EngineFailed(String),
}

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
    /// returns `RhaiError` on error
    pub fn run_return_void(&self, p_script: &str) -> Result<(), RhaiError> {
        let x = self.engine.run(p_script).map_err(|e| {
            let error_msg = format!("{e:?}");
            Err(RhaiError::EngineFailed(error_msg))
        });
    }

    /// # Errors
    ///
    /// returns `RhaiError` on error
    pub fn run_return_any<T>(&self, p_script: &str) -> Result<T, RhaiError>
    where
        T: Clone + Variant,
    {
        self.engine
            .eval(p_script)
            .map_err(|e| Err(RhaiError::EngineFailed(e)))
    }

    pub fn register_function<A, const N: usize, const C: bool, R, const L: bool, F>(
        &mut self,
        p_name: impl AsRef<str> + Into<Identifier>,
        p_func: F,
    ) -> &mut Self
    where
        A: 'static,
        R: Variant + Clone,
        F: RegisterNativeFunction<A, N, C, R, L>,
    {
        self.engine.register_fn(p_name, p_func)
    }
}
