pub const MAX_BUFFER_SIZE: usize = 1024; //1K

#[repr(C)]
#[derive(Debug)]
pub enum BufferType {
    BINARY = 0,
    TEXT,
}

#[repr(C)]
pub struct Buffer {
    pub type_: BufferType,
    pub size: usize,
    pub buf: [u8; MAX_BUFFER_SIZE],
}

impl Buffer {
    #[must_use]
    pub const fn new(p_type: BufferType) -> Self {
        Self {
            type_: p_type,
            size: 0,
            buf: [0; MAX_BUFFER_SIZE],
        }
    }
    pub fn reset(&mut self) {
        self.size = 0;
        self.buf.fill(0u8);
    }
}
