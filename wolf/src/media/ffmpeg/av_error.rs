#[derive(Debug, thiserror::Error)]
pub enum AvError {
    #[error("invalid parameter")]
    InvalidParameter,
    #[error("invalid video frame size")]
    InvalidVideoFrameSize,
    #[error("invalid av config")]
    InvalidAvConfig,
    #[error("could not allocate memory")]
    OutOfMemory,
    #[error("could not fill image")]
    ImageFillFailed,
    #[error("could not create SWS context")]
    InvalidSwsContext,
    #[error("could not scale via SWS context")]
    SwsScaleFailed,
}
