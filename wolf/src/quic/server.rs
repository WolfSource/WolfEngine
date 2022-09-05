use quinn::{
    ConnectionError, Endpoint, IncomingBiStreams, IncomingUniStreams, ReadToEndError, RecvStream,
    SendStream, ServerConfig, WriteError,
};
use std::{error::Error, fmt, fs, future::Future, net::SocketAddr, pin::Pin, sync::Arc};
use {
    tokio::{
        select, signal, spawn,
        sync::watch::{channel, Receiver, Sender},
    },
    tokio_stream::{Stream, StreamExt},
};

#[derive(Debug, Clone)]
/// Different Error variants that may happend on the server
pub enum QuicServerError {
    ConnectionErr(ConnectionError),
    ReadToEndErr(ReadToEndError),
    TerminationErr(WriteError),
    WriteError(WriteError),
}
unsafe impl Send for QuicServerError {}
unsafe impl Sync for QuicServerError {}
impl fmt::Display for QuicServerError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        let display = match self {
            Self::ConnectionErr(e) => {
                format!("Connection errror: `{}`", e.to_string())
            }
            Self::ReadToEndErr(e) => {
                format!("Read-to-end errror: `{}`", e.to_string())
            }
            Self::TerminationErr(e) => {
                format!("Termination error: `{}`", e.to_string())
            }
            Self::WriteError(e) => {
                format!("Write error: `{}`", e.to_string())
            }
        };

        write!(f, "{}", display)
    }
}
impl Error for QuicServerError {}

/// Used Future extensions
pub trait FutureExt: Future {
    /// Wraps the Future in a Pinned Box
    fn boxed<'a>(self) -> Pin<Box<dyn Future<Output = Self::Output> + Send + 'a>>
    where
        Self: Sized + Send + 'a,
    {
        Box::pin(self)
    }
}

impl<T: ?Sized> FutureExt for T where T: Future {}

trait IncomigStreams: Stream + Unpin {}
impl IncomigStreams for IncomingUniStreams {}
impl IncomigStreams for IncomingBiStreams {}

trait NewIncomingStream {}
impl NewIncomingStream for Result<(SendStream, RecvStream), ConnectionError> {}
impl NewIncomingStream for Result<RecvStream, ConnectionError> {}

/// A trait for handlers (error handlers, unidirectional handlers, biodirectional handlers)
pub trait Handler<T, U>: HandlerClone<T, U> + Send + Sync + 'static {
    /// Calls the closure wrapped in a Box
    fn call(self: Box<Self>, p_data: T) -> Pin<Box<dyn Future<Output = U> + Send>>;
}

#[doc(hidden)]
pub trait HandlerClone<T, U> {
    fn clone_box(&self) -> Box<dyn Handler<T, U>>;
}

impl<T, U, K> HandlerClone<U, K> for T
where
    T: Handler<U, K> + Clone,
{
    fn clone_box(&self) -> Box<dyn Handler<U, K>> {
        Box::new(self.clone())
    }
}
// We can now implement Clone manually by forwarding to clone_box.
impl<T, U> Clone for Box<dyn Handler<T, U>> {
    fn clone(&self) -> Box<dyn Handler<T, U>> {
        self.clone_box()
    }
}
impl<F> Handler<Vec<u8>, ()> for F
where
    F: FnOnce(Vec<u8>) -> Pin<Box<dyn Future<Output = ()> + Send>> + Clone + Send + Sync + 'static,
{
    fn call(self: Box<F>, p_data: Vec<u8>) -> Pin<Box<dyn Future<Output = ()> + Send>> {
        Box::pin(async move { self(p_data).await })
    }
}
impl<F> Handler<Vec<u8>, Option<Vec<u8>>> for F
where
    F: FnOnce(Vec<u8>) -> Pin<Box<dyn Future<Output = Option<Vec<u8>>> + Send>>
        + Clone
        + Send
        + Sync
        + 'static,
{
    fn call(
        self: Box<F>,
        p_data: Vec<u8>,
    ) -> Pin<Box<dyn Future<Output = Option<Vec<u8>>> + Send>> {
        Box::pin(async move { self(p_data).await })
    }
}
impl<F> Handler<QuicServerError, ()> for F
where
    F: FnOnce(QuicServerError) -> Pin<Box<dyn Future<Output = ()> + Send>>
        + Clone
        + Send
        + Sync
        + 'static,
{
    fn call(self: Box<F>, p_data: QuicServerError) -> Pin<Box<dyn Future<Output = ()> + Send>> {
        Box::pin(async move { self(p_data).await })
    }
}
type GeneralRequestHandlerInput = (
    (IncomingUniStreams, IncomingBiStreams),
    Receiver<Option<()>>,
);
impl<F> Handler<GeneralRequestHandlerInput, ()> for F
where
    F: FnOnce(GeneralRequestHandlerInput) -> Pin<Box<dyn Future<Output = ()> + Send>>
        + Clone
        + Send
        + Sync
        + 'static,
{
    fn call(
        self: Box<F>,
        p_data: GeneralRequestHandlerInput,
    ) -> Pin<Box<dyn Future<Output = ()> + Send>> {
        Box::pin(async move { self(p_data).await })
    }
}

/// TLS certificate for the server
pub struct TlsCertificate {
    tls_crt: Vec<u8>,
    tls_key: Vec<u8>,
}
impl TlsCertificate {
    /// Makes a new TlsCertificate instance
    pub fn new<T: ToString>(p_tls_crt_path: T, p_tls_key_path: T) -> Self {
        let tls_crt = fs::read(p_tls_crt_path.to_string()).unwrap();
        let tls_key = fs::read(p_tls_key_path.to_string()).unwrap();

        Self { tls_crt, tls_key }
    }
}

type BiRequestHandlerClosure = Box<dyn Handler<Vec<u8>, Option<Vec<u8>>>>;
type GeneralRequestHandlerClosure = Box<dyn Handler<GeneralRequestHandlerInput, ()>>;
type RequestErrorHandlerClosure = Arc<Box<dyn Handler<QuicServerError, ()>>>;
type UniRequestHandlerClosure = Box<dyn Handler<Vec<u8>, ()>>;

/// Essentials configs for the Quic server
pub struct QuicServerEssentialConfigs {
    addr: SocketAddr,
    error_handler: RequestErrorHandlerClosure,
    tls_certificate: Option<TlsCertificate>,
    shutdown_channel: (Sender<Option<()>>, Receiver<Option<()>>),
}
impl QuicServerEssentialConfigs {
    /// Makes a new QuicServerEssentialConfigs instance with TLS certificate
    pub fn new<A: ToString>(
        p_addr: A,
        p_error_handler: impl Handler<QuicServerError, ()>,
        p_tls_certificate: Option<TlsCertificate>,
    ) -> Self {
        let shutdown_channel = channel::<Option<()>>(None);

        Self {
            addr: p_addr.to_string().parse().unwrap(),
            error_handler: Arc::new(Box::new(p_error_handler)),
            shutdown_channel,
            tls_certificate: p_tls_certificate,
        }
    }

    /// Makes a new QuicServerEssentialConfigs instance without TLS certificate
    pub fn new_insecure<A: ToString>(
        p_addr: A,
        p_error_handler: impl Handler<QuicServerError, ()>,
    ) -> Self {
        Self::new(p_addr, p_error_handler, None)
    }

    /// Generates config based on TLS provided
    pub fn generate_tls_config(&self) -> Result<ServerConfig, rustls::Error> {
        let cert_der: Vec<u8>;
        let priv_key: Vec<u8>;
        if let Some(TlsCertificate { tls_crt, tls_key }) = &self.tls_certificate {
            cert_der = tls_crt.to_vec();
            priv_key = tls_key.to_vec();
        } else {
            let cert = rcgen::generate_simple_self_signed(vec!["localhost".into()]).unwrap();
            cert_der = cert.serialize_der().unwrap();
            priv_key = cert.serialize_private_key_der();
        }

        let priv_key = rustls::PrivateKey(priv_key);
        let cert_chain = vec![rustls::Certificate(cert_der.clone())];
        let mut server_config = ServerConfig::with_single_cert(cert_chain, priv_key)?;

        Arc::get_mut(&mut server_config.transport).unwrap();

        Ok(server_config)
    }
}

/// A struct acting as a server for the Quic protocol
pub struct QuicServer {
    essential_cofnigs: QuicServerEssentialConfigs,
    request_handler: GeneralRequestHandlerClosure,
}
impl Drop for QuicServer {
    fn drop(&mut self) {
        let _ = self.essential_cofnigs.shutdown_channel.0.send(Some(()));
    }
}
impl QuicServer {
    fn new_raw(
        p_essentials: QuicServerEssentialConfigs,
        p_uni_request_handler: Option<UniRequestHandlerClosure>,
        p_bi_request_handler: Option<BiRequestHandlerClosure>,
    ) -> Self {
        let error_handler = Arc::clone(&p_essentials.error_handler);

        // This variable declartion may have a bit of redundent code but it has a perfomance benefit at runtime
        let request_handler: GeneralRequestHandlerClosure =
            if p_uni_request_handler.is_some() && p_bi_request_handler.is_some() {
                Box::new(
                    |(streams, mut shutdown_receiver): GeneralRequestHandlerInput| {
                        async move {
                            let shutdown_receiver2 = shutdown_receiver.clone();
                            let error_handler2 = error_handler.clone();
                            let uni = || async move {
                                if let Some(e) = Self::handle_uni_connection(
                                    streams.0,
                                    &p_uni_request_handler.unwrap(),
                                    error_handler2.clone(),
                                    shutdown_receiver2,
                                )
                                .await
                                .err()
                                {
                                    (&*Arc::clone(&error_handler2)).clone().call(e).await;
                                }
                            };
                            let shutdown_receiver3 = shutdown_receiver.clone();
                            let bi = || async move {
                                if let Some(e) = Self::handle_bi_connection(
                                    streams.1,
                                    &p_bi_request_handler.unwrap(),
                                    error_handler.clone(),
                                    shutdown_receiver3,
                                )
                                .await
                                .err()
                                {
                                    (&*Arc::clone(&error_handler)).clone().call(e).await;
                                }
                            };

                            loop {
                                select! {
                                    _ = signal::ctrl_c() => {
                                        break;
                                    },
                                    _ = shutdown_receiver.changed() => {
                                        break;
                                    },
                                    _ = uni() => {
                                        break;
                                    },
                                    _ = bi() => {
                                        break;
                                    },
                                }
                            }
                        }
                        .boxed()
                    },
                )
            } else if p_uni_request_handler.is_none() && p_bi_request_handler.is_some() {
                Box::new(
                    |(streams, mut shutdown_receiver): GeneralRequestHandlerInput| {
                        {
                            async move {
                                let shutdown_receiver2 = shutdown_receiver.clone();
                                let handler = || async move {
                                    if let Some(e) = Self::handle_bi_connection(
                                        streams.1,
                                        &p_bi_request_handler.unwrap(),
                                        error_handler.clone(),
                                        shutdown_receiver2,
                                    )
                                    .await
                                    .err()
                                    {
                                        (&*Arc::clone(&error_handler)).clone().call(e).await;
                                    }
                                };

                                loop {
                                    select! {
                                        _ = signal::ctrl_c() => {
                                            break;
                                        },
                                        _ = shutdown_receiver.changed() => {
                                            break;
                                        },
                                        _ = handler() => {
                                            break;
                                        },
                                    }
                                }
                            }
                        }
                        .boxed()
                    },
                )
            } else if p_uni_request_handler.is_some() && p_bi_request_handler.is_none() {
                Box::new(
                    |(streams, mut shutdown_receiver): GeneralRequestHandlerInput| {
                        async move {
                            let shutdown_receiver2 = shutdown_receiver.clone();
                            let handler = || async move {
                                if let Some(e) = Self::handle_uni_connection(
                                    streams.0,
                                    &p_uni_request_handler.unwrap(),
                                    error_handler.clone(),
                                    shutdown_receiver2,
                                )
                                .await
                                .err()
                                {
                                    (&*Arc::clone(&error_handler)).clone().call(e).await;
                                }
                            };

                            loop {
                                select! {
                                    _ = signal::ctrl_c() => {
                                        break;
                                    },
                                    _ = shutdown_receiver.changed() => {
                                        break;
                                    },
                                    _ = handler() => {
                                        break;
                                    },
                                }
                            }
                        }
                        .boxed()
                    },
                )
            } else {
                panic!("uni_request_handler and bi_request_handler cannot both be None!");
            };

        Self {
            essential_cofnigs: p_essentials,
            request_handler,
        }
    }

    /// Makes a new QuicServer instance which handles both biodirectional and unidirectional incoming streams
    pub fn new(
        p_essentials: QuicServerEssentialConfigs,
        p_uni_request_handler: impl Handler<Vec<u8>, ()>,
        p_bi_request_handler: impl Handler<Vec<u8>, Option<Vec<u8>>>,
    ) -> Self {
        Self::new_raw(
            p_essentials,
            Some(Box::new(p_uni_request_handler)),
            Some(Box::new(p_bi_request_handler)),
        )
    }
    /// Makes a new QuicServer instance which only handles unidirectional incoming streams
    pub fn new_uni(
        p_essentials: QuicServerEssentialConfigs,
        p_request_handler: impl Handler<Vec<u8>, ()>,
    ) -> Self {
        Self::new_raw(p_essentials, Some(Box::new(p_request_handler)), None)
    }
    /// Makes a new QuicServer instance which only handles biodirectional incoming streams
    pub fn new_bi(
        p_essentials: QuicServerEssentialConfigs,
        p_request_handler: impl Handler<Vec<u8>, Option<Vec<u8>>>,
    ) -> Self {
        Self::new_raw(p_essentials, None, Some(Box::new(p_request_handler)))
    }

    async fn handle_connection<
        T: IncomigStreams + Stream<Item = K>,
        U: Fn(K) -> UFut,
        K,
        UFut: Future<Output = Result<(), QuicServerError>>,
    >(
        mut p_streams: T,
        p_action_handler: U,
        p_error_handler: RequestErrorHandlerClosure,
        mut p_shutdown_receiver: Receiver<Option<()>>,
    ) -> Result<(), QuicServerError> {
        async {
            loop {
                select! {
                    _ = signal::ctrl_c() => {
                        break;
                    },
                    _ = p_shutdown_receiver.changed() => {
                        break;
                    },
                    stream_option = p_streams.next() => {
                        if let Some(stream) = stream_option {
                            if let Err(e) = p_action_handler(stream).await {
                                (&*Arc::clone(&p_error_handler)).clone().call(e).await;
                            }
                        }
                    },
                };
            }

            Ok(())
        }
        .await
        .map_err(|e| QuicServerError::ConnectionErr(e))?;

        Ok(())
    }

    async fn handle_uni_connection(
        p_streams: IncomingUniStreams,
        p_request_handler: &UniRequestHandlerClosure,
        p_error_handler: RequestErrorHandlerClosure,
        p_shutdown_receiver: Receiver<Option<()>>,
    ) -> Result<(), QuicServerError> {
        Self::handle_connection(
            p_streams,
            |stream| async {
                let stream = match stream {
                    Err(quinn::ConnectionError::ApplicationClosed { .. }) => {
                        // Connetion closed
                        return Ok(());
                    }
                    Err(e) => {
                        return Err(QuicServerError::ConnectionErr(e));
                    }
                    Ok(s) => s,
                };

                let recv: RecvStream = stream;

                // Reading the request
                let req = recv
                    .read_to_end(64 * 1024)
                    .await
                    .map_err(|e| QuicServerError::ReadToEndErr(e))?;

                p_request_handler.clone().call(req).await;

                Ok(())
            },
            p_error_handler,
            p_shutdown_receiver,
        )
        .await
    }

    async fn handle_bi_connection(
        p_streams: IncomingBiStreams,
        p_request_handler: &BiRequestHandlerClosure,
        p_error_handler: RequestErrorHandlerClosure,
        p_shutdown_receiver: Receiver<Option<()>>,
    ) -> Result<(), QuicServerError> {
        Self::handle_connection(
            p_streams,
            |stream| async {
                let stream = match stream {
                    Err(quinn::ConnectionError::ApplicationClosed { .. }) => {
                        // Connetion closed
                        return Ok(());
                    }
                    Err(e) => {
                        return Err(QuicServerError::ConnectionErr(e));
                    }
                    Ok(s) => s,
                };

                let (mut send, recv): (SendStream, RecvStream) = stream;

                // Reading the request
                let req = recv
                    .read_to_end(64 * 1024)
                    .await
                    .map_err(|e| QuicServerError::ReadToEndErr(e))?;

                let response = p_request_handler.clone().call(req).await;

                if let Some(response) = response {
                    // Sending a response
                    send.write_all(&response)
                        .await
                        .map_err(|e| QuicServerError::WriteError(e))?;
                }

                // Gracefully terminating the stream
                send.finish()
                    .await
                    .map_err(|e| QuicServerError::TerminationErr(e))?;

                Ok(())
            },
            p_error_handler,
            p_shutdown_receiver,
        )
        .await
    }

    /// Initiates and runs the server based on the config provided
    pub async fn init(&mut self) -> Result<(), Box<dyn Error + '_>>
    where
        Self: Send,
    {
        let (_endpoint, mut incoming) = Endpoint::server(
            self.essential_cofnigs.generate_tls_config()?,
            self.essential_cofnigs.addr,
        )?;

        loop {
            let mut shutdown_receiver = self.essential_cofnigs.shutdown_channel.1.clone();
            select! {
                _ = signal::ctrl_c() => {
                    break;
                },
                _ = shutdown_receiver.changed() => {
                    break;
                },
                incoming_option = incoming.next() => {
                    if let Some(conn) = incoming_option {
                        let cloned_handler = self.request_handler.clone();
                        let error_handler = self.essential_cofnigs.error_handler.clone();
                        let shutdown_receiver = self.essential_cofnigs.shutdown_channel.1.clone();

                        spawn(async move {
                            let error_handler_closure = |e: QuicServerError| async {
                                return (*error_handler).clone().call(e).await;
                            };

                            let conn = conn.await.map_err(|e| QuicServerError::ConnectionErr(e));
                            if let Err(e) = conn {
                                return error_handler_closure(e).await;
                            }

                            match conn {
                                Ok(quinn::NewConnection {
                                    uni_streams,
                                    bi_streams,
                                    ..
                                }) => {
                                    cloned_handler.call(((uni_streams, bi_streams), shutdown_receiver)).await;
                                }
                                Err(e) => {
                                    error_handler_closure(e).await;
                                }
                            }
                        });
                    }
                },
            };
        }

        Ok(())
    }
}
