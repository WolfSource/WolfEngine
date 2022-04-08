// #![allow(missing_debug_implementations)]

// use std::future::Future;
// use std::pin::Pin;
// //use std::sync::Arc;

// /// Signal Slot manager
// ///
// /// # Examples
// ///
// /// ```
// /// use wolf_system::compression::lzma;
// ///
// /// let content = "HELLO WOLF!".as_bytes();
// /// let mut trace = String::new();
// /// let compress = lzma::ffi::compress(content, lzma::ffi::LZMAType::LZMA1, 5, &mut trace);
// ///
// /// println!("{:?}", compress);
// /// ```
// pub struct WSigSlot {
//     slots: Vec<Pin<Box<dyn Future<Output = ()> + Send + 'static>>>,
// }

// unsafe impl Send for WSigSlot {}
// unsafe impl Sync for WSigSlot {}

// impl Default for WSigSlot {
//     fn default() -> Self {
//         Self::new()
//     }
// }

// impl Clone for WSigSlot {
//     fn clone(&self) -> Self {
//         let x = async move {};
//         let b = x.clone();

//         Self {
//             slots: self.slots.clone(),
//         }
//     }
// }

// impl WSigSlot {
//     #[must_use]
//     pub fn new() -> Self {
//         Self { slots: Vec::new() }
//     }

//     pub fn connect<F>(&mut self, p_slot: F)
//     where
//         F: Future<Output = ()> + Send + 'static,
//     {
//         let b = Box::pin(p_slot);
//         self.slots.push(b);
//     }

//     pub async fn emit(&mut self) {
//         //TODO: run all slots in parallel mode
//         for s in &mut self.slots {
//             s.await;
//         }
//     }
// }

// //#[allow(clippy::semicolon_if_nothing_returned)]
// #[tokio::test]
// async fn test() {
//     use crate::system::os::w_runtime::WRuntime;
//     use crate::w_log;

//     let mut s = WSigSlot::new();
//     s.connect(async {
//         w_log!("slot 1 just called from main thread");
//     });
//     s.connect(async {
//         w_log!("slot 2 just called from main thread");
//     });
//     //from a green thread
//     let _j = WRuntime::green_thread(async move {
//         s.connect(async {
//             w_log!("slot 4 just called from a green thread");
//         });
//         s.emit().await;
//     })
//     .await;
// }
