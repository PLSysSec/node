#include <assert.h>
#include <functional>
#include "v8.h"
#include "safe_v8_3.h"

using namespace v8;

namespace safeV8 {
//
//  template <typename E>
//  class OnErr {
//  public:
//    OnErr() : is_err(false) { }
//    OnErr(const E &e) : is_err(true), err(e) { }
//
//    void onErr(std::function<void(E)> func) const {
//      if (is_err) {
//        func(err);
//      }
//    }
//
//  private:
//    bool is_err;
//    E err;
//  };
//
//  template <typename E, typename V>
//  class MVal {
//  public:
//    MVal(const E& e) : is_err(true), err(e) { }
//    MVal(const V& v) : is_err(false), val(v) { }
//
//    bool IsError()  const { return is_err; }
//    bool IsVal() const { return !is_err; }
//
//    inline V Val(V defaultVal) {
//      if (is_err) {
//        return defaultVal;
//      }
//      else {
//        return val;
//      }
//    }
//
//    OnErr<E> onVal(std::function<void(V)> func) const {
//      if (!is_err) {
//        func(val);
//        return OnErr<E>();
//      }
//      else {
//        return OnErr<E>(err);
//      }
//    }
//
//  public:
//    // Unsafe interface
//    inline E UnsafeError() const {
//      assert(is_err);
//      return err;
//    }
//
//    inline V UnsafeVal() const {
//      assert(!is_err);
//      return val;
//    }
//
//  private:
//    bool is_err;
//    E err;
//    V val;
//    template <typename X, typename Y>
//    friend MVal<X, Y> Err(const X&);
//    template <typename X, typename Y>
//    friend MVal<X, Y> Val(const Y&);
//  };
//
//
//#define DEFINE_TY_VAL(Type) \
//  MVal<Local<Value>, Local<Type>> Type##Val(Isolate* isolate, Local<Value> v) { \
//    if (v->Is##Type()) { \
//      return MVal<Local<Value>, Local<Type>>(v.As<Type>()); \
//    } else { \
//      MaybeLocal<String> mErrMsg =  \
//        v8::String::NewFromUtf8(isolate, "Invalid type", v8::String::NewStringType::kNormalString); \
//      Local<Value> err = v8::Exception::TypeError(mErrMsg.ToLocalChecked()); \
//      return MVal<Local<Value>, Local<Type>>(err); \
//    } \
//  }
//
//#define TYPE_LIST(V)   \
//  V(Array)             \
//  V(ArrayBuffer)       \
//  V(ArrayBufferView)   \
//  V(Boolean)           \
//  V(DataView)          \
//  V(Date)              \
//  V(External)          \
//  V(Float32Array)      \
//  V(Float64Array)      \
//  V(Function)          \
//  V(Int16Array)        \
//  V(Int32)             \
//  V(Int32Array)        \
//  V(Int8Array)         \
//  V(Map)               \
//  V(Name)              \
//  V(Number)            \
//  V(Object)            \
//  V(Proxy)             \
//  V(RegExp)            \
//  V(Set)               \
//  V(SharedArrayBuffer) \
//  V(String)            \
//  V(StringObject)      \
//  V(Symbol)            \
//  V(TypedArray)        \
//  V(Uint16Array)       \
//  V(Uint32)            \
//  V(Uint32Array)       \
//  V(Uint8Array)        \
//  V(Uint8ClampedArray)
//
//  TYPE_LIST(DEFINE_TY_VAL);
//
//#undef TYPE_LIST
//#undef DEFINE_TY_VAL
//
//#define DEFINE_CTY_VAL(CType, JSType) \
//  MVal<Local<Value>, CType> CType##Val(Isolate* isolate, Local<Value> v) { \
//    if (v->Is##JSType()) { \
//      return MVal<Local<Value>, CType>(v->JSType##Value()); \
//    } else { \
//      MaybeLocal<String> mErrMsg =  \
//        v8::String::NewFromUtf8(isolate, "Invalid type", v8::String::NewStringType::kNormalString); \
//      Local<Value> err = v8::Exception::TypeError(mErrMsg.ToLocalChecked()); \
//      return MVal<Local<Value>, CType>(err); \
//    } \
//  }
//
//  DEFINE_CTY_VAL(bool, Boolean)
//    DEFINE_CTY_VAL(double, Number)
//    DEFINE_CTY_VAL(uint32_t, Uint32)
//    DEFINE_CTY_VAL(int32_t, Int32)
//#undef DEFINE_CTY_VAL
//
//    MVal<Local<Value>, int64_t>
//    int64_tVal(Isolate* isolate, Local<Value> v) {
//    int64_t num;
//    if (v->IsNumber() && v->IntegerValue(isolate->GetCurrentContext()).To(&num)) {
//      return MVal<Local<Value>, int64_t>(num);
//    }
//    else {
//      MaybeLocal<String> mErrMsg =
//        v8::String::NewFromUtf8(isolate, "Invalid type", v8::String::NewStringType::kNormalString);
//      Local<Value> err = v8::Exception::TypeError(mErrMsg.ToLocalChecked());
//      return MVal<Local<Value>, int64_t>(err);
//    }
//  }
//
//  ////////////////////////////////////////////////////////////////////////////////////////////
//
//#define DEFINE_TY_VAL(Type) \
//  bool SafeV8ConvertVal(Isolate* isolate, Local<Value> v, Local<Type>& outVal, Local<Value>& err, bool& hasError) { \
//    if (v->Is##Type()) { \
//      outVal = v.As<Type>(); \
//      hasError = false; \
//      return true; \
//    } else { \
//      MaybeLocal<String> mErrMsg =  \
//        v8::String::NewFromUtf8(isolate, "Invalid type", v8::String::NewStringType::kNormalString); \
//      err = v8::Exception::TypeError(mErrMsg.ToLocalChecked()); \
//      hasError = true; \
//      return false; \
//    } \
//  }
//
//#define TYPE_LIST(V)   \
//  V(Array)             \
//  V(ArrayBuffer)       \
//  V(ArrayBufferView)   \
//  V(Boolean)           \
//  V(DataView)          \
//  V(Date)              \
//  V(External)          \
//  V(Float32Array)      \
//  V(Float64Array)      \
//  V(Function)          \
//  V(Int16Array)        \
//  V(Int32)             \
//  V(Int32Array)        \
//  V(Int8Array)         \
//  V(Map)               \
//  V(Name)              \
//  V(Number)            \
//  V(Object)            \
//  V(Proxy)             \
//  V(RegExp)            \
//  V(Set)               \
//  V(SharedArrayBuffer) \
//  V(String)            \
//  V(StringObject)      \
//  V(Symbol)            \
//  V(TypedArray)        \
//  V(Uint16Array)       \
//  V(Uint32)            \
//  V(Uint32Array)       \
//  V(Uint8Array)        \
//  V(Uint8ClampedArray)
//
//  TYPE_LIST(DEFINE_TY_VAL);
//
//#undef TYPE_LIST
//#undef DEFINE_TY_VAL
//
//#define DEFINE_CTY_VAL(CType, JSType) \
//  bool SafeV8ConvertVal(Isolate* isolate, Local<Value> v, CType& outVal, Local<Value>& err, bool& hasError) { \
//    if (v->Is##JSType()) { \
//      outVal = v->JSType##Value(); \
//      hasError = false; \
//      return true; \
//    } else { \
//      MaybeLocal<String> mErrMsg =  \
//        v8::String::NewFromUtf8(isolate, "Invalid type", v8::String::NewStringType::kNormalString); \
//      err = v8::Exception::TypeError(mErrMsg.ToLocalChecked()); \
//      hasError = true; \
//      return false; \
//    } \
//  }
//
//  DEFINE_CTY_VAL(bool, Boolean)
//    DEFINE_CTY_VAL(double, Number)
//    DEFINE_CTY_VAL(uint32_t, Uint32)
//    DEFINE_CTY_VAL(int32_t, Int32)
//#undef DEFINE_CTY_VAL
//
//    //int64 version
//    bool SafeV8ConvertVal(Isolate* isolate, Local<Value> v, int64_t& outVal, Local<Value>& err, bool& hasError) {
//    if (v->IsNumber() && v->IntegerValue(isolate->GetCurrentContext()).To(&outVal)) {
//      hasError = false;
//      return true;
//    }
//    else {
//      MaybeLocal<String> mErrMsg =
//        v8::String::NewFromUtf8(isolate, "Invalid type", v8::String::NewStringType::kNormalString);
//      err = v8::Exception::TypeError(mErrMsg.ToLocalChecked());
//      hasError = true;
//      return false;
//    }
//  }
//
//  /* Base class from which the various output classes derive from */
//  class SafeV8Promise_GetOutput
//  {
//  protected:
//    Local<Value> err;
//    bool exceptionThrown = false;
//  public:
//    Local<Value> GetException()
//    {
//      return err;
//    }
//    bool GetIsExceptionThrown()
//    {
//      return exceptionThrown;
//    }
//  };
//
//  /* Class which handles the single output value case */
//  class SafeV8Promise_GetOutput1 : public SafeV8Promise_GetOutput
//  {
//  private:
//    Isolate* isolate;
//    Local<Value> v1;
//  public:
//    SafeV8Promise_GetOutput1(Isolate* _isolate, Local<Value> _v1) : v1(_v1), isolate(_isolate) {}
//
//    //Returns the marshalled and converted values. The lambda provided does not marshal additional values inside
//    template<typename F>
//    V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_same<return_argument<F>, void>::value, SafeV8Promise_GetOutput1>::type ToVal(F func)
//    {
//      first_argument<F> obj1;
//      if (SafeV8ConvertVal(isolate, v1, obj1, err, exceptionThrown))
//      {
//        func(obj1);
//      }
//
//      return *this;
//    }
//
//    //Returns the marshalled and converted values. The lambda provided does marshal additional values inside
//    template<typename F>
//    V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_base_of<SafeV8Promise_GetOutput, return_argument<F>>::value, SafeV8Promise_GetOutput1>::type ToVal(F func)
//    {
//      first_argument<F> obj1;
//      if (SafeV8ConvertVal(isolate, v1, obj1, err, exceptionThrown))
//      {
//        SafeV8Promise_GetOutput nestedCall = func(obj1);
//        exceptionThrown = nestedCall.GetIsExceptionThrown();
//        err = nestedCall.GetException();
//      }
//
//      return *this;
//    }
//
//    //Handle any errors caught so far. The error handling lambda provided does not marshal additional values inside
//    template<typename F>
//    typename std::enable_if<std::is_same<return_argument<F>, void>::value, void>::type OnErr(F func)
//    {
//      if (exceptionThrown)
//      {
//        func(err);
//      }
//    }
//
//    //Handle any errors caught so far. The error handling lambda provided does marshal additional values inside
//    template<typename F>
//    V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_base_of<SafeV8Promise_GetOutput, return_argument<F>>::value, SafeV8Promise_GetOutput1>::type OnErr(F func)
//    {
//      if (exceptionThrown)
//      {
//        SafeV8Promise_GetOutput nestedCall = func(err);
//        exceptionThrown = nestedCall.GetIsExceptionThrown();
//        err = nestedCall.GetException();
//      }
//      return *this;
//    }
//  };
//
//  /* Class which handles 2 output value case */
//  class SafeV8Promise_GetOutput2 : public SafeV8Promise_GetOutput
//  {
//  private:
//    Isolate* isolate;
//    Local<Value> v1;
//    Local<Value> v2;
//  public:
//    SafeV8Promise_GetOutput2(Isolate* _isolate, Local<Value> _v1, Local<Value> _v2) : v1(_v1), v2(_v2), isolate(_isolate) {}
//
//    //Returns the marshalled and converted values. The lambda provided does not marshal additional values inside
//    template<typename F>
//    V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_same<return_argument<F>, void>::value, SafeV8Promise_GetOutput2>::type ToVal(F func)
//    {
//      first_argument<F> obj1;
//      second_argument<F> obj2;
//      if (SafeV8ConvertVal(isolate, v1, obj1, err, exceptionThrown))
//      {
//        if (SafeV8ConvertVal(isolate, v2, obj2, err, exceptionThrown))
//        {
//          func(obj1, obj2);
//        }
//      }
//
//      return *this;
//    }
//
//    //Returns the marshalled and converted values. The lambda provided does marshal additional values inside
//    template<typename F>
//    V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_base_of<SafeV8Promise_GetOutput, return_argument<F>>::value, SafeV8Promise_GetOutput2>::type ToVal(F func)
//    {
//      first_argument<F> obj1;
//      second_argument<F> obj2;
//      if (SafeV8ConvertVal(isolate, v1, obj1, err, exceptionThrown))
//      {
//        if (SafeV8ConvertVal(isolate, v2, obj2, err, exceptionThrown))
//        {
//          SafeV8Promise_GetOutput nestedCall = func(obj1, obj2);
//          exceptionThrown = nestedCall.GetIsExceptionThrown();
//          err = nestedCall.GetException();
//        }
//      }
//
//      return *this;
//    }
//
//    //Handle any errors caught so far. The error handling lambda provided does not marshal additional values inside
//    template<typename F>
//    typename std::enable_if<std::is_same<return_argument<F>, void>::value, void>::type OnErr(F func)
//    {
//      if (exceptionThrown)
//      {
//        func(err);
//      }
//    }
//
//    //Handle any errors caught so far. The error handling lambda provided does marshal additional values inside
//    template<typename F>
//    V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_base_of<SafeV8Promise_GetOutput, return_argument<F>>::value, SafeV8Promise_GetOutput2>::type OnErr(F func)
//    {
//      if (exceptionThrown)
//      {
//        SafeV8Promise_GetOutput nestedCall = func(err);
//        exceptionThrown = nestedCall.GetIsExceptionThrown();
//        err = nestedCall.GetException();
//      }
//      return *this;
//    }
//  };
//
//  /* Class which handles 3 output value case */
//  class SafeV8Promise_GetOutput3 : public SafeV8Promise_GetOutput
//  {
//  private:
//    Isolate* isolate;
//    Local<Value> v1;
//    Local<Value> v2;
//    Local<Value> v3;
//  public:
//    SafeV8Promise_GetOutput3(Isolate* _isolate, Local<Value> _v1, Local<Value> _v2, Local<Value> _v3) : v1(_v1), v2(_v2), v3(_v3), isolate(_isolate) {}
//
//    //Returns the marshalled and converted values. The lambda provided does not marshal additional values inside
//    template<typename F>
//    V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_same<return_argument<F>, void>::value, SafeV8Promise_GetOutput3>::type ToVal(F func)
//    {
//      first_argument<F> obj1;
//      second_argument<F> obj2;
//      third_argument<F> obj3;
//      if (SafeV8ConvertVal(isolate, v1, obj1, err, exceptionThrown))
//      {
//        if (SafeV8ConvertVal(isolate, v2, obj2, err, exceptionThrown))
//        {
//          if (SafeV8ConvertVal(isolate, v3, obj3, err, exceptionThrown))
//          {
//            func(obj1, obj2, obj3);
//          }
//        }
//      }
//
//      return *this;
//    }
//
//    //Returns the marshalled and converted values. The lambda provided does marshal additional values inside
//    template<typename F>
//    V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_base_of<SafeV8Promise_GetOutput, return_argument<F>>::value, SafeV8Promise_GetOutput3>::type ToVal(F func)
//    {
//      first_argument<F> obj1;
//      second_argument<F> obj2;
//      third_argument<F> obj3;
//      if (SafeV8ConvertVal(isolate, v1, obj1, err, exceptionThrown))
//      {
//        if (SafeV8ConvertVal(isolate, v2, obj2, err, exceptionThrown))
//        {
//          if (SafeV8ConvertVal(isolate, v3, obj3, err, exceptionThrown))
//          {
//            SafeV8Promise_GetOutput nestedCall = func(obj1, obj2, obj3);
//            exceptionThrown = nestedCall.GetIsExceptionThrown();
//            err = nestedCall.GetException();
//          }
//        }
//      }
//
//      return *this;
//    }
//
//    //Handle any errors caught so far. The error handling lambda provided does not marshal additional values inside
//    template<typename F>
//    typename std::enable_if<std::is_same<return_argument<F>, void>::value, void>::type OnErr(F func)
//    {
//      if (exceptionThrown)
//      {
//        func(err);
//      }
//    }
//
//    //Handle any errors caught so far. The error handling lambda provided does marshal additional values inside
//    template<typename F>
//    V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_base_of<SafeV8Promise_GetOutput, return_argument<F>>::value, SafeV8Promise_GetOutput3>::type OnErr(F func)
//    {
//      if (exceptionThrown)
//      {
//        SafeV8Promise_GetOutput nestedCall = func(err);
//        exceptionThrown = nestedCall.GetIsExceptionThrown();
//        err = nestedCall.GetException();
//      }
//      return *this;
//    }
//  };
//
//  /* Entry point to users who want to use the SafeV8 api api */
//  V8_WARN_UNUSED_RESULT SafeV8Promise_GetOutput1 With(Isolate* isolate, Local<Value> first)
//  {
//    return SafeV8Promise_GetOutput1(isolate, first);
//  }
//
//  V8_WARN_UNUSED_RESULT SafeV8Promise_GetOutput2 With(Isolate* isolate, Local<Value> first, Local<Value> second)
//  {
//    return SafeV8Promise_GetOutput2(isolate, first, second);
//  }
//
//  V8_WARN_UNUSED_RESULT SafeV8Promise_GetOutput3 With(Isolate* isolate, Local<Value> first, Local<Value> second, Local<Value> third)
//  {
//    return SafeV8Promise_GetOutput3(isolate, first, second, third);
//  }
//
  //////// Get API ////////////

  template<typename ObjectType, typename KeyType>
  bool SafeV8Get(Local<Context> context, ObjectType object, KeyType key, Local<Value>& outVal, Local<Value>& err, bool& hasError) {
    if (object->Get(context, key).ToLocal(&outVal))
    {
      hasError = false;
      return true;
    }
    else
    {
      MaybeLocal<String> mErrMsg =
        v8::String::NewFromUtf8(context->GetIsolate(), "Get failed", v8::String::NewStringType::kNormalString);
      err = v8::Exception::TypeError(mErrMsg.ToLocalChecked());
      hasError = true;
      return false;
    }
  }

  //template<typename ObjectType, typename KeyType>
  //class SafeV8_GetterOutput : public SafeV8Promise_GetOutput
  //{
  //private:
  //  Local<Context> context;
  //  ObjectType object;
  //  KeyType key;
  //public:
    template<typename ObjectType, typename KeyType>
    SafeV8_GetterOutput::SafeV8_GetterOutput(Local<Context> _context, ObjectType _object, KeyType _key) : context(_context), object(_object), key(_key) {}

    //Returns the marshalled and converted values. The lambda provided does not marshal additional values inside
    template<typename F>
    V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_same<return_argument<F>, void>::value, SafeV8_GetterOutput>::type SafeV8_GetterOutput::ToVal(F func)
    {
      Local<Value> outVal;
      if (SafeV8Get(context, object, key, outVal, err, exceptionThrown))
      {
        func(outVal);
      }

      return *this;
    }

    template<typename F>
    void SafeV8_GetterOutput::OnErr(F func)
    {
      if (exceptionThrown)
      {
        func(err);
      }
    }
  //};

  template<typename ObjectType, typename KeyType>
  V8_WARN_UNUSED_RESULT SafeV8_GetterOutput<ObjectType, KeyType> GetField(Local<Context> context, ObjectType object, KeyType key)
  {
    return SafeV8_GetterOutput<ObjectType, KeyType>(context, object, key);
  }


  //////// Set API ////////////

  //template<typename ObjectType, typename KeyType>
  //bool SafeV8Set(Local<Context> context, ObjectType object, KeyType key, Local<Value> val, Local<Value>& err, bool& hasError) {
  //  if (object->Set(context, key, val).FromMaybe(false))
  //  {
  //    hasError = false;
  //    return true;
  //  }
  //  else
  //  {
  //    MaybeLocal<String> mErrMsg =
  //      v8::String::NewFromUtf8(context->GetIsolate(), "Set failed", v8::String::NewStringType::kNormalString);
  //    err = v8::Exception::TypeError(mErrMsg.ToLocalChecked());
  //    hasError = true;
  //    return false;
  //  }
  //}

  ////template<typename ObjectType, typename KeyType>
  ////class SafeV8_SetterOutput : public SafeV8Promise_GetOutput
  ////{
  ////public:
  //  template<typename ObjectType, typename KeyType>
  //  V8_WARN_UNUSED_RESULT SafeV8_SetterOutput::SafeV8_SetterOutput(Local<Context> context, ObjectType object, KeyType key, Local<Value> val)
  //  {
  //    SafeV8Set(context, object, key, val, err, exceptionThrown);
  //  }

  //  template<typename ObjectType, typename KeyType>
  //  V8_WARN_UNUSED_RESULT SafeV8_SetterOutput::SafeV8_SetterOutput(Local<Value> exception)
  //  {
  //    exceptionThrown = true;
  //    err = exception;
  //  }

  //  template<typename F>
  //  void SafeV8_SetterOutput::OnErr(F func)
  //  {
  //    if (exceptionThrown)
  //    {
  //      func(err);
  //    }
  //  }
  ////};


  //template<typename ObjectType, typename KeyType>
  //V8_WARN_UNUSED_RESULT SafeV8_SetterOutput<ObjectType, KeyType> SetField(Local<Context> context, ObjectType object, KeyType key, Local<Value> val)
  //{
  //  return SafeV8_SetterOutput<ObjectType, KeyType>(context, object, key, val);
  //}

  //template<typename ObjectType, typename KeyType, typename GetObjectType, typename GetKeyType>
  //V8_WARN_UNUSED_RESULT SafeV8_SetterOutput<ObjectType, KeyType> SetField(Local<Context> context, ObjectType object, KeyType key, SafeV8_GetterOutput<GetObjectType, GetKeyType> val)
  //{
  //  SafeV8_SetterOutput<ObjectType, KeyType>* ptr;

  //  val.ToVal([&](Local<Value> result) {
  //    ptr = new SafeV8_SetterOutput<ObjectType, KeyType>(context, object, key, result);
  //  }).OnErr([&](Local<Value> exception) {
  //    ptr = new SafeV8_SetterOutput<ObjectType, KeyType>(exception);
  //  });

  //  SafeV8_SetterOutput<ObjectType, KeyType> ret(*ptr);
  //  return ret;
  //}
}
