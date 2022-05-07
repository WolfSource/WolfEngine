// #[cxx::bridge(namespace = "rist_cxx")]
// pub mod ffi {
//     unsafe extern "C++" {
//         include!("src/stream/_cxx/rist/Rist.hpp");

//         type Rist;

//         pub fn init(&self);
//         // pub fn init(
//         //     self: Pin<&mut Rist>,
//         //     p_profile: i32,
//         //     p_url: &str,
//         //     p_error_msg: &mut String,
//         // ) -> i32;

//         pub fn New(p_name: &str) -> UniquePtr<Rist>;
//     }
// }

// #[tokio::test]
// async fn test() {
//     let mut error_msg = "".to_owned();
//     let rist_inst = ffi::New("reciever");
//     rist_inst.init();
//     // let pin_mut = rist_inst.pin_mut();
//     // let ret = pin_mut.init(0, "", &mut error_msg);
//     // if ret == 0 {
//     //     println!("no error");
//     // }
//     println!("here");
// }
