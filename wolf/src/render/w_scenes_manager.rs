use super::{
    w_graphics_device::WGraphicsDevice,
    w_scene::{IWScene, WScene},
};
use crate::{system::chrono::w_gametime::WGameTime, w_log};
use anyhow::{anyhow, Result};
use winit::{
    event::*,
    event_loop::{ControlFlow, EventLoop},
    window::WindowBuilder,
};

pub struct WScenesManager {
    //scene: &'a WScene,
    //next_scene: Option<&'b WScene>,
}

impl WScenesManager {
    pub async fn run<I>(p_scene: &'static mut WScene<'_>) -> Result<()> {
        // TODO: create a tracing log for native
        #[cfg(target_arch = "wasm32")]
        {
            std::panic::set_hook(Box::new(console_error_panic_hook::hook));
        }

        // create a window
        let event_loop = EventLoop::new();
        let window = WindowBuilder::new()
            .build(&event_loop)
            .map_err(|e| anyhow!("could not create window {:?}", e))?;

        #[cfg(target_arch = "wasm32")]
        {
            // Winit prevents sizing with CSS, so we have to set the size manually when on web.
            use winit::dpi::PhysicalSize;
            window.set_inner_size(PhysicalSize::new(640, 480));

            use winit::platform::web::WindowExtWebSys;
            web_sys::window()
                .and_then(|win| win.document())
                .and_then(|doc| {
                    let dst = doc.get_element_by_id("wolf")?;
                    let canvas = web_sys::Element::from(window.canvas());
                    dst.append_child(&canvas).ok()?;
                    Some(())
                })
                .expect("couldn't append canvas to document body.");
        }

        // create a graphics device
        let mut g_device = WGraphicsDevice::new(Some(&window)).await?;
        let mut game_time = WGameTime::new();
        // load scene
        let _ = p_scene.load(&mut g_device);

        // create an event loop
        event_loop.run(move |event, _, control_flow| match event {
            Event::WindowEvent {
                ref event,
                window_id,
            } if window_id == window.id() => match event {
                WindowEvent::CloseRequested
                | WindowEvent::KeyboardInput {
                    input:
                        KeyboardInput {
                            state: ElementState::Pressed,
                            virtual_keycode: Some(VirtualKeyCode::Escape),
                            ..
                        },
                    ..
                } => *control_flow = ControlFlow::Exit,
                _ => {}
            },
            Event::RedrawRequested(window_id) if window_id == window.id() => {
                game_time.tick();
                p_scene
                    .render(&mut g_device, &mut game_time)
                    .unwrap_or_else(|e| {
                        w_log!("render got error {:?}", e);
                    });

                match g_device.render() {
                    Ok(_) => {}
                    // Reconfigure the surface if lost
                    Err(wgpu::SurfaceError::Lost) => {
                        g_device.resize(g_device.size);
                    }
                    // The system is out of memory, we should probably quit
                    Err(wgpu::SurfaceError::OutOfMemory) => {
                        *control_flow = ControlFlow::Exit;
                    }
                    // All other errors (Outdated, Timeout) should be resolved by the next frame
                    Err(e) => {
                        w_log!("render failed because of {:?}", e);
                    }
                };
            }
            Event::RedrawEventsCleared => {
                // RedrawRequested will only trigger once, unless we manually, request it.
                window.request_redraw();
            }
            _ => {}
        });
    }
}
