use super::graphics_device::GraphicsDevice;
use crate::system::chrono::gametime::GameTime;

pub trait IScene {
    fn load(&self, _p_g_device: &mut GraphicsDevice) -> anyhow::Result<()> {
        Ok(())
    }
    fn render(
        &self,
        _p_g_device: &mut GraphicsDevice,
        _p_gametime: &mut GameTime,
    ) -> std::result::Result<(), wgpu::SurfaceError> {
        Ok(())
    }
}
