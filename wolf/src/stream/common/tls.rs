use anyhow::{anyhow, ensure, Result};
use std::{path::Path, sync::Arc};
use tokio_rustls::{
    rustls::{Certificate, PrivateKey, ServerConfig},
    TlsAcceptor,
};

#[derive(Debug)]
pub enum TlsPrivateKeyType {
    PKCS8,
    RSA,
}

/// # Errors
///
/// TODO: add error description
pub fn load_certs(p_path: &Path) -> Result<Vec<Certificate>> {
    std::fs::File::open(p_path)
        .and_then(|f| {
            let mut buf = std::io::BufReader::new(f);
            rustls_pemfile::certs(&mut buf)
                .map(|certs| certs.into_iter().map(Certificate).collect())
        })
        .map_err(|e| anyhow!("could not load certs {:?} because of {:?}", p_path, e))
}

/// # Errors
///
/// TODO: add error description
pub fn load_private_keys(p_path: &Path, p_type: &TlsPrivateKeyType) -> Result<Vec<PrivateKey>> {
    std::fs::File::open(p_path)
        .and_then(|f| {
            let mut buf = std::io::BufReader::new(f);
            match p_type {
                TlsPrivateKeyType::PKCS8 => rustls_pemfile::pkcs8_private_keys(&mut buf)
                    .map(|keys| keys.into_iter().map(PrivateKey).collect()),
                TlsPrivateKeyType::RSA => rustls_pemfile::rsa_private_keys(&mut buf)
                    .map(|keys| keys.into_iter().map(PrivateKey).collect()),
            }
        })
        .map_err(|e| {
            anyhow!(
                "could not load private keys {:?} with type {:?} because of {:?}",
                p_path,
                p_type,
                e
            )
        })
}

/// # Errors
///
/// TODO: add error description
pub fn init_tls_acceptor(
    p_tls_certificate_path: Option<&Path>,
    p_tls_private_key_path: Option<&Path>,
    p_tls_private_type: Option<&TlsPrivateKeyType>,
) -> Result<TlsAcceptor> {
    //tls-mode
    let err_msg = "Invalid Parameters for tcp::run_server";

    //load certificate
    let crt = p_tls_certificate_path.ok_or_else(|| {
        anyhow!(err_msg).context("p_tls_certificate_path not provided for tcp server")
    })?;
    let certs = load_certs(crt)?;
    ensure!(certs.is_empty(), "missing certificate for TLS tcp server");

    //load private key
    let key = p_tls_private_key_path.ok_or_else(|| {
        anyhow!(err_msg).context("p_tls_private_key_path not provided for tcp server")
    })?;
    //load private key type
    let key_type = p_tls_private_type.ok_or_else(|| {
        anyhow!(err_msg).context("p_tls_private_type not provided for tcp server")
    })?;
    let mut keys = load_private_keys(key, key_type)?;
    ensure!(keys.is_empty(), "missing private key for TLS tcp server");

    //create tls config
    let tls_server_config = ServerConfig::builder()
        .with_safe_defaults()
        .with_no_client_auth()
        .with_single_cert(certs, keys.remove(0))
        .map_err(|e| anyhow!("could not build tls server config because of {:?}", e))?;

    // run acceptor & listener
    let tls_acceptor = TlsAcceptor::from(Arc::new(tls_server_config));
    Ok(tls_acceptor)
}
