use std::sync::OnceLock;
use std::{ffi::c_void, ptr::null_mut};
use mscm_sys::{
    MSCMRuntime,
    MSCMScope,
    MSCMValue,
    MSCMType,
    MSCMHandle,
    mscm_make_native_function,
    mscm_runtime_push,
    mscm_gc_add,
    mscm_make_handle,
    mscm_runtime_alloc_type_id,
    mscm_gc_mark,
    mscm_runtime_trace_exit,
    mscm_make_int,
    value_type_string, MSCMInt
};

static VECTOR_TID: OnceLock<u32> = OnceLock::new();

#[no_mangle]
pub unsafe extern "C" fn mscm_load_ext(rt: *mut MSCMRuntime) {
    let vector_tid = mscm_runtime_alloc_type_id(rt);
    VECTOR_TID.set(vector_tid).unwrap();

    let make_vector_v = mscm_make_native_function(
        b"vector\0".as_ptr() as _,
        make_vector,
        null_mut() as _,
        None,
        None
    );
    mscm_runtime_push(rt, b"vector\0".as_ptr() as _, make_vector_v);
    mscm_gc_add(rt, make_vector_v);

    let vector_length_v = mscm_make_native_function(
        b"vector-length\0".as_ptr() as _,
        vector_length,
        null_mut() as _,
        None,
        None
    );
    mscm_runtime_push(rt, b"vector-length\0".as_ptr() as _, vector_length_v);
    mscm_gc_add(rt, vector_length_v);

    let vector_push_v = mscm_make_native_function(
        b"vector-push!\0".as_ptr() as _,
        vector_push,
        null_mut() as _,
        None,
        None
    );
    mscm_runtime_push(rt, b"vector-push!\0".as_ptr() as _, vector_push_v);
    mscm_gc_add(rt, vector_push_v);

    let vector_ref_v = mscm_make_native_function(
        b"vector-ref\0".as_ptr() as _,
        vector_ref,
        null_mut() as _,
        None,
        None
    );
    mscm_runtime_push(rt, b"vector-ref\0".as_ptr() as _, vector_ref_v);
    mscm_gc_add(rt, vector_ref_v);

    let vector_set_v = mscm_make_native_function(
        b"vector-set!\0".as_ptr() as _,
        vector_set,
        null_mut() as _,
        None,
        None
    );
    mscm_runtime_push(rt, b"vector-set!\0".as_ptr() as _, vector_set_v);
    mscm_gc_add(rt, vector_set_v);
}

extern "C" fn make_vector(
    rt: *mut MSCMRuntime,
    _scope: *mut MSCMScope,
    _ctx: *mut c_void,
    narg: usize,
    args: *mut MSCMValue
) -> MSCMValue {
    unsafe {
        let mut vec = Box::new(Vec::new());
        for i in 0..narg {
            vec.push(*args.offset(i as isize));
        }

        let ptr = Box::into_raw(vec) as *mut c_void;
        let ret = mscm_make_handle(
            *VECTOR_TID.get().unwrap(),
            ptr,
            Some(dealloc_vec),
            Some(mark_vec)
        );
        mscm_gc_add(rt, ret);
        ret
    }
}

extern "C" fn vector_length(
    rt: *mut MSCMRuntime,
    _scope: *mut MSCMScope,
    _ctx: *mut c_void,
    narg: usize,
    args: *mut MSCMValue
) -> MSCMValue {
    unsafe {
        if narg != 1 {
            eprintln!("vector-length: expected 1 argument, got {}", narg);
            mscm_runtime_trace_exit(rt)
        }

        let arg0 = *args.offset(0);
        if (*arg0).ty != MSCMType::HANDLE {
            eprintln!("vector-length: expected handle, got {}", (*arg0).ty);
            mscm_runtime_trace_exit(rt)
        }

        let handle = arg0 as *mut MSCMHandle;
        if (*handle).user_tid != *VECTOR_TID.get().unwrap() {
            eprintln!("vector-length: expected vector, got {}", (*handle).user_tid);
            mscm_runtime_trace_exit(rt)
        }

        let vec = &*((*handle).ptr as *mut Vec<MSCMValue>);
        let ret = mscm_make_int(vec.len() as i64);
        mscm_gc_add(rt, ret);
        ret
    }
}

extern "C" fn vector_push(
    rt: *mut MSCMRuntime,
    _scope: *mut MSCMScope,
    _ctx: *mut c_void,
    narg: usize,
    args: *mut MSCMValue
) -> MSCMValue {
    let vector_tid = *VECTOR_TID.get().unwrap();

    unsafe {
        if narg != 2 {
            eprintln!("vector-push!: expected 2 arguments, got {}", narg);
            mscm_runtime_trace_exit(rt)
        }

        let arg0 = *args.offset(0);
        if (*arg0).ty != MSCMType::HANDLE {
            eprintln!("vector-push!: expected handle, got {}", value_type_string(arg0));
            mscm_runtime_trace_exit(rt)
        }

        let handle = arg0 as *mut MSCMHandle;
        if (*handle).user_tid != vector_tid {
            eprintln!("vector-push!: expected vector tid = {}, got tid={}",
                      vector_tid,
                      (*handle).user_tid);
            mscm_runtime_trace_exit(rt)
        }

        let vec = &mut *((*handle).ptr as *mut Vec<MSCMValue>);
        vec.push(*args.offset(1));
        null_mut()
    }
}

extern "C" fn vector_ref(
    rt: *mut MSCMRuntime,
    _scope: *mut MSCMScope,
    _ctx: *mut c_void,
    narg: usize,
    args: *mut MSCMValue
) -> MSCMValue {
    let vector_tid = *VECTOR_TID.get().unwrap();

    unsafe {
        if narg != 2 {
            eprintln!("vector-at: expected 2 arguments, got {}", narg);
            mscm_runtime_trace_exit(rt)
        }

        let arg0 = *args.offset(0);
        if (*arg0).ty != MSCMType::HANDLE {
            eprintln!("vector-at: expected handle, got {}", value_type_string(arg0));
            mscm_runtime_trace_exit(rt)
        }

        let handle = arg0 as *mut MSCMHandle;
        if (*handle).user_tid != vector_tid {
            eprintln!("vector-at: expected vector tid = {}, got tid={}",
                      vector_tid,
                      (*handle).user_tid);
            mscm_runtime_trace_exit(rt)
        }

        let arg1 = *args.offset(1);
        if (*arg1).ty != MSCMType::INT {
            eprintln!("vector-at: expected int, got {}", value_type_string(arg1));
            mscm_runtime_trace_exit(rt)
        }

        let idx = arg1 as *mut MSCMInt;
        let idx = (*idx).value;

        let vec: &Vec<MSCMValue> = &*((*handle).ptr as *mut Vec<MSCMValue> as *const _);
        if idx < 0 || idx >= vec.len() as i64 {
            eprintln!("vector-at: index {} out of bounds, vector length {}", idx, vec.len() - 1);
            mscm_runtime_trace_exit(rt)
        }

        *vec.get_unchecked(idx as usize)
    }
}

extern "C" fn vector_set(
    rt: *mut MSCMRuntime,
    _scope: *mut MSCMScope,
    _ctx: *mut c_void,
    narg: usize,
    args: *mut MSCMValue
) -> MSCMValue {
    let vector_tid = *VECTOR_TID.get().unwrap();

    unsafe {
        if narg != 3 {
            eprintln!("vector-set!: expected 3 arguments, got {}", narg);
            mscm_runtime_trace_exit(rt)
        }

        let arg0 = *args.offset(0);
        if (*arg0).ty != MSCMType::HANDLE {
            eprintln!("vector-set!: expected handle, got {}", value_type_string(arg0));
            mscm_runtime_trace_exit(rt)
        }

        let handle = arg0 as *mut MSCMHandle;
        if (*handle).user_tid != vector_tid {
            eprintln!("vector-set!: expected vector tid = {}, got tid={}",
                      vector_tid,
                      (*handle).user_tid);
            mscm_runtime_trace_exit(rt)
        }

        let arg1 = *args.offset(1);
        if (*arg1).ty != MSCMType::INT {
            eprintln!("vector-set!: expected int, got {}", value_type_string(arg1));
            mscm_runtime_trace_exit(rt)
        }

        let idx = arg1 as *mut MSCMInt;
        let idx = (*idx).value;
        let vec: &mut Vec<MSCMValue> = &mut *((*handle).ptr as *mut Vec<MSCMValue>);
        if idx < 0 || idx >= vec.len() as i64 {
            eprintln!("vector-set!: index {} out of bounds, vector length {}", idx, vec.len() - 1);
            mscm_runtime_trace_exit(rt)
        }

        let arg2 = *args.offset(2);

        *vec.get_unchecked_mut(idx as usize) = arg2;
        null_mut()
    }
}

extern "C" fn dealloc_vec(ptr: *mut c_void) {
    unsafe {
        let _vec = Box::from_raw(ptr as *mut Vec<MSCMValue>);
    }
}

extern "C" fn mark_vec(rt: *mut MSCMRuntime, ptr: *mut c_void) {
    unsafe {
        let vec = ptr as *mut Vec<MSCMValue>;
        for v in (*vec).iter() {
            mscm_gc_mark(rt, *v);
        }
    }
}
