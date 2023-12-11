use std::collections::HashMap;
use std::ffi::{c_char, CStr};
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
    MSCMString
};

static STRUCT_TID: OnceLock<u32> = OnceLock::new();

#[no_mangle]
pub unsafe extern "C" fn mscm_load_ext(rt: *mut MSCMRuntime) {
    let struct_tid = mscm_runtime_alloc_type_id(rt);
    STRUCT_TID.set(struct_tid).unwrap();

    let make_struct_v = mscm_make_native_function(
        b"struct\0".as_ptr() as _,
        make_struct,
        null_mut() as _,
        None,
        None
    );
    mscm_runtime_push(rt, b"struct\0".as_ptr() as _, make_struct_v);
    mscm_gc_add(rt, make_struct_v);

    let get_field_v = mscm_make_native_function(
        b"get-field\0".as_ptr() as _,
        get_field,
        null_mut() as _,
        None,
        None
    );
    mscm_runtime_push(rt, b"get-field\0".as_ptr() as _, get_field_v);
    mscm_gc_add(rt, get_field_v);

    let set_field_v = mscm_make_native_function(
        b"set-field!\0".as_ptr() as _,
        set_field,
        null_mut() as _,
        None,
        None
    );
    mscm_runtime_push(rt, b"set-field!\0".as_ptr() as _, set_field_v);
    mscm_gc_add(rt, set_field_v);

    let struct_dup_v = mscm_make_native_function(
        b"struct-dup\0".as_ptr() as _,
        struct_dup,
        null_mut() as _,
        None,
        None
    );
    mscm_runtime_push(rt, b"struct-dup\0".as_ptr() as _, struct_dup_v);
    mscm_gc_add(rt, struct_dup_v);
}

extern "C" fn make_struct(
    rt: *mut MSCMRuntime,
    _scope: *mut MSCMScope,
    _ctx: *mut c_void,
    narg: usize,
    args: *mut MSCMValue
) -> MSCMValue {
    unsafe {
        if narg % 2 != 0 {
            eprintln!("struct: expected even number of arguments");
            mscm_runtime_trace_exit(rt);
        }

        let mut hash_map = Box::new(HashMap::new());
        for i in (0..narg).step_by(2) {
            let key = args.add(i).read();
            let value = args.add(i + 1).read();

            if (*key).ty != MSCMType::STRING && (*key).ty != MSCMType::SYMBOL {
                eprintln!("struct: {}th field: expected string or symbol as key", i + 1);
                drop(hash_map);
                mscm_runtime_trace_exit(rt);
            }

            let key = key as *mut MSCMString;
            let key_data_ptr = &(*key).data as *const c_char;
            let key = CStr::from_ptr(key_data_ptr).to_string_lossy().to_string();

            hash_map.insert(key, value);
        }

        let ptr = Box::into_raw(hash_map) as *mut c_void;
        let ret = mscm_make_handle(
            *STRUCT_TID.get().unwrap(),
            ptr,
            Some(dealloc_struct),
            Some(mark_struct)
        );
        mscm_gc_add(rt, ret);
        ret
    }
}

extern "C" fn get_field(
    rt: *mut MSCMRuntime,
    _scope: *mut MSCMScope,
    _ctx: *mut c_void,
    narg: usize,
    args: *mut MSCMValue
) -> MSCMValue {
    unsafe {
        if narg != 2 {
            eprintln!("get-field: expected 2 arguments");
            mscm_runtime_trace_exit(rt);
        }

        let struct_v = args.add(0).read();
        let field_v = args.add(1).read();

        if (*struct_v).ty != MSCMType::HANDLE {
            eprintln!("get-field: expected struct as first argument");
            mscm_runtime_trace_exit(rt);
        }

        if (*field_v).ty != MSCMType::STRING && (*field_v).ty != MSCMType::SYMBOL {
            eprintln!("get-field: expected string or symbol as field");
            mscm_runtime_trace_exit(rt);
        }

        let struct_v = struct_v as *mut MSCMHandle;
        if (*struct_v).user_tid != *STRUCT_TID.get().unwrap() {
            eprintln!("get-field: expected struct as first argument");
            mscm_runtime_trace_exit(rt);
        }

        let struct_ptr = (*struct_v).ptr as *mut HashMap<String, MSCMValue>;
        let struct_ptr = &mut *struct_ptr;

        let field_v = field_v as *mut MSCMString;
        let field_data_ptr = &(*field_v).data as *const c_char;
        let field = CStr::from_ptr(field_data_ptr).to_string_lossy().to_string();

        let ret = match struct_ptr.get(&field) {
            Some(value) => *value,
            None => {
                eprintln!("get-field: field `{}` not found", field);
                drop(field);
                mscm_runtime_trace_exit(rt)
            }
        };
        ret
    }
}

extern "C" fn set_field(
    rt: *mut MSCMRuntime,
    _scope: *mut MSCMScope,
    _ctx: *mut c_void,
    narg: usize,
    args: *mut MSCMValue
) -> MSCMValue {
    unsafe {
        if narg != 3 {
            eprintln!("set-field: expected 3 arguments");
            mscm_runtime_trace_exit(rt);
        }

        let struct_v = args.add(0).read();
        let field_v = args.add(1).read();
        let value_v = args.add(2).read();

        if (*struct_v).ty != MSCMType::HANDLE {
            eprintln!("set-field: expected struct as first argument");
            mscm_runtime_trace_exit(rt);
        }

        if (*field_v).ty != MSCMType::STRING && (*field_v).ty != MSCMType::SYMBOL {
            eprintln!("set-field: expected string or symbol as field");
            mscm_runtime_trace_exit(rt);
        }

        let struct_v = struct_v as *mut MSCMHandle;
        if (*struct_v).user_tid != *STRUCT_TID.get().unwrap() {
            eprintln!("set-field: expected struct as first argument");
            mscm_runtime_trace_exit(rt);
        }

        let struct_ptr = (*struct_v).ptr as *mut HashMap<String, MSCMValue>;
        let struct_ptr = &mut *struct_ptr;

        let field_v = field_v as *mut MSCMString;
        let field_data_ptr = &(*field_v).data as *const c_char;
        let field = CStr::from_ptr(field_data_ptr).to_string_lossy().to_string();

        struct_ptr.insert(field, value_v);
        null_mut()
    }
}

extern "C" fn struct_dup(
    rt: *mut MSCMRuntime,
    _scope: *mut MSCMScope,
    _ctx: *mut c_void,
    narg: usize,
    args: *mut MSCMValue
) -> MSCMValue {
    unsafe {
        if narg != 1 {
            eprintln!("struct-dup: expected 2 arguments");
            mscm_runtime_trace_exit(rt);
        }

        let struct_v = args.add(0).read();

        if (*struct_v).ty != MSCMType::HANDLE {
            eprintln!("struct-dup: expected a struct");
            mscm_runtime_trace_exit(rt);
        }

        let struct_v = struct_v as *mut MSCMHandle;
        if (*struct_v).user_tid != *STRUCT_TID.get().unwrap() {
            eprintln!("struct-dup: expected a struct");
            mscm_runtime_trace_exit(rt);
        }

        let struct_ptr = (*struct_v).ptr as *mut HashMap<String, MSCMValue>;
        let struct_ptr = &mut *struct_ptr;

        let hash_map = Box::new(struct_ptr.clone());
        let ptr = Box::into_raw(hash_map) as *mut c_void;
        let ret = mscm_make_handle(
            *STRUCT_TID.get().unwrap(),
            ptr,
            Some(dealloc_struct),
            Some(mark_struct)
        );
        mscm_gc_add(rt, ret);
        ret
    }
}

extern "C" fn dealloc_struct(ptr: *mut c_void) {
    unsafe {
        let hash_map = Box::from_raw(ptr as *mut HashMap<String, MSCMValue>);
        drop(hash_map);
    }
}

extern "C" fn mark_struct(rt: *mut MSCMRuntime, ptr: *mut c_void) {
    unsafe {
        let hash_map = &*(ptr as *const HashMap<String, MSCMValue>);
        for (_key, value) in hash_map {
            mscm_gc_mark(rt, *value);
        }
    }
}
