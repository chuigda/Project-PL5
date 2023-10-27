use std::{mem::size_of, ptr::null_mut, ffi::c_void, sync::{OnceLock, Mutex}, collections::HashMap, any::TypeId};
use mscm_sys::{
    MSCMSlice as RawSlice,
    MSCMValue as RawValue,
    MSCMType as RawType,
    MSCMInt as RawIntValue,
    MSCMFloat as RawFloatValue,
    MSCMStringHead as RawStringValueHead,
    MSCMPair as RawPair,
    MSCMFunction as RawFunction,
    MSCMHandle as RawHandle,
    MSCMNativeFunction as RawNativeFunction,

    MSCMRuntime as RawRuntime, mscm_gc_toggle
};

#[derive(Debug, Clone)]
pub enum Value {
    Int(i64),
    Float(f64),
    String(String),
    Symbol(String),
    Pair(Pair),
    Function(Function),
    Handle(Handle),
    NativeFunction(NativeFunction)
}

impl Value {
    pub unsafe fn from_raw(raw: RawValue) -> Self {
        match (*raw).ty {
            RawType::INT => {
                let raw_int = raw as *mut RawIntValue;
                Value::Int((*raw_int).value)
            },
            RawType::FLOAT => {
                let raw_float = raw as *mut RawFloatValue;
                Value::Float((*raw_float).value)
            },
            RawType::STRING | RawType::SYMBOL => {
                let raw_string = raw as *mut RawStringValueHead;
                let len = (*raw_string).len;
                let raw_bytes = (raw_string as *mut u8).add(size_of::<RawStringValueHead>());
                let bytes = std::slice::from_raw_parts(raw_bytes, len);
                let string = String::from_utf8_lossy(bytes);
                if (*raw).ty == RawType::STRING {
                    Value::String(string.into_owned())
                } else {
                    Value::Symbol(string.into_owned())
                }
            },
            RawType::PAIR => {
                let raw_pair = raw as *mut RawPair;
                Value::Pair(Pair(raw_pair))
            },
            RawType::FUNCTION => {
                let raw_function = raw as *mut RawFunction;
                Value::Function(Function(raw_function))
            },
            RawType::HANDLE => {
                let raw_handle = raw as *mut RawHandle;
                Value::Handle(Handle(raw_handle))
            },
            RawType::NATIVE => {
                let raw_native = raw as *mut RawNativeFunction;
                Value::NativeFunction(NativeFunction(raw_native))
            },
            _ => panic!("unknown type id: {}", (*raw).ty)
        }
    }

    pub fn into_raw(&self, rt: &Runtime) -> RawValue {
        unsafe {
            match self {
                Value::Int(ivalue) => {
                    let r = mscm_sys::mscm_make_int(*ivalue);
                    mscm_sys::mscm_gc_add(rt.0, r);
                    r
                },
                Value::Float(fvalue) => {
                    let r = mscm_sys::mscm_make_float(*fvalue);
                    mscm_sys::mscm_gc_add(rt.0, r);
                    r
                },
                Value::String(svalue) => {
                    let cstr = std::ffi::CString::new(svalue.as_str()).unwrap();
                    let slice = RawSlice {
                        ptr: cstr.as_ptr(),
                        len: cstr.as_bytes().len()
                    };
                    let r = mscm_sys::mscm_make_string(slice, false, null_mut());
                    mscm_sys::mscm_gc_add(rt.0, r);
                    r
                },
                Value::Symbol(svalue) => {
                    let cstr = std::ffi::CString::new(svalue.as_str()).unwrap();
                    let slice = RawSlice {
                        ptr: cstr.as_ptr(),
                        len: cstr.as_bytes().len()
                    };
                    let r = mscm_sys::mscm_make_string(slice, false, null_mut());
                    (*r).ty = RawType::SYMBOL;
                    mscm_sys::mscm_gc_add(rt.0, r);
                    r
                },
                Value::Pair(pair) => pair.0 as RawValue,
                Value::Function(function) => function.0 as RawValue,
                Value::Handle(handle) => handle.0 as RawValue,
                Value::NativeFunction(native) => native.0 as RawValue
            }
        }
    }
}

#[derive(Debug, Clone, Copy)]
pub struct Pair(*mut RawPair);

impl Pair {
    pub fn make_pair(rt: &Runtime, fst: Value, snd: Value) -> Self {
        let raw_fst = fst.into_raw(rt);
        let raw_snd = snd.into_raw(rt);
        unsafe {
            let raw_pair = mscm_sys::mscm_make_pair(raw_fst, raw_snd);
            mscm_sys::mscm_gc_add(rt.0, raw_pair);
            Pair(raw_pair as *mut RawPair)
        }
    }
}

#[derive(Debug, Clone, Copy)]
pub struct Function(*mut RawFunction);

pub static USER_TID_POOL: OnceLock<Mutex<HashMap<TypeId, u32>>> = OnceLock::new();

fn get_user_tid<T: 'static>(rt: &Runtime) -> u32 {
    let cache = USER_TID_POOL
        .get_or_init(|| Mutex::new(HashMap::new()));
    let mut cache = cache.lock().unwrap();

    let tid = TypeId::of::<T>();
    if let Some(user_tid) = cache.get(&tid) {
        return *user_tid;
    } else {
        unsafe {
            let user_tid = mscm_sys::mscm_runtime_alloc_type_id(rt.0);
            cache.insert(tid, user_tid);
            user_tid
        }
    }
}

#[derive(Debug, Clone, Copy)]
pub struct Handle(*mut RawHandle);

impl Handle {
    pub fn make_handle<T: 'static + Drop>(rt: &Runtime, t: T) -> Self {
        let user_tid = get_user_tid::<T>(rt);
        let ptr = Box::into_raw(Box::new(t)) as *mut c_void;
        let dtor = drop_ptr::<T>;

        unsafe {
            let r = mscm_sys::mscm_make_handle(user_tid, ptr, Some(dtor), None);
            mscm_sys::mscm_gc_add(rt.0, r);
            Handle(r as *mut RawHandle)
        }
    }

    pub fn make_handle_nodrop<T: 'static + Copy>(rt: &Runtime, t: T) -> Self {
        let user_tid = get_user_tid::<T>(rt);
        let ptr = Box::into_raw(Box::new(t)) as *mut c_void;
        
        unsafe {
            let r = mscm_sys::mscm_make_handle(user_tid, ptr, None, None);
            mscm_sys::mscm_gc_add(rt.0, r);
            Handle(r as *mut RawHandle)
        }
    }

    pub fn get<'a, T: 'static>(&'a self, rt: &Runtime) -> &'a T {
        unsafe {
            assert_eq!((*self.0).user_tid, get_user_tid::<T>(rt));
            let ptr = (*self.0).ptr as *mut T;
            &*ptr
        }
    }

    pub fn get_mut<'a, T: 'static>(&'a mut self, rt: &Runtime) -> &'a mut T {
        unsafe {
            assert_eq!((*self.0).user_tid, get_user_tid::<T>(rt));
            let ptr = (*self.0).ptr as *mut T;
            &mut *ptr
        }
    }
}

extern "C" fn drop_ptr<T>(data: *mut c_void) {
    unsafe {
        let ptr = data as *mut T;
        let _ = Box::from_raw(ptr);
    }
}

#[derive(Debug, Clone, Copy)]
pub struct NativeFunction(*mut RawNativeFunction);

pub struct Runtime(*mut RawRuntime);

impl Runtime {
    pub unsafe fn from_raw(raw: *mut RawRuntime) -> Self {
        mscm_gc_toggle(raw, false);
        Runtime(raw)
    }
}

impl Drop for Runtime {
    fn drop(&mut self) {
        unsafe {
            mscm_gc_toggle(self.0, true);
        }
    }
}
