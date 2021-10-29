#[cxx::bridge(namespace = "lzma_cxx")]
pub mod ffi {
    #[derive(Debug)]
    struct LZMAResult {
        size_in: usize,
        size_out: usize,
        data: Vec<u8>,
    }
    unsafe extern "C++" {
        include!("wolf/src/compression/cxx/lzma/lzma.hpp");

        pub fn compress(pSourceBuffer: &[u8], pResult: &mut LZMAResult) -> bool;
        pub fn decompress(pSourceBuffer: &[u8], pResult: &mut LZMAResult) -> bool;
    }
}

#[test]
fn test() {
    use crate::compression::lzma;
    let mut result = lzma::ffi::LZMAResult {
        size_in: 0,
        size_out: 0,
        data: Vec::new(),
    };
    //l.compress(&mut result);

    println!("{:?}", result);

    println!("lzma version");
}
