use crate::error::WError;
use std::path::Path;
use tonic::transport::{Certificate, Channel, ClientTlsConfig, Endpoint};

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

/// # Errors
///
/// returns `WError` on error
pub async fn run_server<S, F>(
    p_config: &GrpcServerConfig<'_>,
    p_service: S,
    p_shutdown_signal: F,
) -> Result<(), WError>
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
        .map_err(|_e| WError::StreamGrpcAddrressParseFailed)?;

    let mut builder = tonic::transport::Server::builder()
        .tcp_nodelay(p_config.tcp_no_delay)
        .tcp_keepalive(p_config.tcp_keep_alive)
        .http2_keepalive_interval(p_config.http2_keepalive_interval)
        .http2_keepalive_timeout(p_config.http2_keepalive_timeout)
        .accept_http1(p_config.accept_http1);

    if p_config.tls {
        use tonic::transport::Identity;
        use tonic::transport::ServerTlsConfig;

        let default_path = Path::new("");
        //load certificate and private key for tls
        let cert_path = p_config.tls_certificate_path.unwrap_or(default_path);
        let key_path = p_config.tls_private_key_path.unwrap_or(default_path);

        if !cert_path.exists() {
            return Err(WError::StreamGrpcTlsCrtNotFound);
        }
        if !key_path.exists() {
            return Err(WError::StreamGrpcTlsKeyNotFound);
        }

        //load certificates
        let cert = tokio::fs::read(cert_path)
            .await
            .map_err(|_e| WError::StreamGrpcTlsCrtNotFound)?;

        let key = tokio::fs::read(key_path)
            .await
            .map_err(|_e| WError::StreamGrpcTlsCrtNotFound)?;

        let identity = Identity::from_pem(cert, key);
        let server_tls_config = ServerTlsConfig::new().identity(identity);

        builder = builder
            .tls_config(server_tls_config)
            .map_err(|_e| WError::StreamGrpcServerBuildFailed)?;
    }

    builder
        .add_service(p_service)
        .serve_with_shutdown(addr, p_shutdown_signal)
        .await
        .map_err(|_e| WError::StreamGrpcServerBuildFailed)
}

///create a tls grpc endpoint
/// # Errors
///
/// returns `WError` on error
pub async fn create_tls_endpoint(
    p_end_point: String,
    p_domain_name: String,
    p_tls_certificate: &Path,
) -> Result<Endpoint, WError> {
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
                        .map_err(|_e| WError::StreamGrpcInvalidTlsConfig)
                }
                Err(_e) => Err(WError::StreamGrpcServerInvalidUri),
            }
        }
        Err(_e) => Err(WError::StreamGrpcInvalidTlsConfig),
    }
}

///create a tls channel for connecting to grpc server
/// # Errors
///
/// returns `WError` on error
pub async fn create_tls_channel(p_tls_endpoint: &Endpoint) -> Result<Channel, WError> {
    //configures TLS for the endpoint
    p_tls_endpoint
        .connect()
        .await
        .map_err(|_| WError::StreamGrpcChannelError)
}

///create a none tls channel for connecting to grpc server
/// # Errors
///
/// returns `WError` on error
pub async fn create_channel(p_end_point: String) -> Result<Channel, WError> {
    //create endpoint
    let endpoint_res = Channel::from_shared(p_end_point.clone());
    match endpoint_res {
        Ok(endpoint) => endpoint
            .connect()
            .await
            .map_err(|_e| WError::StreamGrpcChannelError),
        Err(_e) => Err(WError::StreamGrpcServerInvalidUri),
    }
}
