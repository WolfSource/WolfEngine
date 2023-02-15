use std::{path::Path, sync::Arc};
use tokio_rustls::{
    rustls::{Certificate, PrivateKey, ServerConfig},
    TlsAcceptor,
};

#[derive(Debug, thiserror::Error)]
pub enum TlsError {
    #[error("invalid tls cert file `{0}`")]
    InvalidCertFile(String),
    #[error("invalid tls key file `{0}`")]
    InvalidPrivateKeyFile(String),
    #[error("build rustls failed because `{0}`")]
    BuildFailed(tokio_rustls::rustls::Error),
}

#[derive(Debug)]
pub enum TlsPrivateKeyType {
    PKCS8,
    RSA,
}

pub struct Tls {
    pub acceptor: Arc<TlsAcceptor>,
}

impl std::fmt::Debug for Tls {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "Tls::acceptor")
    }
}

impl Tls {
    /// # Errors
    ///
    /// TODO: add error description
    pub fn new(
        p_tls_certificate_path: Option<&Path>,
        p_tls_private_key_path: Option<&Path>,
        p_tls_private_type: Option<&TlsPrivateKeyType>,
    ) -> Result<Self, TlsError> {
        //load certificate
        let crt = p_tls_certificate_path.ok_or_else(|| TlsError::InvalidCertFile("".to_owned()))?;
        let certs = Self::load_certs(crt)?;
        if certs.is_empty() {
            return Err(TlsError::InvalidCertFile(crt));
        }

        //load private key
        let key =
            p_tls_private_key_path.ok_or_else(|| TlsError::InvalidPrivateKeyFile("".to_owned()))?;

        //load private key type
        let key_type = p_tls_private_type.ok_or_else(|| TlsError::InvalidPrivateKeyFile(key))?;

        let mut keys = Self::load_private_keys(key, key_type)?;
        if keys.is_empty() {
            return Err(TlsError::InvalidPrivateKeyFile(key));
        }

        //create tls config
        let tls_server_config = ServerConfig::builder()
            .with_safe_defaults()
            .with_no_client_auth()
            .with_single_cert(certs, keys.remove(0))
            .map_err(|e| Err(TlsError::BuildFailed(e)))?;

        // run acceptor & listener
        let tls_ = Self {
            acceptor: Arc::new(TlsAcceptor::from(Arc::new(tls_server_config))),
        };
        Ok(tls_)
    }

    /// # Errors
    ///
    /// TODO: add error description
    fn load_certs(p_path: &Path) -> Result<Vec<Certificate>, TlsError> {
        std::fs::File::open(p_path)
            .and_then(|f| {
                let mut buf = std::io::BufReader::new(f);
                rustls_pemfile::certs(&mut buf)
                    .map(|certs| certs.into_iter().map(Certificate).collect())
            })
            .map_err(|_| Err(TlsError::InvalidCertFile(p_path)))
    }

    /// # Errors
    ///
    /// TODO: add error description
    fn load_private_keys(
        p_path: &Path,
        p_type: &TlsPrivateKeyType,
    ) -> Result<Vec<PrivateKey>, TlsError> {
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
            .map_err(|_| Err(TlsError::InvalidPrivateKeyFile(p_path)))
    }
}
