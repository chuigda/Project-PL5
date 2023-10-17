use std::ffi::c_void;

pub type MSCMValue = *mut c_void;

extern "C" {
    pub fn mscm_make_int(value: i64) -> MSCMValue;
}