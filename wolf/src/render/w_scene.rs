use super::w_graphics_device::WGraphicsDevice;
use crate::system::chrono::w_gametime::WGameTime;
use anyhow::{bail, Result};

pub struct WScene<'a> {
    loaded: bool,
    name: String,
    on_load: &'a mut dyn FnMut(&mut WGraphicsDevice) -> Result<()>,
    on_render: &'a mut dyn FnMut(&mut WGraphicsDevice, &mut WGameTime) -> Result<()>,
}

pub trait IWScene {
    fn load(&mut self, p_gdevice: &mut WGraphicsDevice) -> Result<()>;
    fn render(
        &mut self,
        p_gdevice: &mut WGraphicsDevice,
        p_game_time: &mut WGameTime,
    ) -> Result<()>;
}

impl<'a> WScene<'a> {
    pub fn new(
        p_scene_name: &str,
        p_on_load: &'a mut dyn FnMut(&mut WGraphicsDevice) -> Result<()>,
        p_on_render: &'a mut dyn FnMut(&mut WGraphicsDevice, &mut WGameTime) -> Result<()>,
    ) -> Self {
        Self {
            loaded: false,
            name: p_scene_name.to_owned(),
            on_load: p_on_load,
            on_render: p_on_render,
        }
    }
}

impl IWScene for WScene<'_> {
    fn load(&mut self, p_gdevice: &mut WGraphicsDevice) -> Result<()> {
        if self.loaded {
            bail!("the scene '{}' was already loaded", self.name);
        }
        (self.on_load)(p_gdevice).map(|_| {
            self.loaded = true;
        })
    }
    fn render(
        &mut self,
        p_gdevice: &mut WGraphicsDevice,
        p_game_time: &mut WGameTime,
    ) -> Result<()> {
        (self.on_render)(p_gdevice, p_game_time)
    }
}
