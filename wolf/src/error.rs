#[repr(C)]
#[derive(Debug, thiserror::Error, Eq, PartialEq)]
pub enum WError {
    #[error("no error")]
    None,
    #[error("could not cast to char")]
    CharCastError,
    #[error("could not cast to int")]
    IntCastError,
    #[error("invalid parameter")]
    InvalidParameter,
    #[cfg(feature = "media_audio")]
    #[error("Audio build stream failed")]
    MediaAudioBuildStreamFailed,
    #[cfg(feature = "media_audio")]
    #[error("Audio host was not avaiable")]
    MediaAudioHostWasNotFound,
    #[cfg(feature = "media_audio")]
    #[cfg(feature = "media_audio")]
    #[error("Audio play stream failed")]
    MediaAudioPlayStreamFailed,
    #[error("Audio stream config was not found")]
    MediaAudioStreamConfigNotFound,
    #[cfg(feature = "media_audio")]
    #[error("Audio supported input config was not found")]
    MediaAudioSupportedInputConfigNotFound,
    #[cfg(feature = "media_ffmpeg")]
    #[error("ffmpeg could not open AvCodec")]
    MediaAvCodecOpenFailed,
    #[cfg(feature = "media_ffmpeg")]
    #[error("ffmpeg could not allocate memory for AvFormatContext")]
    MediaAvFormatContextAllocFailed,
    #[cfg(feature = "media_ffmpeg")]
    #[error("ffmpeg AVCodec was not found")]
    MediaCodecNotFound,
    #[cfg(feature = "media_ffmpeg")]
    #[error("ffmpeg could not receive packet while decoding")]
    MediaDecodeReceivePacketFailed,
    #[cfg(feature = "media_ffmpeg")]
    #[error("ffmpeg could not send frame while decoding")]
    MediaDecodeSendFrameFailed,
    #[cfg(feature = "media_ffmpeg")]
    #[error("ffmpeg could not receive packet while encoding")]
    MediaEncodeReceivePacketFailed,
    #[cfg(feature = "media_ffmpeg")]
    #[error("ffmpeg could not send frame while encoding")]
    MediaEncodeSendFrameFailed,
    #[cfg(feature = "media_ffmpeg")]
    #[error("ffmpeg could not fill the image")]
    MediaImageFillFailed,
    #[cfg(feature = "media_ffmpeg")]
    #[error("ffmpeg could not initialize the parser")]
    MediaInitParserFailed,
    #[cfg(feature = "media_ffmpeg")]
    #[error("ffmpeg invalid av config")]
    MediaInvalidAvConfig,
    #[cfg(feature = "media_ffmpeg")]
    #[error("ffmpeg invalid av set option")]
    MediaInvalidAvSetOption,
    #[cfg(feature = "media_ffmpeg")]
    #[error("ffmpeg could not create SWS context")]
    MediaInvalidSwsContext,
    #[cfg(feature = "media_ffmpeg")]
    #[error("ffmpeg has an invalid video frame size")]
    MediaInvalidVideoFrameSize,
    #[cfg(feature = "media_ffmpeg")]
    #[error("ffmpeg could not parse the media frame")]
    MediaParserFailed,
    #[cfg(feature = "media_ffmpeg")]
    #[error("ffmpeg could not get stream info from the media")]
    MediaStreamInfoNotFound,
    #[cfg(feature = "media_ffmpeg")]
    #[error("ffmpeg could not scale frame via SWS context")]
    MediaSwsScaleFailed,
    #[cfg(feature = "media_ffmpeg")]
    #[error("ffmpeg media URL was invalid")]
    MediaUrlInvalid,
    #[error("memory was not initialized")]
    MemoryWasNotInitialized,
    #[error("could not allocate memory")]
    OutOfMemory,
    #[error("could not cast to usize or isize")]
    SizeCastError,
    #[cfg(feature = "stream_grpc")]
    #[error("grpc address parser was failed")]
    StreamGrpcAddrressParseFailed,
    #[cfg(feature = "stream_grpc")]
    #[error("grpc channel to the endpoint could not create")]
    StreamGrpcChannelError,
    #[cfg(feature = "stream_grpc")]
    #[error("grpc endpoint has an invalid TLS config")]
    StreamGrpcInvalidTlsConfig,
    #[cfg(feature = "stream_grpc")]
    #[error("grpc endpoint doesn't have TLS config")]
    StreamGrpcMissingTlsConfig,
    #[cfg(feature = "stream_grpc")]
    #[error("grpc server could not build")]
    StreamGrpcServerBuildFailed,
    #[cfg(feature = "stream_grpc")]
    #[error("invalid grpc endpoint")]
    StreamGrpcServerInvalidUri,
    #[cfg(feature = "stream_grpc")]
    #[error("grpc TLS certificate file was not found")]
    StreamGrpcTlsCrtNotFound,
    #[cfg(feature = "stream_grpc")]
    #[error("grpc TLS key file was not found")]
    StreamGrpcTlsKeyNotFound,
    #[cfg(feature = "system_lz4")]
    #[error("lz4 has an invalid destination size")]
    SystemLZ4InvalidDestinationSize,
    #[cfg(feature = "system_lz4")]
    #[error("lz4 has an invalid source size")]
    SystemLZ4InvalidSourceSize,
    #[cfg(feature = "system_lzma")]
    #[error("lzma compression failed")]
    SystemLZMACompressionFailed,
    #[cfg(feature = "system_lzma")]
    #[error("lzma has an invalid destination size")]
    SystemLZMAInvalidDestinationSize,
    #[cfg(feature = "system_lzma")]
    #[error("lzma has an invalid source size")]
    SystemLZMAInvalidSourceSize,
    #[cfg(any(
        feature = "system_socket_client",
        feature = "system_socket_server",
        feature = "stream_quic"
    ))]
    #[error("system could not create poll for socket")]
    SystemPollFailed,
    #[cfg(any(
        feature = "system_socket_client",
        feature = "system_socket_server",
        feature = "stream_quic"
    ))]
    #[error("system could not register the poll for socket")]
    SystemPollRegisterFailed,
    #[cfg(any(feature = "system_raft"))]
    #[error("system raft append enteries with error response")]
    SystemRaftAppendEntriesErrorResponse,
    #[cfg(any(feature = "system_raft"))]
    #[error("system raft append enteries failed")]
    SystemRaftAppendEntriesFailed,
    #[cfg(any(feature = "system_raft"))]
    #[error("system raft append enteries create channel failed")]
    SystemRaftAppendEntriesCreateChannelFailed,
    #[cfg(any(feature = "system_raft"))]
    #[error("system raft append enteries have no response")]
    SystemRaftAppendEntriesNoResponse,
    #[cfg(any(
        feature = "system_socket_client",
        feature = "system_socket_server",
        feature = "stream_quic"
    ))]
    #[error("system could not parse the socket address")]
    SystemSocketAddressParseFailed,
    #[cfg(any(
        feature = "system_socket_client",
        feature = "system_socket_server",
        feature = "stream_quic"
    ))]
    #[error("system could not bind to the socket")]
    SystemSocketBindFailed,
    #[cfg(any(feature = "system_socket_client", feature = "system_socket_server"))]
    #[error("system socket could not connect to the address")]
    SystemSocketConnectFailed,
    #[cfg(any(feature = "system_socket_client", feature = "system_socket_server"))]
    #[error("system socket could not connect to the server")]
    SystemSocketConnectTimeout,
    #[cfg(any(feature = "system_socket_client", feature = "system_socket_server"))]
    #[error("system socket could not load local address")]
    SystemSocketLoadLocalAddressFailed,
    #[cfg(any(feature = "system_socket_client", feature = "system_socket_server"))]
    #[error("system socket could not accept ssl socket stream")]
    SystemSocketSSLAcceptFailed,
    #[cfg(any(feature = "system_socket_client", feature = "system_socket_server"))]
    #[error("system socket could not create SSL builder")]
    SystemSocketSSLBuilderFailed,
    #[cfg(any(feature = "system_socket_client", feature = "system_socket_server"))]
    #[error("system socket could not connect to the ssl stream")]
    SystemSocketSSLConnectFailed,
    #[cfg(any(feature = "system_socket_client", feature = "system_socket_server"))]
    #[error("system socket could not success on ssl handshake")]
    SystemSocketSSLHandshakeTimeout,
    #[cfg(any(feature = "system_socket_client", feature = "system_socket_server"))]
    #[error("system socket could not convert UTF8")]
    SystemSocketUtf8Error,
    #[cfg(any(feature = "system_socket_client", feature = "system_socket_server"))]
    #[error("system socket could not read from the tcp socket")]
    SystemTCPSocketReadFailed,
    #[cfg(any(feature = "system_socket_client", feature = "system_socket_server"))]
    #[error("system socket could not write to the tcp socket")]
    SystemTCPSocketWriteFailed,
    #[cfg(any(feature = "system_socket_client", feature = "system_socket_server"))]
    #[error("system socket could not read from the udp socket")]
    SystemUDPSocketReadFailed,
    #[cfg(any(feature = "system_socket_client", feature = "system_socket_server"))]
    #[error("system socket could not write to the udp socket")]
    SystemUDPSocketWriteFailed,
    #[error("unknown error")]
    Unknown,
}
