// #![allow(unused_crate_dependencies)]

// use wolf::stream::{
//     ffi::rist::{
//         w_rist_config_t, w_rist_mode_W_RIST_MODE_SENDER, w_rist_profile_W_RIST_PROFILE_SIMPLE,
//     },
//     rist::*,
// };

// #[cfg(feature = "stream_rist")]
// #[test]
// fn test_rist() {
//     let mut sender = rist::new(w_rist_config_t {
//         mode: w_rist_mode_W_RIST_MODE_SENDER,
//         profile: w_rist_profile_W_RIST_PROFILE_SIMPLE,
//         loss_percent: 10,
//         timeout: 0,
//     });
//     let mut ret = sender.bind("127.0.0.1:9999".to_owned());
//     assert!(ret.is_ok());

//     ret = sender.start();
//     assert!(ret.is_ok());
// }
