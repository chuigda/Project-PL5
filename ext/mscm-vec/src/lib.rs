use std::{ffi::c_void, ptr::{null_mut}};

use mscm_sys::{MSCMRuntime, MSCMScope, MSCMValue, mscm_make_native_function, mscm_runtime_push, mscm_runtime_gc_add};

#[no_mangle]
pub unsafe extern "C" fn mscm_load_ext(rt: *mut MSCMRuntime) {
    let my_print_v= mscm_make_native_function(
        my_print,
        null_mut() as _,
        None,
        None
    );
    mscm_runtime_push(rt, b"my-print\0".as_ptr() as _, my_print_v);
    mscm_runtime_gc_add(rt, my_print_v);
}

extern "C" fn my_print(
    _rt: *mut MSCMRuntime,
    _scope: *mut MSCMScope,
    _ctx: *mut c_void,
    _narg: usize,
    _args: *mut MSCMValue
) -> MSCMValue {
    println!("Hello world from Rust!");
    null_mut()
}
