#[cxx::bridge(namespace = "rtsp_client_cxx")]
pub mod ffi {
    unsafe extern "C++" {
        include!("wolf_stream/src/rtsp/client/cxx/rtsp_client.hpp");

    }
}

#[test]
fn test() {}
