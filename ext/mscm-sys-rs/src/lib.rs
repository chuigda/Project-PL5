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
    *mut MSCMRuntime,
    *mut MSCMScope,
    *mut c_void,
    usize,
    *mut MSCMValue
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
        dtor: Option<MSCMUserDtor>,
        marker: Option<MSCMUserMarker>
    ) -> MSCMValue;
    pub fn mscm_make_native_function(
        fnptr: MSCMNativeFnPtr,
        ctx: *mut c_void,
        ctx_dtor: Option<MSCMUserDtor>,
        ctx_marker: Option<MSCMUserMarker>
    ) -> MSCMValue;
    pub fn mscm_free_value(value: MSCMValue);
    pub fn mscm_free_value_deep(value: MSCMValue);
    pub fn mscm_value_is_true(value: MSCMValue) -> u8;
}

// scope.h

pub type MSCMScope = c_void;

extern "C" {
    pub fn mscm_scope_new(parent: *mut MSCMScope) -> *mut MSCMScope;
    pub fn mscm_scope_push(scope: *mut MSCMScope,
                           name: *const c_char,
                           value: MSCMValue);
    pub fn mscm_scope_set(scope: *mut MSCMScope,
                          name: *const c_char,
                          value: MSCMValue,
                          ok: *mut u8);
    pub fn mscm_get(scope: *mut MSCMScope,
                    name: *const c_char,
                    ok: *mut u8) -> MSCMValue;
    pub fn mscm_get_current(scope: *mut MSCMScope,
                            name: *const c_char,
                            ok: *mut u8) -> MSCMValue;
}

// rt.h

pub type MSCMRuntime = c_void;

extern "C" {
    pub fn mscm_runtime_push(rt: *mut MSCMRuntime,
                             name: *const c_char,
                             value: MSCMValue);
    pub fn mscm_runtime_get(rt: *mut MSCMRuntime,
                            name: *const c_char,
                            ok: *mut u8) -> MSCMValue;
    pub fn mscm_runtime_trace_exit(rt: *mut MSCMRuntime) -> !;
    pub fn mscm_toggle_gc(rt: *mut MSCMRuntime, enable: bool);
    pub fn mscm_runtime_gc_add(rt: *mut MSCMRuntime, value: MSCMValue);
    pub fn mscm_gc_mark(value: MSCMValue);
    pub fn mscm_gc_mark_scope(scope: *mut MSCMScope);
}
