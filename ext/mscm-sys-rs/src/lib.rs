use std::ffi::{c_char, c_void, CStr};

// slice.h

#[repr(C)]
#[derive(Copy, Clone)]
pub struct MSCMSlice {
    pub ptr: *const c_char,
    pub len: usize
}

// value.h

#[allow(non_snake_case)]
pub mod MSCMType {
    pub const INT: u8 = 0;
    pub const FLOAT: u8 = 1;
    pub const STRING: u8 = 2;
    pub const SYMBOL: u8 = 3;
    pub const PAIR: u8 = 4;
    pub const FUNCTION: u8 = 5;
    pub const HANDLE: u8 = 6;
    pub const NATIVE: u8 = 7;
}

pub unsafe fn type_string(t: u8) -> String {
    CStr::from_ptr(mscm_type_name(t))
        .to_string_lossy()
        .into_owned()
}

pub unsafe fn value_type_string(value: MSCMValue) -> String {
    CStr::from_ptr(mscm_value_type_name(value))
        .to_string_lossy()
        .into_owned()
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct MSCMValueBase {
    pub ty: u8,
    pub gc_mark: u8
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct MSCMInt {
    pub base: MSCMValueBase,
    pub value: i64
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct MSCMFloat {
    pub base: MSCMValueBase,
    pub value: f64
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct MSCMStringHead {
    pub base: MSCMValueBase,
    pub len: usize,
    //
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct MSCMPair {
    pub base: MSCMValueBase,
    pub fst: MSCMValue,
    pub snd: MSCMValue
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct MSCMHandle {
    pub base: MSCMValueBase,
    pub user_tid: u32,
    pub ptr: *mut c_void,
    pub dtor: Option<MSCMUserDtor>,
    pub marker: Option<MSCMUserMarker>
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct MSCMFunction {
    pub base: MSCMValueBase,
    pub fndef: *mut c_void,
    pub scope: *mut MSCMScope
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

#[repr(C)]
pub struct MSCMNativeFunction {
    pub base: MSCMValueBase,
    pub name: *const c_char,
    pub fnptr: MSCMNativeFnPtr,
    pub ctx: *mut c_void,
    pub ctx_dtor: Option<MSCMUserDtor>,
    pub ctx_marker: Option<MSCMUserMarker>
}

extern "C" {
    pub fn mscm_type_name(t: u8) -> *const c_char;
    pub fn mscm_value_type_name(value: MSCMValue) -> *const c_char;

    pub fn mscm_make_int(value: i64) -> MSCMValue;
    pub fn mscm_make_float(value: f64) -> MSCMValue;
    pub fn mscm_make_string(value: MSCMSlice, escape: bool, quote: bool) -> MSCMValue;
    pub fn mscm_alloc_string(size: usize) -> MSCMValue;
    pub fn mscm_make_pair(fst: MSCMValue, snd: MSCMValue) -> MSCMValue;
    pub fn mscm_make_handle(
        user_tid: u32,
        ptr: *mut c_void,
        dtor: Option<MSCMUserDtor>,
        marker: Option<MSCMUserMarker>
    ) -> MSCMValue;
    pub fn mscm_make_native_function(
        name: *const c_char,
        fnptr: MSCMNativeFnPtr,
        ctx: *mut c_void,
        ctx_dtor: Option<MSCMUserDtor>,
        ctx_marker: Option<MSCMUserMarker>
    ) -> MSCMValue;
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

    pub fn mscm_gc_toggle(rt: *mut MSCMRuntime, enable: bool);
    pub fn mscm_gc_add(rt: *mut MSCMRuntime, value: MSCMValue);
    pub fn mscm_gc_mark(value: MSCMValue);
    pub fn mscm_gc_mark_scope(scope: *mut MSCMScope);

    pub fn mscm_runtime_alloc_type_id(rt: *mut MSCMRuntime) -> u32;
}
