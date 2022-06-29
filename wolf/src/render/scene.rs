use super::graphics_device::GraphicsDevice;
use crate::system::chrono::gametime::GameTime;

pub trait IScene {
    /// # Errors
    ///
    /// TODO: add error description
    fn load(&self, _p_g_device: &mut GraphicsDevice) -> anyhow::Result<()> {
        Ok(())
    }
    /// # Errors
    ///
    /// TODO: add error description
    fn render(
        &self,
        _p_g_device: &mut GraphicsDevice,
        _p_gametime: &mut GameTime,
    ) -> std::result::Result<(), wgpu::SurfaceError> {
        Ok(())
    }
}
