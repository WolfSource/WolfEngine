#[cxx::bridge(namespace = "rtsp_client_cxx")]
pub mod ffi {
    unsafe extern "C++" {
    }
}

#[tokio::main]
#[test]
async fn tests() -> () {
    println!("test");
}
