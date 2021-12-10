#![allow(missing_debug_implementations)]

use core::future::Future;
use std::pin::Pin;

pub struct SigSlot {
    slots: Vec<Pin<Box<dyn Future<Output = ()> + Send + 'static>>>,
}

unsafe impl Send for SigSlot {}
unsafe impl Sync for SigSlot {}

impl Default for SigSlot {
    fn default() -> Self {
        Self::new()
    }
}

impl SigSlot {
    #[must_use]
    pub fn new() -> Self {
        Self { slots: Vec::new() }
    }

    pub fn connect<T>(&mut self, p_slot: T)
    where
        T: Future<Output = ()> + Send + 'static,
    {
        self.slots.push(Box::pin(p_slot));
    }

    pub async fn emit(&mut self) {
        for s in &mut self.slots {
            s.await;
        }
    }
}

#[allow(clippy::semicolon_if_nothing_returned)]
#[tokio::main]
#[test]
async fn tests() {
    let mut s = SigSlot::new();
    s.connect(async {
        println!("slot 1 just called from main thread");
    });
    s.connect(async {
        println!("slot 2 just called from main thread");
    });

    let _r = tokio::spawn(async move {
        s.connect(async {
            println!("slot 3 just called from tokio thread");
        });
        s.emit().await;
    })
    .await;
}