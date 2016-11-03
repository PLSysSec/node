#ifndef SAFE_V8_H_
#define SAFE_V8_H_

#include <assert.h>
#include <functional>
#include "v8.h"

using namespace v8;

namespace safeV8 {



  template <typename E, typename V>
  class MVal;

  template <typename E>
  class OnErr {
    public:
    OnErr () : is_err(false) { }
    OnErr (const E &e) : is_err(true), err(e) { }

    void onErr(std::function<void (E)> func) const {
      if (is_err) {
        func(err);
      }
    }

    private:
    bool is_err;
    E err;
  };

  template <typename E, typename V>
  class MVal {
    public:
      MVal(const E& e) : is_err(true), err (e) { }
      MVal(const V& v) : is_err(false), val (v) { }

      bool IsError()  const { return is_err; }
      bool IsVal() const { return !is_err; }

      inline V Val(V defaultVal) {
        if (is_err) {
          return defaultVal;
        } else {
          return val;
        }
      }

      OnErr<E> onVal(std::function<void (V)> func) const {
        if (!is_err) {
          func(val);
          return OnErr<E>();
        } else {
          return OnErr<E>(err);
        }
      }

    public:
      // Unsafe interface
      inline E UnsafeError() const {
        assert(is_err);
        return err;
      }

      inline V UnsafeVal() const {
        assert(!is_err);
        return val;
      }

    private:
      bool is_err;
      E err;
      V val;
      template <typename X, typename Y>
        friend MVal<X, Y> Err(const X&);
      template <typename X, typename Y>
        friend MVal<X, Y> Val(const Y&);
  };


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
  }

  DEFINE_CTY_VAL(bool, Boolean)
  DEFINE_CTY_VAL(double, Number)
  DEFINE_CTY_VAL(uint32_t, Uint32)
  DEFINE_CTY_VAL(int32_t, Int32)
#undef DEFINE_CTY_VAL

  MVal<Local<Value>, int64_t>
  int64_tVal(Isolate* isolate, Local<Value> v) {
    int64_t num;
    if (v->IsNumber() && v->IntegerValue(isolate->GetCurrentContext()).To(&num)) {
      return MVal<Local<Value>, int64_t>(num);
    } else {
      MaybeLocal<String> mErrMsg =
        v8::String::NewFromUtf8(isolate, "Invalid type", v8::String::NewStringType::kNormalString);
      Local<Value> err = v8::Exception::TypeError(mErrMsg.ToLocalChecked());
      return MVal<Local<Value>, int64_t>(err);
    }
  }


#define MARSHALL(mval) \
  do { \
    auto __safe_v8_local_mv = (mval); \
    if (__safe_v8_local_mv.IsVal()) {

#define AS(val) \
      (val) =  __safe_v8_local_mv.UnsafeVal(); \
    } else {

#define OR_THROW(isolate, errorKind, errMsg) \
      MaybeLocal<String> mErrMsg =  \
        v8::String::NewFromUtf8(isolate, (errMsg), v8::String::NewStringType::kNormalString); \
      isolate->ThrowException(v8::Exception::errorKind(mErrMsg.ToLocalChecked())); \
      return; \
    } \
  } while(0);

}

#endif  // SAFE_V8_H_
