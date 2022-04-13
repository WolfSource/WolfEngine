use anyhow::{bail, Result};
use winit::{dpi::PhysicalSize, window::Window};

use crate::w_log;

#[allow(dead_code)] //TODO: will be removed
pub struct WGraphicsDevice {
    adaptor: wgpu::Adapter,
    config: wgpu::SurfaceConfiguration,
    device: wgpu::Device,
    queue: wgpu::Queue,
    pub size: winit::dpi::PhysicalSize<u32>,
    surface: Option<wgpu::Surface>,
}

impl WGraphicsDevice {
    pub async fn new(p_window: Option<&Window>) -> Result<Self> {
        let size: PhysicalSize<u32>;
        let surface: Option<wgpu::Surface>;

        // Backends::all means select Vulkan or Metal or DX12 or Browser
        // the WebGPU will be choose a backend based on the host platform
        let instance = wgpu::Instance::new(wgpu::Backends::all());
        let adapter_res = if let Some(window) = p_window {
            // create an adaptor for window
            size = window.inner_size();
            surface = Some(unsafe { instance.create_surface(window) });
            instance
                .request_adapter(&wgpu::RequestAdapterOptions {
                    power_preference: wgpu::PowerPreference::default(),
                    compatible_surface: surface.as_ref(),
                    force_fallback_adapter: false,
                })
                .await
        } else {
            // create an adaptor for off screen rendering
            surface = None;
            size = PhysicalSize::<u32>::new(640, 480);
            instance
                .request_adapter(&wgpu::RequestAdapterOptions::default())
                .await
        };
        match adapter_res {
            Some(adapter) => {
                let (device, queue) = adapter
                    .request_device(
                        &wgpu::DeviceDescriptor {
                            label: None,
                            features: wgpu::Features::empty(),
                            // WebGL doesn't support all of wgpu's features, so if
                            // we're building for the web we'll have to disable some.
                            limits: if cfg!(target_arch = "wasm32") {
                                wgpu::Limits::downlevel_webgl2_defaults()
                            } else {
                                wgpu::Limits::default()
                            },
                        },
                        // Some(&std::path::Path::new("trace")), // Trace path
                        None,
                    )
                    .await
                    .unwrap();

                if let Some(g_surface) = surface {
                    // create a graphics device for this surface window
                    let format_res = g_surface.get_preferred_format(&adapter);
                    // in this match block, we are going to return a Result<WGraphicsDevice> object
                    match format_res {
                        Some(texture_format) => {
                            // create config
                            let config = wgpu::SurfaceConfiguration {
                                usage: wgpu::TextureUsages::RENDER_ATTACHMENT,
                                format: texture_format,
                                width: size.width,
                                height: size.height,
                                present_mode: wgpu::PresentMode::Fifo,
                            };
                            // configure the surface
                            g_surface.configure(&device, &config);
                            // create graphics device
                            let g_device = Self {
                                config,
                                device,
                                queue,
                                size,
                                surface: Some(g_surface),
                                adaptor: adapter,
                            };
                            // return the graphics device
                            Ok(g_device)
                        }
                        None => {
                            bail!("couldn't create texture format")
                        }
                    }
                } else {
                    // create config
                    let config = wgpu::SurfaceConfiguration {
                        usage: wgpu::TextureUsages::RENDER_ATTACHMENT,
                        format: wgpu::TextureFormat::Rgba32Sint,
                        width: size.width,
                        height: size.height,
                        present_mode: wgpu::PresentMode::Fifo,
                    };
                    // create graphics device
                    let g_device = Self {
                        config,
                        device,
                        queue,
                        size,
                        surface: None,
                        adaptor: adapter,
                    };
                    Ok(g_device)
                }
            }
            None => {
                bail!("couldn't create graphics device adaptor")
            }
        }
    }

    pub fn resize(&mut self, new_size: PhysicalSize<u32>) {
        if new_size.width > 0 && new_size.height > 0 {
            self.size = new_size;
            self.config.width = new_size.width;
            self.config.height = new_size.height;
            if let Some(surf) = &self.surface {
                surf.configure(&self.device, &self.config);
            }
        }
    }

    pub fn render(&mut self) -> Result<(), wgpu::SurfaceError> {
        // get output from surface
        let output_res = if let Some(surf) = &self.surface {
            surf.get_current_texture()
        } else {
            w_log!("surface is None, make sure use render_to_texture function for offscreen rendering mode");
            Err(wgpu::SurfaceError::Outdated)
        };
        let output = output_res?;

        // create texture view
        let view = output
            .texture
            .create_view(&wgpu::TextureViewDescriptor::default());

        // create command encoder
        let mut cmd_encoder = self
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
        self.queue.submit(std::iter::once(cmd_encoder.finish()));
        // send to output
        output.present();
        // return OK
        Ok(())
    }
}
