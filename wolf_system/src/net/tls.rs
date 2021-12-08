use anyhow::Result;
use std::{fs::File, io::BufReader, path::Path};
use tokio_rustls::{
    rustls::{self, Certificate, OwnedTrustAnchor, PrivateKey, RootCertStore},
    webpki,
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
