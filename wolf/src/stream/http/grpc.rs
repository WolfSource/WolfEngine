use anyhow::{bail, Result};
use std::path::Path;
use tonic::transport::{Certificate, Channel, ClientTlsConfig, Endpoint};

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
/// Will return `Err` if `p_config` does not provide TLS certificates or
/// grpc server could not build.
pub async fn run_server<S, F>(
    p_config: &GrpcServerConfig<'_>,
    p_service: S,
    p_shutdown_signal: F,
) -> Result<()>
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
    const TRACE: &str = "run_server";

    let addr = format!("{}:{}", p_config.address, p_config.port).parse()?;

    if p_config.tls {
        use tonic::transport::Identity;
        use tonic::transport::ServerTlsConfig;

        let p = Path::new("");
        //load certificate and private key for tls
        let cert_path = p_config.tls_certificate_path.unwrap_or(p);
        let key_path = p_config.tls_private_key_path.unwrap_or(p);

        if !cert_path.exists() {
            anyhow::bail!(
                "path to the tls certificate was not provided. trace {}",
                TRACE
            )
        }
        if !key_path.exists() {
            bail!(
                "path to the tls private key was not provided. trace {}",
                TRACE
            )
        }

        //load certificates
        let cert = tokio::fs::read(cert_path).await?;
        let key = tokio::fs::read(key_path).await?;
        let identity = Identity::from_pem(cert, key);
        let server_tls_config = ServerTlsConfig::new().identity(identity);

        tonic::transport::Server::builder()
            .tcp_nodelay(p_config.tcp_no_delay)
            .tcp_keepalive(p_config.tcp_keep_alive)
            .http2_keepalive_interval(p_config.http2_keepalive_interval)
            .http2_keepalive_timeout(p_config.http2_keepalive_timeout)
            .accept_http1(p_config.accept_http1)
            .tls_config(server_tls_config)?
            .add_service(p_service)
            .serve_with_shutdown(addr, p_shutdown_signal)
            .await
            .map_err(|e| {
                anyhow::anyhow!("could not build grpc tls server because {:?}", e).context(TRACE)
            })
    } else {
        tonic::transport::Server::builder()
            .tcp_nodelay(p_config.tcp_no_delay)
            .tcp_keepalive(p_config.tcp_keep_alive)
            .http2_keepalive_interval(p_config.http2_keepalive_interval)
            .http2_keepalive_timeout(p_config.http2_keepalive_timeout)
            .accept_http1(p_config.accept_http1)
            .add_service(p_service)
            .serve_with_shutdown(addr, p_shutdown_signal)
            .await
            .map_err(|e| {
                anyhow::anyhow!("could not build grpc tls server because {:?}", e).context(TRACE)
            })
    }
}

///create a tls grpc endpoint
pub async fn create_tls_endpoint(
    p_end_point: String,
    p_domain_name: String,
    p_tls_certificate: &Path,
) -> Result<Endpoint> {
    const TRACE: &str = "create_grpc_channel";
    let pem_res = tokio::fs::read(p_tls_certificate).await;
    let res = match pem_res {
        Ok(pem) => {
            //create endpoint
            let endpoint_res = Channel::from_shared(p_end_point.clone());
            let res = match endpoint_res {
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
                        .or_else(|e| {
                            bail!(
                                "could not create grpc tls_config for endpoint {} because: {:?}. trace : {}",
                                p_end_point,
                                e,
                                TRACE
                            )
                        })
                }
                Err(e) => bail!(
                    "grpc endpoint uri {} is invalid because: {:?}. trace: {}",
                    p_end_point,
                    e,
                    TRACE
                ),
            };
            res
        }
        Err(e) => bail!(
            "could not load certificate for grpc client because: {:?}. trace: {}",
            e,
            TRACE
        ),
    };
    res
}

///create a tls channel for connecting to grpc server
pub async fn create_tls_channel(p_tls_endpoint: &Endpoint) -> Result<Channel> {
    const TRACE: &str = "create_tls_channel";
    //configures TLS for the endpoint
    let ret = p_tls_endpoint
        .connect()
        .await
        .and_then(|channel| {
            //return channel
            anyhow::Result::Ok(channel)
        })
        .or_else(|e| {
            bail!(
                "could not create channel for grpc on endpoint {} because: {:?}. trace: {}",
                p_tls_endpoint.uri().to_string(),
                e,
                TRACE
            )
        });
    ret
}

///create a none tls channel for connecting to grpc server
pub async fn create_channel(p_end_point: String) -> Result<Channel> {
    const TRACE: &str = "create_channel";
    //create endpoint
    let endpoint_res = Channel::from_shared(p_end_point.clone());
    let res = match endpoint_res {
        Ok(endpoint) => {
            endpoint
                .connect()
                .await
                .and_then(|channel| {
                    //return channel
                    anyhow::Result::Ok(channel)
                })
                .or_else(|e| {
                    bail!(
                        "could not create channel for grpc client on endpoint {} because: {:?}. trace: {}",
                        p_end_point,
                        e,
                        TRACE
                    )
                })
        }
        Err(e) => bail!(
            "endpoint uri {} is invalid because: {:?}. trace: {}",
            p_end_point,
            e,
            TRACE
        ),
    };
    res
}
