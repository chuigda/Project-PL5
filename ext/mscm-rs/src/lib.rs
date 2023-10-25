use mscm_sys::{
    MSCMValueBase as RawValueBase,
    MSCMType as RawType,
    MSCMPair as RawPair,
    MSCMHandle as RawHandle,
    MSCMFunction as RawFunction,

};

#[derive(Debug, Clone, Copy)]
pub enum Value {
    Int(i64),
    Float(f64),
    String(String),
    Symbol(String),
    Pair(Pair),
    Handle(Handle),
    Unknown(*mut MSCMValueBase)
}

#[derive(Debug, Clone, Copy)]
pub struct Pair(*mut RawPair);

#[derive(Debug, Clone, Copy)]
pub struct Handle(*mut RawHandle);

#[derive(Debug, Clone, Copy)]
pub struct Function(*mut RawFunction);

impl Value {
    pub unsafe fn from_raw(raw: *mut RawValueBase) -> Self {
        todo!()
    }
}
