use std::{
    net::AddrParseError,
    path::{Path, PathBuf},
};
use tonic::transport::{Certificate, Channel, ClientTlsConfig, Endpoint};

#[derive(Debug, thiserror::Error)]
pub enum GrpcError {
    #[error("TLS cert not found: `{0}`")]
    TlsCrtNotFound(PathBuf),
    #[error("TLS key not found: `{0}`")]
    TlsKeyNotFound(PathBuf),
    #[error("Address parse error: `{0}`")]
    AddrParse(AddrParseError),
    #[error("Server could not build because: `{0}`")]
    ServerBuildFailed(tonic::transport::Error),
    #[error("Grpc endpoint `{0}` is invalid because `{1}`")]
    InvalidUri(String, tonic::codegen::http::uri::InvalidUri),
    #[error("Grpc endpoint `{0}` had an invalid TLS config `{1}`")]
    InvalidClientTlsConfig(String, tonic::transport::Error),
    #[error("Grpc endpoint `{0}` missing Tls file `{1}`")]
    InvalidTlsCert(String, std::io::Error),
    #[error("Grpc endpoint `{0}` could not create a channel because of `{1}`")]
    ChannelError(String, tonic::transport::Error),
}

#[cfg(not(any(target_os = "android", target_os = "ios", target_arch = "wasm32")))]
#[derive(Debug)]
pub struct GrpcServerConfig<'a> {
    pub address: &'a str,
    pub port: u16,
    pub tls: bool,
    pub tls_certificate_path: Option<&'a Path>,
    pub tls_private_key_path: Option<&'a Path>,
    pub tcp_no_delay: bool,
    pub tcp_keep_alive: Option<std::time::Duration>,
    pub http2_keepalive_interval: Option<std::time::Duration>,
    pub http2_keepalive_timeout: Option<std::time::Duration>,
    pub accept_http1: bool,
}

#[cfg(not(any(target_os = "android", target_os = "ios", target_arch = "wasm32")))]
/// # Errors
///
/// returns `GrpcError` on error
pub async fn run_server<S, F>(
    p_config: &GrpcServerConfig<'_>,
    p_service: S,
    p_shutdown_signal: F,
) -> Result<(), GrpcError>
where
    S: tonic::codegen::Service<
            hyper::Request<hyper::Body>,
            Response = hyper::Response<tonic::body::BoxBody>,
            Error = std::convert::Infallible,
        > + tonic::transport::NamedService
        + Clone
        + Send
        + 'static,
    S::Future: Send + 'static,
    F: std::future::Future<Output = ()> + Send,
{
    let addr = format!("{}:{}", p_config.address, p_config.port)
        .parse()
        .map_err(GrpcError::AddrParse)?;

    let mut builder = tonic::transport::Server::builder()
        .tcp_nodelay(p_config.tcp_no_delay)
        .tcp_keepalive(p_config.tcp_keep_alive)
        .http2_keepalive_interval(p_config.http2_keepalive_interval)
        .http2_keepalive_timeout(p_config.http2_keepalive_timeout)
        .accept_http1(p_config.accept_http1);

    if p_config.tls {
        use tonic::transport::Identity;
        use tonic::transport::ServerTlsConfig;

        let p = Path::new("");
        //load certificate and private key for tls
        let cert_path = p_config.tls_certificate_path.unwrap_or(p);
        let key_path = p_config.tls_private_key_path.unwrap_or(p);

        if !cert_path.exists() {
            return Err(GrpcError::TlsCrtNotFound(cert_path.to_path_buf()));
        }
        if !key_path.exists() {
            return Err(GrpcError::TlsKeyNotFound(key_path.to_path_buf()));
        }

        //load certificates
        let cert = tokio::fs::read(cert_path)
            .await
            .map_err(|_| GrpcError::TlsCrtNotFound(cert_path.to_path_buf()))?;

        let key = tokio::fs::read(key_path)
            .await
            .map_err(|_| GrpcError::TlsCrtNotFound(key_path.to_path_buf()))?;

        let identity = Identity::from_pem(cert, key);
        let server_tls_config = ServerTlsConfig::new().identity(identity);

        builder = builder
            .tls_config(server_tls_config)
            .map_err(GrpcError::ServerBuildFailed)?;
    }

    builder
        .add_service(p_service)
        .serve_with_shutdown(addr, p_shutdown_signal)
        .await
        .map_err(GrpcError::ServerBuildFailed)
}

///create a tls grpc endpoint
/// # Errors
///
/// returns an enpoint on success or `GrpcError` on error
pub async fn create_tls_endpoint(
    p_end_point: String,
    p_domain_name: String,
    p_tls_certificate: &Path,
) -> Result<Endpoint, GrpcError> {
    let pem_res = tokio::fs::read(p_tls_certificate).await;
    match pem_res {
        Ok(pem) => {
            //create endpoint
            let endpoint_res = Channel::from_shared(p_end_point.clone());
            match endpoint_res {
                Ok(endpoint) => {
                    //load tls certificate
                    let cert = Certificate::from_pem(pem);
                    //create tls config for domain
                    let tls = ClientTlsConfig::new()
                        .ca_certificate(cert)
                        .domain_name(p_domain_name);

                    //configures TLS for the endpoint
                    endpoint
                        .tls_config(tls)
                        .map_err(|e| GrpcError::InvalidClientTlsConfig(p_end_point, e))
                }
                Err(e) => Err(GrpcError::InvalidUri(p_end_point, e)),
            }
        }
        Err(e) => Err(GrpcError::InvalidTlsCert(p_end_point, e)),
    }
}

///create a tls channel for connecting to grpc server
/// # Errors
///
/// returns a Channel on success or `GrpcError` on error
pub async fn create_tls_channel(p_tls_endpoint: &Endpoint) -> Result<Channel, GrpcError> {
    const TRACE: &str = "create_tls_channel";
    //configures TLS for the endpoint
    p_tls_endpoint
        .connect()
        .await
        .map_err(|e| GrpcError::ChannelError(p_tls_endpoint.uri().to_string(), e))
}

///create a none tls channel for connecting to grpc server
/// # Errors
///
/// returns a Channel on success or `GrpcError` on error
pub async fn create_channel(p_end_point: String) -> Result<Channel, GrpcError> {
    const TRACE: &str = "create_channel";
    //create endpoint
    let endpoint_res = Channel::from_shared(p_end_point.clone());
    match endpoint_res {
        Ok(endpoint) => endpoint
            .connect()
            .await
            .map_err(|e| GrpcError::ChannelError(p_end_point, e)),
        Err(e) => Err(GrpcError::InvalidUri(p_end_point, e)),
    }
}
