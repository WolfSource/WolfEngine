use crate::w_log;

/// This example shows how to describe the adapter in use.
async fn run() {
    let adapter_res = wgpu::Instance::new(wgpu::Backends::all())
        .request_adapter(&wgpu::RequestAdapterOptions::default())
        .await;
    match adapter_res {
        Some(adapter) => {
            w_log!("adapter info: {:?}", adapter.get_info());
        }
        None => {
            w_log!("couldn't get adapter info");
        }
    }
}

pub fn start() {
    #[cfg(not(feature = "wasm"))]
    {
        futures::executor::block_on(run());
    }
    #[cfg(feature = "wasm")]
    {
        std::panic::set_hook(Box::new(console_error_panic_hook::hook));
        wasm_bindgen_futures::spawn_local(run());
    }
}
