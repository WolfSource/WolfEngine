use super::{
    callback::{OnMessageCallback, OnSocketCallback},
    tcp::tcp_client::TcpClient,
};
use crate::{error::WError, system::buffer::Buffer};
use std::net::SocketAddr;

/// # Safety
///
/// unsafe function for C ABI
#[no_mangle]
pub unsafe extern "C" fn w_system_socket_tcp_client_new(
    p_connect_number_of_retry: u32,
    p_connect_timeout_in_secs: f64,
    p_endpoint_address: *const std::ffi::c_char,
    p_io_number_of_retry: u32,
    p_io_timeout_in_secs: f64,
    p_port: u16,
    p_tls: bool,
    p_error: *mut WError,
) -> *mut TcpClient {
    use std::ffi::CStr;
    let str_res = CStr::from_ptr(p_endpoint_address).to_str();
    str_res.map_or_else(
        |_| {
            std::ptr::write(p_error, WError::SystemSocketUtf8Error);
            std::ptr::null_mut()
        },
        |str| {
            let tcp_client = TcpClient::new(
                str,
                p_port,
                p_connect_number_of_retry,
                p_connect_timeout_in_secs,
                p_io_number_of_retry,
                p_io_timeout_in_secs,
                p_tls,
            );
            std::ptr::write(p_error, WError::None);
            Box::into_raw(Box::new(tcp_client))
        },
    )
}

/// # Safety
///
/// unsafe function for C ABI
#[no_mangle]
pub unsafe extern "C" fn w_system_socket_tcp_client_connect(
    p_socket: *mut TcpClient,
    p_on_accept_connection: &'static extern "C" fn(*const std::ffi::c_char) -> WError,
    p_on_message: &'static extern "C" fn(*const std::ffi::c_char, *mut Buffer, f64) -> WError,
    p_on_close_connection: &'static extern "C" fn(*const std::ffi::c_char) -> WError,
) -> WError {
    pollster::block_on(async move {
        let socket = Box::from_raw(p_socket);
        let on_connect = OnSocketCallback::new(Box::new(
            |p_socket_address: &SocketAddr| -> Result<(), WError> {
                let str = p_socket_address.to_string();
                let error = p_on_accept_connection(str.as_str().as_ptr().cast::<i8>());
                if error == WError::None {
                    return Ok(());
                }
                Err(error)
            },
        ));
        let on_close = OnSocketCallback::new(Box::new(
            |p_socket_address: &SocketAddr| -> Result<(), WError> {
                let str = p_socket_address.to_string();
                let error = p_on_close_connection(str.as_str().as_ptr().cast::<i8>());
                if error == WError::None {
                    return Ok(());
                }
                Err(error)
            },
        ));
        let on_msg = OnMessageCallback::new(Box::new(
            |p_socket_address: &SocketAddr,
             p_buffer: &mut Buffer,
             p_socket_live_time_in_secs: f64|
             -> Result<(), WError> {
                let str = p_socket_address.to_string();
                let error = p_on_message(
                    str.as_str().as_ptr().cast::<i8>(),
                    p_buffer,
                    p_socket_live_time_in_secs,
                );
                if error == WError::None {
                    return Ok(());
                }
                Err(error)
            },
        ));

        let res = socket.connect(on_connect, on_msg, on_close).await;
        std::mem::forget(socket);
        match res {
            Ok(_) => WError::None,
            Err(e) => e,
        }
    })
}

/// # Safety
///
/// unsafe function for C ABI
#[no_mangle]
pub unsafe extern "C" fn w_system_socket_tcp_client_free(p_ptr: *mut TcpClient) {
    if p_ptr.is_null() {
        return;
    }
    std::mem::drop(Box::from_raw(p_ptr));
}
