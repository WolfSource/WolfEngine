use std::time::Duration;
use tokio::{
    sync::mpsc::{self, Receiver, Sender},
    task::JoinHandle,
};
pub use wolf::quic::{
    client::QuicClient,
    server::{FutureExt, QuicServer, QuicServerError, QuicServerEssentialConfigs},
};

const MAX_CHUNK_SIZE: usize = 100;
const LOREM_IPSUM : &str = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Tellus molestie nunc non blandit massa. Ut sem nulla pharetra diam sit amet nisl suscipit. Turpis massa sed elementum tempus egestas. Fusce ut placerat orci nulla pellentesque. Ultrices dui sapien eget mi proin sed. Urna condimentum mattis pellentesque id. Velit sed ullamcorper morbi tincidunt ornare. Amet nisl purus in mollis nunc sed id semper risus. Arcu ac tortor dignissim convallis aenean et. Erat nam at lectus urna duis convallis convallis. Eu volutpat odio facilisis mauris sit amet massa vitae. Vestibulum lectus mauris ultrices eros in cursus turpis. Diam vulputate ut pharetra sit amet aliquam id. Ac turpis egestas sed tempus urna et. Habitasse platea dictumst quisque sagittis. Porta lorem mollis aliquam ut porttitor leo a diam. Id nibh tortor id aliquet lectus proin.";

fn read_by_chunks<T: ToString>(string: T) -> Vec<Vec<u8>> {
    string
        .to_string()
        .as_bytes()
        .chunks(MAX_CHUNK_SIZE)
        .map(Vec::from)
        .collect::<Vec<Vec<u8>>>()
}

fn run_mpsc_channel() -> (Sender<String>, Receiver<String>) {
    mpsc::channel::<String>(1)
}
fn get_string_catcher(mut rx: Receiver<String>) -> JoinHandle<String> {
    tokio::spawn(async move {
        let mut gotten_string = String::new();

        loop {
            if let Ok(contents) = rx.try_recv() {
                if contents.as_str() != "done" {
                    gotten_string.push_str(contents.as_str());
                } else {
                    break;
                }
            }
        }

        gotten_string
    })
}
async fn run_client<A: ToString>(addr: A, gotten_string: JoinHandle<String>, bio_send: bool) {
    let mut client = QuicClient::new_insecure(addr, String::from("localhost"), 3);
    client.connect().await.unwrap();

    for chunk in read_by_chunks(LOREM_IPSUM) {
        if bio_send {
            let resp = client.bio_send(chunk.clone()).await;
            if let Ok(resp) = resp {
                assert_eq!(chunk, resp);
            }
        } else {
            let _ = client.uni_send(chunk).await;
        }
    }

    let done_message = String::from("done").into_bytes();
    if bio_send {
        let _ = client.bio_send(done_message).await;
    } else {
        let _ = client.uni_send(done_message).await;
    }

    assert_eq!(gotten_string.await.unwrap().as_str(), LOREM_IPSUM);

    client.disconnect().await;
}

async fn server_bio_request_handler(tx: Sender<String>, req: Vec<u8>) -> Option<Vec<u8>> {
    let recv = String::from_utf8(req).unwrap();

    println!("Recieved: `{}`", recv.as_str());

    loop {
        if tx.send(recv.clone()).await.is_ok() {
            break;
        } else {
            tokio::time::sleep(Duration::from_millis(100)).await;
        }
    }

    Some(recv.into_bytes())
}
async fn server_uni_request_handler(tx: Sender<String>, req: Vec<u8>) {
    server_bio_request_handler(tx, req).await;
}
async fn server_error_handler(e: QuicServerError) {
    eprintln!("Server Error: {:#?}", e)
}

#[tokio::test(flavor = "multi_thread")]
async fn test_bio_directional() {
    const QUIC_ADDR: &str = "127.0.0.1:5000";

    let (tx, rx) = run_mpsc_channel();
    let client = || async {
        let string_catcher = get_string_catcher(rx);
        run_client(QUIC_ADDR, string_catcher, true).await;
    };
    let server = || async {
        let request_handler =
            |req| async move { server_bio_request_handler(tx, req).await }.boxed();
        let error_handler = |e| async move { server_error_handler(e).await }.boxed();

        let mut server = QuicServer::new_bi(
            QuicServerEssentialConfigs::new_insecure(QUIC_ADDR, error_handler),
            request_handler,
        );
        server.init().await.unwrap();
    };

    tokio::select! {
        val = server() => {
            println!("Server completed first with {:?}", val);
        }
        val = client() => {
            println!("Client completed first with {:?}", val);
        }
    };
}

#[tokio::test(flavor = "multi_thread")]
async fn test_uni_directional() {
    const QUIC_ADDR: &str = "127.0.0.1:5001";

    let (tx, rx) = run_mpsc_channel();
    let client = || async {
        let string_catcher = get_string_catcher(rx);
        run_client(QUIC_ADDR, string_catcher, false).await;
    };
    let server = || async {
        let request_handler =
            |req| async move { server_uni_request_handler(tx, req).await }.boxed();
        let error_handler = |e| {
            async move {
                server_error_handler(e).await;
            }
            .boxed()
        };

        async move {
            let mut server = QuicServer::new_uni(
                QuicServerEssentialConfigs::new_insecure(QUIC_ADDR, error_handler),
                request_handler,
            );
            server.init().await.unwrap();
        }
        .await;
    };

    tokio::select! {
        val = server() => {
            println!("Server completed first with {:?}", val);
        }
        val = client() => {
            println!("Client completed first with {:?}", val);
        }
    };
}

#[tokio::test(flavor = "multi_thread")]
async fn test_both_directions() {
    const QUIC_ADDR: &str = "127.0.0.1:5002";

    let (tx, rx) = run_mpsc_channel();
    let (tx2, rx2) = run_mpsc_channel();
    let client = || async {
        let string_catcher = get_string_catcher(rx);
        run_client(QUIC_ADDR, string_catcher, false).await;

        let string_catcher2 = get_string_catcher(rx2);
        run_client(QUIC_ADDR, string_catcher2, true).await;
    };
    let server = || async {
        let uni_request_handler =
            |req| async move { server_uni_request_handler(tx, req).await }.boxed();
        let bi_request_handler =
            |req| async move { server_bio_request_handler(tx2, req).await }.boxed();
        let error_handler = |e| async move { server_error_handler(e).await }.boxed();

        let _ = tokio::spawn(async move {
            let mut server = QuicServer::new(
                QuicServerEssentialConfigs::new_insecure(QUIC_ADDR, error_handler),
                uni_request_handler,
                bi_request_handler,
            );
            server.init().await.unwrap();
        })
        .await;
    };

    tokio::select! {
        val = server() => {
            println!("Server completed first with {:?}", val);
        }
        val = client() => {
            println!("Client completed first with {:?}", val);
        }
    };
}
