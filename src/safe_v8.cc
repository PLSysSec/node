#include "safe_v8.h"

namespace safeV8 {

#define DEFINE_TY_VAL(Type) \
  MVal<Local<Value>, Local<Type>> Type##Val(Isolate* isolate, Local<Value> v) { \
    if (v->Is##Type()) { \
      return MVal<Local<Value>, Local<Type>>(v.As<Type>()); \
    } else { \
      MaybeLocal<String> mErrMsg =  \
        v8::String::NewFromUtf8(isolate, "Invalid type", v8::String::NewStringType::kNormalString); \
      Local<Value> err = v8::Exception::TypeError(mErrMsg.ToLocalChecked()); \
      return MVal<Local<Value>, Local<Type>>(err); \
    } \
  }

#define TYPE_LIST(V)   \
  V(Array)             \
  V(ArrayBuffer)       \
  V(ArrayBufferView)   \
  V(Boolean)           \
  V(DataView)          \
  V(Date)              \
  V(External)          \
  V(Float32Array)      \
  V(Float64Array)      \
  V(Function)          \
  V(Int16Array)        \
  V(Int32)             \
  V(Int32Array)        \
  V(Int8Array)         \
  V(Map)               \
  V(Name)              \
  V(Number)            \
  V(Object)            \
  V(Proxy)             \
  V(RegExp)            \
  V(Set)               \
  V(SharedArrayBuffer) \
  V(String)            \
  V(StringObject)      \
  V(Symbol)            \
  V(TypedArray)        \
  V(Uint16Array)       \
  V(Uint32)            \
  V(Uint32Array)       \
  V(Uint8Array)        \
  V(Uint8ClampedArray)

  TYPE_LIST(DEFINE_TY_VAL);

#undef TYPE_LIST
#undef DEFINE_TY_VAL

#define DEFINE_CTY_VAL(CType, JSType) \
  MVal<Local<Value>, CType> CType##Val(Isolate* isolate, Local<Value> v) { \
    if (v->Is##JSType()) { \
      return MVal<Local<Value>, CType>(v->JSType##Value()); \
    } else { \
      MaybeLocal<String> mErrMsg =  \
        v8::String::NewFromUtf8(isolate, "Invalid type", v8::String::NewStringType::kNormalString); \
      Local<Value> err = v8::Exception::TypeError(mErrMsg.ToLocalChecked()); \
      return MVal<Local<Value>, CType>(err); \
    } \
  } \
  \
  MVal<Local<Value>, CType> coerceTo##CType(Isolate* isolate, Local<Value> v) { \
    Maybe<CType> mv = v->JSType##Value(isolate->GetCurrentContext()); \
    if (mv.IsJust()) { \
      return MVal<Local<Value>, CType>(mv.FromJust()); \
    } else { \
      MaybeLocal<String> mErrMsg =  \
        v8::String::NewFromUtf8(isolate, "Invalid type", v8::String::NewStringType::kNormalString); \
      Local<Value> err = v8::Exception::TypeError(mErrMsg.ToLocalChecked()); \
      return MVal<Local<Value>, CType>(err); \
    } \
  }

  DEFINE_CTY_VAL(bool, Boolean)
    DEFINE_CTY_VAL(double, Number)
    DEFINE_CTY_VAL(uint32_t, Uint32)
    DEFINE_CTY_VAL(int32_t, Int32)
#undef DEFINE_CTY_VAL

  MVal<Local<Value>, int64_t> int64_tVal(Isolate* isolate, Local<Value> v) {
    int64_t num;
    if (v->IsNumber() && v->IntegerValue(isolate->GetCurrentContext()).To(&num)) {
      return MVal<Local<Value>, int64_t>(num);
    }
    else {
      MaybeLocal<String> mErrMsg =
        v8::String::NewFromUtf8(isolate, "Invalid type", v8::String::NewStringType::kNormalString);
      Local<Value> err = v8::Exception::TypeError(mErrMsg.ToLocalChecked());
      return MVal<Local<Value>, int64_t>(err);
    }
  }

  MVal<Local<Value>, Local<String>> toString(Isolate* isolate, Local<Value> v) {
    MaybeLocal<String> ret = v->ToString(isolate->GetCurrentContext());
    if (!ret.IsEmpty()) {
      return MVal<Local<Value>, Local<String>>(ret.FromMaybe(Local<String>()));
    }
    else {
      MaybeLocal<String> mErrMsg =
        v8::String::NewFromUtf8(isolate, "Could not convert to string", v8::String::NewStringType::kNormalString);
      Local<Value> err = v8::Exception::TypeError(mErrMsg.ToLocalChecked());
      return MVal<Local<Value>, Local<String>>(err);
    }
  }

  #define DEFINE_TY_VAL(Type) \
  bool SafeV8ConvertVal(Isolate* isolate, Local<Value> v, Local<Type>& outVal, Local<Value>& err, bool& hasError) { \
    if (v->Is##Type()) { \
      outVal = v.As<Type>(); \
      hasError = false; \
      return true; \
    } else { \
      MaybeLocal<String> mErrMsg =  \
        v8::String::NewFromUtf8(isolate, "Invalid type", v8::String::NewStringType::kNormalString); \
      err = v8::Exception::TypeError(mErrMsg.ToLocalChecked()); \
      hasError = true; \
      return false; \
    } \
  }

#define TYPE_LIST(V)   \
  V(Array)             \
  V(ArrayBuffer)       \
  V(ArrayBufferView)   \
  V(Boolean)           \
  V(DataView)          \
  V(Date)              \
  V(External)          \
  V(Float32Array)      \
  V(Float64Array)      \
  V(Function)          \
  V(Int16Array)        \
  V(Int32)             \
  V(Int32Array)        \
  V(Int8Array)         \
  V(Map)               \
  V(Name)              \
  V(Number)            \
  V(Object)            \
  V(Proxy)             \
  V(RegExp)            \
  V(Set)               \
  V(SharedArrayBuffer) \
  V(String)            \
  V(StringObject)      \
  V(Symbol)            \
  V(TypedArray)        \
  V(Uint16Array)       \
  V(Uint32)            \
  V(Uint32Array)       \
  V(Uint8Array)        \
  V(Uint8ClampedArray)

TYPE_LIST(DEFINE_TY_VAL);

#undef TYPE_LIST
#undef DEFINE_TY_VAL

#define DEFINE_CTY_VAL(CType, JSType) \
  bool SafeV8ConvertVal(Isolate* isolate, Local<Value> v, CType& outVal, Local<Value>& err, bool& hasError) { \
    if (v->Is##JSType()) { \
      outVal = v->JSType##Value(); \
      hasError = false; \
      return true; \
    } else { \
      MaybeLocal<String> mErrMsg =  \
        v8::String::NewFromUtf8(isolate, "Invalid type", v8::String::NewStringType::kNormalString); \
      err = v8::Exception::TypeError(mErrMsg.ToLocalChecked()); \
      hasError = true; \
      return false; \
    } \
  } \
  bool SafeV8CoerceVal(Isolate* isolate, Local<Value> v, CType& outVal, Local<Value>& err, bool& hasError) { \
    Maybe<CType> mv = v->JSType##Value(isolate->GetCurrentContext()); \
    if (mv.IsJust()) { \
      outVal = mv.FromJust(); \
      hasError = false; \
      return true; \
    } else { \
      MaybeLocal<String> mErrMsg =  \
        v8::String::NewFromUtf8(isolate, "Invalid type", v8::String::NewStringType::kNormalString); \
      err = v8::Exception::TypeError(mErrMsg.ToLocalChecked()); \
      hasError = true; \
      return false; \
    } \
  }

DEFINE_CTY_VAL(bool, Boolean)
DEFINE_CTY_VAL(double, Number)
DEFINE_CTY_VAL(uint32_t, Uint32)
DEFINE_CTY_VAL(int32_t, Int32)
#undef DEFINE_CTY_VAL

//int64 version
  bool SafeV8ConvertVal(Isolate* isolate, Local<Value> v, int64_t& outVal, Local<Value>& err, bool& hasError) {
    if (v->IsNumber() && v->IntegerValue(isolate->GetCurrentContext()).To(&outVal)) {
      hasError = false;
      return true;
    }
    else {
      MaybeLocal<String> mErrMsg =
        v8::String::NewFromUtf8(isolate, "Invalid type", v8::String::NewStringType::kNormalString);
      err = v8::Exception::TypeError(mErrMsg.ToLocalChecked());
      hasError = true;
      return false;
    }
  }


  SafeV8Promise_Base Err(Local<Value> err)
  {
    SafeV8Promise_Base e;
    e.exceptionThrown = true;
    e.err = err;
    return e;
  }

  SafeV8Promise_Base Err(Isolate* isolate, char* err)
  {
    SafeV8Promise_Base e;
    e.exceptionThrown = true;
    MaybeLocal<String> mErrMsg = v8::String::NewFromUtf8(isolate, err, v8::String::NewStringType::kNormalString);
    e.err = v8::Exception::TypeError(mErrMsg.ToLocalChecked());
    return e;
  }

  SafeV8Promise_Base Err(Isolate* isolate, char* err, v8::Local<v8::Value>(*errorType)(v8::Local<v8::String>))
  {
    SafeV8Promise_Base e;
    e.exceptionThrown = true;
    MaybeLocal<String> mErrMsg = v8::String::NewFromUtf8(isolate, err, v8::String::NewStringType::kNormalString);
    e.err = errorType(mErrMsg.ToLocalChecked());
    return e;
  }

  v8::Local<v8::Value> V8Err(Isolate* isolate, char* err, v8::Local<v8::Value>(*errorType)(v8::Local<v8::String>))
  {
    MaybeLocal<String> mErrMsg = v8::String::NewFromUtf8(isolate, err, v8::String::NewStringType::kNormalString);
    auto ret = errorType(mErrMsg.ToLocalChecked());
    return ret;
  }

  V8_WARN_UNUSED_RESULT SafeV8Promise_GetOutput1 With(Isolate* isolate, Local<Value> first)
  {
    return SafeV8Promise_GetOutput1(isolate, first);
  }

  V8_WARN_UNUSED_RESULT SafeV8Promise_GetOutput2 With(Isolate* isolate, Local<Value> first, Local<Value> second)
  {
    return SafeV8Promise_GetOutput2(isolate, first, second);
  }

  V8_WARN_UNUSED_RESULT SafeV8Promise_GetOutput3 With(Isolate* isolate, Local<Value> first, Local<Value> second, Local<Value> third)
  {
    return SafeV8Promise_GetOutput3(isolate, first, second, third);
  }

  V8_WARN_UNUSED_RESULT SafeV8Promise_GetOutput_Coerce1 WithCoerce(Isolate* isolate, Local<Value> first)
  {
    return SafeV8Promise_GetOutput_Coerce1(isolate, first);
  }
}
