#[repr(C)]
pub struct XINPUT_GAMEPAD_SHARED {
    pub buttons: std::os::raw::c_ushort,
    pub left_trigger: std::os::raw::c_uchar,
    pub right_trigger: std::os::raw::c_uchar,
    pub thumb_lx: std::os::raw::c_short,
    pub thumb_ly: std::os::raw::c_short,
    pub thumb_rx: std::os::raw::c_short,
    pub thumb_ry: std::os::raw::c_short,
}

#[repr(C)]
pub struct XINPUT_STATE_SHARED {
    pub packet_number: std::os::raw::c_ulong,
    pub game_pad: XINPUT_GAMEPAD_SHARED,
}
