#[cfg(feature = "stream_grpc")]
#[tokio::test]
async fn test_raft_over_grpc() {
    use std::time::Duration;
    use wolf::stream::http::grpc::*;
    use wolf::system::algorithm::raft::raft_srv::get_service;

    let (s, r) = tokio::sync::oneshot::channel();
    let _r = tokio::spawn(async {
        tokio::time::sleep(Duration::from_secs(3)).await;
        println!("GRPC server is going to shutdown");
        let _r = s.send(());
    });

    //finally run raft over grpc which block current thread
    let config = GrpcServerConfig {
        address: "0.0.0.0",
        port: 7777,
        tls: false,
        tls_certificate_path: None,
        tls_private_key_path: None,
        tcp_no_delay: true,
        tcp_keep_alive: None,
        http2_keepalive_interval: None,
        http2_keepalive_timeout: None,
        accept_http1: true,
    };

    let srv = get_service("wolf", 1);
    let ret = run_server(&config, srv, async {
        let _r = r.await.ok();
        println!("GRPC server shutdown successfully");
    })
    .await;
    assert!(ret.is_ok(), "{:?}", ret);
}
