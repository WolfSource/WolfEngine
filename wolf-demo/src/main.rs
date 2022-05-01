#![cfg_attr(target_arch = "wasm32", feature(async_closure))]
#![allow(unused_imports)]
#![allow(unreachable_code)]
use anyhow::{anyhow, Result};
use raw_window_handle::HasRawWindowHandle;
use wgpu::SurfaceError;
use winit::event::{ElementState, Event, KeyboardInput, VirtualKeyCode, WindowEvent};
use winit::event_loop::{ControlFlow, EventLoop};
use winit::window::WindowBuilder;
use wolf::render::w_graphics_device::WWindowInfo;
use wolf::system::os::w_sigslot::WSigSlot;
use wolf::{
    render::{w_graphics_device::WGraphicsDevice, w_scene::IWScene},
    system::{chrono::w_gametime::WGameTime, os::w_runtime::WRunTime, script::w_rhai::WRhai},
    w_log,
};

#[cfg(target_arch = "wasm32")]
use {
    wasm_bindgen::prelude::*, wasm_mt::prelude::*, wolf::system::script::w_javascript::WJavaScript,
};

// Normal function
fn add(x: i64, y: i64) -> i64 {
    x + y
}

async fn test() {
    let mut script = WRhai::new();

    // register add function for our embedded script
    script.register_function("add", add);

    let res = script.run_return_any::<i64>(r#"add(10, 7)"#);
    match res {
        Ok(v) => {
            w_log!("add returns: {}", v);
        }
        Err(e) => {
            w_log!("add returns error: {:?}", e);
        }
    };

    #[cfg(not(target_arch = "wasm32"))]
    {
        let f = async move {
            println!("t1 started");
            WRunTime::sleep(std::time::Duration::from_secs(1));
            w_log!("t1 just stopped after 2 seconds");
        };
        // execute thread
        WRunTime::green_thread(f).await;
        WRunTime::async_sleep(std::time::Duration::from_secs(2)).await;
    }

    #[cfg(target_arch = "wasm32")]
    {
        let f1 = async move {
            let js = WJavaScript::new(None);
            let _js_res = js
                .execute(
                    "
             console.log(\"hello from javascript promise\");
             const sub = (a, b) => new Promise(resolve => {
                 setTimeout(() => resolve(a - b), 1000);
             });
             return await sub(1, 2);
         "
                    .to_owned(),
                    true,
                )
                .await;
        };
        WRunTime::spawn_local(f1);

        #[cfg(target_arch = "wasm32")]
        let f2 = FnOnce!(async move || {
            w_log!("t1 worker started");
            WRunTime::async_sleep(std::time::Duration::from_secs(2)).await;
            w_log!("t1 worker just stopped after 5 seconds");
            Ok(JsValue::null())
        });
        // execute thread
        WRunTime::thread(f2);
    }

    // create SigSlot
    let mut sig_slot = WSigSlot::new();

    // create slots
    let i = 1;
    let con_1 = sig_slot.connect(move || {
        w_log!("hello from slot{}", i);
    });
    let con_2 = sig_slot.connect(|| {
        w_log!("hello from slot2");
    });

    // check for connections
    if con_1.is_connected() && con_2.is_connected() {
        w_log!("slot 1 & 2 was connected");
        // disconnect slot 2
        con_2.disconnect();
        w_log!("slot 2 just disconnected");
    }

    // wait for threads
    WRunTime::async_sleep(std::time::Duration::from_secs(1)).await;

    // emit all
    sig_slot.emit();
}

#[derive(Default)]
struct WScene {}

impl IWScene for WScene {
    fn load(&self, _p_g_device: &mut WGraphicsDevice) -> Result<()> {
        w_log!("scene is going to loaded");
        WRunTime::spawn_local(async move {
            test().await;
        });
        w_log!("scene just loaded");
        Ok(())
    }
    fn render(
        &self,
        p_gdevice: &mut WGraphicsDevice,
        p_gametime: &mut WGameTime,
    ) -> std::result::Result<(), wgpu::SurfaceError> {
        w_log!(
            "scene is rendering {}",
            p_gametime.get_total_elapsed_seconds()
        );

        // get output from surface
        let output_res = if let Some(surf) = &p_gdevice.surface {
            surf.get_current_texture()
        } else {
            w_log!("surface is None, make sure use render_to_texture function for offscreen rendering mode");
            Err(SurfaceError::Outdated)
        };

        let output = output_res?;

        // create texture view
        let view = output
            .texture
            .create_view(&wgpu::TextureViewDescriptor::default());

        // create command encoder
        let mut cmd_encoder =
            p_gdevice
                .device
                .create_command_encoder(&wgpu::CommandEncoderDescriptor {
                    label: Some("Render Encoder"),
                });
        // execute command
        {
            let _render_pass = cmd_encoder.begin_render_pass(&wgpu::RenderPassDescriptor {
                label: Some("Render Pass"),
                color_attachments: &[wgpu::RenderPassColorAttachment {
                    view: &view,
                    resolve_target: None,
                    ops: wgpu::Operations {
                        load: wgpu::LoadOp::Clear(wgpu::Color {
                            r: 0.1,
                            g: 0.2,
                            b: 0.3,
                            a: 1.0,
                        }),
                        store: true,
                    },
                }],
                depth_stencil_attachment: None,
            });
        }
        // submit to the queue
        p_gdevice
            .queue
            .submit(std::iter::once(cmd_encoder.finish()));
        // send to output
        output.present();
        Ok(())
    }
}

async fn run<I>(p_scene: I) -> Result<()>
where
    I: IWScene + Sync + 'static,
{
    const TRACE: &str = "WSceneManager::run";

    // create a window
    let event_loop = EventLoop::new();
    let window = WindowBuilder::new()
        .build(&event_loop)
        .map_err(|e| anyhow!("could not create window {:?}", e))?;

    #[cfg(target_arch = "wasm32")]
    {
        // Winit prevents sizing with CSS, so we have to set the size manually when on web.
        use winit::dpi::PhysicalSize;
        window.set_inner_size(PhysicalSize::new(800, 600));

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

    let size = window.inner_size();
    let win_info = WWindowInfo {
        handle: window.raw_window_handle(),
        width: size.width,
        height: size.height,
    };

    // create a graphics device
    let mut g_device = WGraphicsDevice::new(Some(win_info)).await?;
    let mut game_time = WGameTime::new();

    // load scene
    let load_res = p_scene.load(&mut g_device);
    match load_res {
        Ok(_) => {}
        Err(e) => {
            w_log!("could not load scene because {:?} trace info: {}", e, TRACE);
        }
    }

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
            // tick game time
            game_time.tick();
            // render scene
            let render_res = p_scene.render(&mut g_device, &mut game_time);
            match render_res {
                Ok(()) => {
                    //Success
                }
                // Reconfigure the surface if lost
                Err(SurfaceError::Lost) => {
                    g_device.resize(g_device.get_size());
                }
                // The system is out of memory, we should probably quit
                Err(SurfaceError::OutOfMemory) => {
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

#[cfg(target_arch = "wasm32")]
#[wasm_bindgen]
pub fn main() {
    std::panic::set_hook(Box::new(console_error_panic_hook::hook));
    // run scene
    w_log!("starting wolf-demo in wasm mode");
    WRunTime::spawn_local(async move {
        let _res = run(WScene::default()).await;
    });
}

#[cfg(not(target_arch = "wasm32"))]
#[tokio::main]
pub async fn main() {
    // run scene
    w_log!("starting wolf-demo in native mode");
    let _res = run(WScene::default()).await;
}
