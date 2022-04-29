#[cxx::bridge]
pub mod ffi {
    unsafe extern "C++" {
        include!("src/system/memory/cxx/WString.hpp");

        type WString;
        pub fn find_str(&self, p_val: &str) -> usize;
        pub fn find_first_of_str(&self, p_val: &str, p_pos: usize) -> usize;
        pub fn get_length(&self) -> usize;
        pub fn get_size(&self) -> usize;
        pub fn get_max_size(&self) -> usize;
        pub fn get_c_str(&self) -> &str;

        pub fn New() -> UniquePtr<WString>;
        pub fn From(p_val: &str) -> UniquePtr<WString>;
        pub fn npos() -> usize;
    }
}

impl std::fmt::Display for ffi::WString {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(f, "{}", self.get_c_str())
    }
}

#[test]
fn test() {
    //let str = ffi::From("This is a test string for Wstring from Wolf Engine");
    // assert!(str.get_size() == 50);
    // assert!(str.get_length() == 50);
    // assert!(str.find_str("str") == 15);

    let t0 = Instant::now();
    let str = "This is a test string for Wstring from and
        This is a test string for Wstring from &
        This is a test string for Wstring from &
        This is a test string for Wstring from !
        This is a test string for Wstring from *  
        This is a test string for Wstring from $  
        Wolf Engine 
        This is a test string for Wstring from *  
        This is a test string for Wstring from ˆ  
        This is a test string for Wstring from %  
        This is a test string for Wstring from @
        wolf ENGINE";
    //let wstr = ffi::From(str);
    let find = str.find("Wolf").unwrap();
    //let find = wstr.find_str("Wolf");
    let duration = Instant::now() - t0;
    println!("{} in time: {}", find, duration.as_nanos());

    //300 in time: 40219
    //300 in time: 28755
}
