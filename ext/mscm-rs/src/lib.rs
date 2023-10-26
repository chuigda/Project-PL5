use std::mem::size_of;
use mscm_sys::{
    MSCMValueBase as RawValueBase,
    MSCMType as RawType,
    MSCMInt as RawIntValue,
    MSCMFloat as RawFloatValue,
    MSCMStringHead as RawStringValueHead,
    MSCMPair as RawPair,
    MSCMFunction as RawFunction,
    MSCMHandle as RawHandle,
    MSCMNativeFunction as RawNativeFunction
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
    pub unsafe fn from_raw(raw: *mut RawValueBase) -> Self {
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
}

#[derive(Debug, Clone, Copy)]
pub struct Pair(*mut RawPair);

#[derive(Debug, Clone, Copy)]
pub struct Function(*mut RawFunction);

#[derive(Debug, Clone, Copy)]
pub struct Handle(*mut RawHandle);

#[derive(Debug, Clone, Copy)]
pub struct NativeFunction(*mut RawNativeFunction);
