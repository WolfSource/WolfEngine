#[cxx::bridge(namespace = "live_cxx")]
pub mod ffi {
    unsafe extern "C++" {
        include!("wolf-stream/src/rtsp/server/cxx/live555.hpp");

    }
}

#[tokio::main]
#[test]
async fn tests() -> () {
    println!("test live555");
}
