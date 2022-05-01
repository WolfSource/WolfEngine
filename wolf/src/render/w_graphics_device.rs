use anyhow::{bail, Result};

pub struct WWindowInfo {
    pub handle: raw_window_handle::RawWindowHandle,
    pub width: u32,
    pub height: u32,
}

unsafe impl raw_window_handle::HasRawWindowHandle for WWindowInfo {
    fn raw_window_handle(&self) -> raw_window_handle::RawWindowHandle {
        self.handle
    }
}

pub struct WGraphicsDevice {
    pub adaptor: wgpu::Adapter,
    pub config: wgpu::SurfaceConfiguration,
    pub device: wgpu::Device,
    pub queue: wgpu::Queue,
    pub size: (u32, u32),
    pub surface: Option<wgpu::Surface>,
}

impl WGraphicsDevice {
    pub async fn new(p_window_info: Option<WWindowInfo>) -> Result<Self> {
        let surface: Option<wgpu::Surface>;
        let size: (u32, u32);

        // Backends::all means select Vulkan or Metal or DX12 or Browser
        // the WebGPU will be choose a backend based on the host platform
        let instance = wgpu::Instance::new(wgpu::Backends::all());
        let adapter_res = if let Some(win_info) = p_window_info {
            // get size
            size = (win_info.width, win_info.height);
            // create an adaptor for window
            surface = Some(unsafe { instance.create_surface(&win_info) });
            // create instance
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
            // set default size
            size = (800, 600);
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
                            // WebGL doesn't support all of WGPU's features, so if
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
                                width: size.0,
                                height: size.1,
                                present_mode: wgpu::PresentMode::Fifo,
                            };

                            // configure the surface
                            g_surface.configure(&device, &config);

                            // create graphics device
                            let g_device = Self {
                                adaptor: adapter,
                                config,
                                device,
                                queue,
                                size,
                                surface: Some(g_surface),
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
                        width: size.0,
                        height: size.1,
                        present_mode: wgpu::PresentMode::Fifo,
                    };
                    // create graphics device
                    let g_device = Self {
                        adaptor: adapter,
                        config,
                        device,
                        queue,
                        size,
                        surface: None,
                    };
                    Ok(g_device)
                }
            }
            None => {
                bail!("couldn't create graphics device adaptor")
            }
        }
    }

    pub fn resize(&mut self, p_size: (u32, u32)) {
        if p_size.0 > 0 && p_size.1 > 0 {
            self.config.width = p_size.0;
            self.config.height = p_size.1;
            self.size = p_size;
            if let Some(surf) = &self.surface {
                surf.configure(&self.device, &self.config);
            }
        }
    }

    pub fn get_adapter_info(&self) -> wgpu::AdapterInfo {
        self.adaptor.get_info()
    }

    pub fn get_size(&self) -> (u32, u32) {
        self.size
    }
}
