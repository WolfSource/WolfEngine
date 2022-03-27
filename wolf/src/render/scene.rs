use crate::wlog;

/// This example shows how to describe the adapter in use.
async fn run() {
    let adapter_res = wgpu::Instance::new(wgpu::Backends::all())
        .request_adapter(&wgpu::RequestAdapterOptions::default())
        .await;
    match adapter_res {
        Some(adapter) => {
            wlog!("adapter info: {:?}", adapter.get_info());
        }
        None => {
            wlog!("couldn't get adapter info");
        }
    }
}

pub fn start() {
    #[cfg(not(target_arch = "wasm32"))]
    {
        pollster::block_on(run());
    }
    #[cfg(target_arch = "wasm32")]
    {
        std::panic::set_hook(Box::new(console_error_panic_hook::hook));
        wasm_bindgen_futures::spawn_local(run());
    }
}
