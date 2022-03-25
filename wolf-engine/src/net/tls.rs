use anyhow::{anyhow, ensure, Result};
use std::{fs::File, io::BufReader, path::Path, sync::Arc};
use tokio_rustls::{
    rustls::{self, Certificate, OwnedTrustAnchor, PrivateKey, RootCertStore, ServerConfig},
    webpki, TlsAcceptor,
};

#[derive(Debug)]
pub enum TlsPrivateKeyType {
    PKCS8,
    RSA,
}

pub fn load_certs(p_path: &Path) -> Result<Vec<Certificate>> {
    std::fs::File::open(p_path)
        .and_then(|f| {
            let mut buf = std::io::BufReader::new(f);
            rustls_pemfile::certs(&mut buf)
                .map(|mut certs| certs.drain(..).map(Certificate).collect())
        })
        .map_err(std::convert::Into::into) //|e| e.into()
}

pub fn load_private_keys(p_path: &Path, p_type: &TlsPrivateKeyType) -> Result<Vec<PrivateKey>> {
    std::fs::File::open(p_path)
        .and_then(|f| {
            let mut buf = std::io::BufReader::new(f);
            match p_type {
                TlsPrivateKeyType::PKCS8 => rustls_pemfile::pkcs8_private_keys(&mut buf)
                    .map(|mut keys| keys.drain(..).map(PrivateKey).collect()),
                TlsPrivateKeyType::RSA => rustls_pemfile::rsa_private_keys(&mut buf)
                    .map(|mut keys| keys.drain(..).map(PrivateKey).collect()),
            }
        })
        .map_err(std::convert::Into::into) //e.into()
}

/// Load root certificate from specific path
/// # Panics
///
/// Will panic if cert is not valid
pub fn load_root_ca(p_root_ca_path: Option<&Path>) -> Result<RootCertStore> {
    let mut root_cert_store = rustls::RootCertStore::empty();

    if let Some(ca_path) = p_root_ca_path {
        let mut pem = BufReader::new(File::open(ca_path)?);
        let certs = rustls_pemfile::certs(&mut pem)?;
        let trust_anchors = certs.iter().map(|cert| {
            let ta = webpki::TrustAnchor::try_from_cert_der(&cert[..]).unwrap();
            OwnedTrustAnchor::from_subject_spki_name_constraints(
                ta.subject,
                ta.spki,
                ta.name_constraints,
            )
        });
        root_cert_store.add_server_trust_anchors(trust_anchors);
    } else {
        root_cert_store.add_server_trust_anchors(webpki_roots::TLS_SERVER_ROOTS.0.iter().map(
            |ta| {
                OwnedTrustAnchor::from_subject_spki_name_constraints(
                    ta.subject,
                    ta.spki,
                    ta.name_constraints,
                )
            },
        ));
    }

    Ok(root_cert_store)
}

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
        .map_err(|err| std::io::Error::new(std::io::ErrorKind::InvalidInput, err))?;

    // run acceptor & listener
    let tls_acceptor = TlsAcceptor::from(Arc::new(tls_server_config));
    Ok(tls_acceptor)
}
