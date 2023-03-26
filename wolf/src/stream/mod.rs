// #![allow(non_upper_case_globals)]
// #![allow(non_camel_case_types)]
// #![allow(non_snake_case)]

#[cfg(all(
    feature = "stream_grpc",
    not(any(target_os = "android", target_os = "ios", target_arch = "wasm32"))
))]
pub mod grpc;
#[cfg(all(feature = "stream_quic", not(target_arch = "wasm32")))]
pub mod quic;
pub mod webrtc;
// #[cfg(feature = "stream_rist")]
// pub mod rist;
// #[cfg(any(feature = "stream_socket_client", feature = "stream_socket_server"))]
// pub mod udp;
