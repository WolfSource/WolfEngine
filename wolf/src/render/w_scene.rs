use super::w_graphics_device::WGraphicsDevice;
use crate::system::chrono::w_gametime::WGameTime;

pub trait IWScene {
    fn load(&self, _p_g_device: &mut WGraphicsDevice) -> anyhow::Result<()> {
        Ok(())
    }
    fn render(
        &self,
        _p_g_device: &mut WGraphicsDevice,
        _p_gametime: &mut WGameTime,
    ) -> std::result::Result<(), wgpu::SurfaceError> {
        Ok(())
    }
}
