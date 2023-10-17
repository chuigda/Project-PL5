use std::ffi::{c_char, c_void};

// slice.h
#[repr(C)]
#[derive(Copy, Clone)]
pub struct MSCMSlice {
    pub ptr: *const c_char,
    pub len: usize
}

// value.h

#[repr(C)]
#[derive(Copy, Clone)]
pub struct MSCMValueBase {
    ty: u8,
    gc_mark: u8
}

pub type MSCMValue = *mut MSCMValueBase;
pub type MSCMUserDtor = extern "C" fn(*mut c_void);
pub type MSCMUserMarker = extern "C" fn(*mut c_void);
pub type MSCMNativeFnPtr = extern "C" fn(
    *mut c_void, /* mscm_runtime *rt */
    *mut c_void, /* mscm_scope *scope */
    *mut c_void, /* void *ctx */
    usize, /* size_t narg */
    *mut MSCMValue /* mscm_value *args */
) -> MSCMValue;

extern "C" {
    pub fn mscm_make_int(value: i64) -> MSCMValue;
    pub fn mscm_make_float(value: f64) -> MSCMValue;
    pub fn mscm_make_string(value: MSCMSlice, escape: bool, quote: bool) -> MSCMValue;
    // Rust cannot access C VLA without many efforts, so not including this API
    // pub unsafe fn mscm_alloc_string
    pub fn mscm_make_pair(fst: MSCMValue, snd: MSCMValue) -> MSCMValue;
    pub fn mscm_make_handle(
        ptr: *mut c_void,
        dtor: MSCMUserDtor,
        marker: MSCMUserMarker
    ) -> MSCMValue;
    pub fn mscm_make_native_function(
        fnptr: MSCMNativeFnPtr,
        ctx: *mut c_void,
        ctx_dtor: MSCMUserDtor,
        ctx_marker: MSCMUserMarker
    ) -> MSCMValue;
    pub fn mscm_free_value(value: MSCMValue);
    pub fn mscm_free_value_deep(value: MSCMValue);
    pub fn mscm_value_is_true(value: MSCMValue) -> bool;
}
