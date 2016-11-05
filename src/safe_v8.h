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

////////////////////////////////////////////////////////////////////////////////////////////

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


/* Returns the first argument type of a given lambda */

template<typename Ret, typename Arg, typename... Rest>
Arg first_argument_helper(Ret(*) (Arg, Rest...));

template<typename Ret, typename F, typename Arg, typename... Rest>
Arg first_argument_helper(Ret(F::*) (Arg, Rest...));

template<typename Ret, typename F, typename Arg, typename... Rest>
Arg first_argument_helper(Ret(F::*) (Arg, Rest...) const);

template <typename F>
decltype(first_argument_helper(&F::operator())) first_argument_helper(F);

template <typename T>
using first_argument = decltype(first_argument_helper(std::declval<T>()));

/* Returns the second argument type of a given lambda */

template<typename Ret, typename Arg, typename Arg2, typename... Rest>
Arg2 second_argument_helper(Ret(*) (Arg, Arg2, Rest...));

template<typename Ret, typename F, typename Arg, typename Arg2, typename... Rest>
Arg2 second_argument_helper(Ret(F::*) (Arg, Arg2, Rest...));

template<typename Ret, typename F, typename Arg, typename Arg2, typename... Rest>
Arg2 second_argument_helper(Ret(F::*) (Arg, Arg2, Rest...) const);

template <typename F>
decltype(second_argument_helper(&F::operator())) second_argument_helper(F);

template <typename T>
using second_argument = decltype(second_argument_helper(std::declval<T>()));

/* Returns the third argument type of a given lambda */

template<typename Ret, typename Arg, typename Arg2, typename Arg3, typename... Rest>
Arg3 third_argument_helper(Ret(*) (Arg, Arg2, Arg3, Rest...));

template<typename Ret, typename F, typename Arg, typename Arg2, typename Arg3, typename... Rest>
Arg3 third_argument_helper(Ret(F::*) (Arg, Arg2, Arg3, Rest...));

template<typename Ret, typename F, typename Arg, typename Arg2, typename Arg3, typename... Rest>
Arg3 third_argument_helper(Ret(F::*) (Arg, Arg2, Arg3, Rest...) const);

template <typename F>
decltype(third_argument_helper(&F::operator())) third_argument_helper(F);

template <typename T>
using third_argument = decltype(third_argument_helper(std::declval<T>()));

/* Returns the return argument type of a given lambda */

template<typename Ret, typename... Rest>
Ret return_argument_helper(Ret(*) (Rest...));

template<typename Ret, typename F, typename... Rest>
Ret return_argument_helper(Ret(F::*) (Rest...));

template<typename Ret, typename F, typename... Rest>
Ret return_argument_helper(Ret(F::*) (Rest...) const);

template <typename F>
decltype(return_argument_helper(&F::operator())) return_argument_helper(F);

template <typename T>
using return_argument = decltype(return_argument_helper(std::declval<T>()));

class SafeV8Promise_Base;

static SafeV8Promise_Base Err(Local<Value> err);
static SafeV8Promise_Base Err(Isolate* isolate, char* err);
static SafeV8Promise_Base Err(Isolate* isolate, char* err, v8::Local<v8::Value>(*errorType)(v8::Local<v8::String>));

/* Base class from which the various output classes derive from */
class SafeV8Promise_Base
{
protected:
  Local<Value> err;
  bool exceptionThrown = false;
public:
  SafeV8Promise_Base() {}

  Local<Value> GetException()
  {
    return err;
  }
  bool GetIsExceptionThrown()
  {
    return exceptionThrown;
  }

  friend SafeV8Promise_Base Err(Local<Value> err);
  friend SafeV8Promise_Base Err(Isolate* isolate, char* err);
  friend SafeV8Promise_Base Err(Isolate* isolate, char* err, v8::Local<v8::Value>(*errorType)(v8::Local<v8::String>));
};

const SafeV8Promise_Base Done;

static SafeV8Promise_Base Err(Local<Value> err)
{
  SafeV8Promise_Base e;
  e.exceptionThrown = true;
  e.err = err;
}

static SafeV8Promise_Base Err(Isolate* isolate, char* err)
{
  SafeV8Promise_Base e;
  e.exceptionThrown = true;
  MaybeLocal<String> mErrMsg = v8::String::NewFromUtf8(isolate, err, v8::String::NewStringType::kNormalString);
  e.err = v8::Exception::TypeError(mErrMsg.ToLocalChecked());
  return e;
}

static SafeV8Promise_Base Err(Isolate* isolate, char* err, v8::Local<v8::Value>(*errorType)(v8::Local<v8::String>))
{
  SafeV8Promise_Base e;
  e.exceptionThrown = true;
  MaybeLocal<String> mErrMsg = v8::String::NewFromUtf8(isolate, err, v8::String::NewStringType::kNormalString);
  e.err = errorType(mErrMsg.ToLocalChecked());
  return e;
}

static v8::Local<v8::Value> V8Err(Isolate* isolate, char* err, v8::Local<v8::Value>(*errorType)(v8::Local<v8::String>))
{
  MaybeLocal<String> mErrMsg = v8::String::NewFromUtf8(isolate, err, v8::String::NewStringType::kNormalString);
  auto ret = errorType(mErrMsg.ToLocalChecked());
  return ret;
}

/* Class which handles the single output value case */
class SafeV8Promise_GetOutput1 : public SafeV8Promise_Base
{
private:
  Isolate* isolate;
  Local<Value> v1;
public:
  SafeV8Promise_GetOutput1(Isolate* _isolate, Local<Value> _v1) : v1(_v1), isolate(_isolate){}

  //Returns the marshalled and converted values. The lambda provided does not marshal additional values inside
  template<typename F>
  V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_same<return_argument<F>, void>::value, SafeV8Promise_GetOutput1>::type OnVal(F func, v8::Local<v8::Value> customException = v8::Local<v8::Value>())
  {
    first_argument<F> obj1;
    if (SafeV8ConvertVal(isolate, v1, obj1, err, exceptionThrown))
    {
      func(obj1);
      return *this;
    }
    
    if (customException.IsEmpty())
    {
      err = customException;
    }
    return *this;
  }

  //Returns the marshalled and converted values. The lambda provided does marshal additional values inside
  template<typename F>
  V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_base_of<SafeV8Promise_Base, return_argument<F>>::value, SafeV8Promise_GetOutput1>::type OnVal(F func, v8::Local<v8::Value> customException = v8::Local<v8::Value>())
  {
    first_argument<F> obj1;
    if (SafeV8ConvertVal(isolate, v1, obj1, err, exceptionThrown))
    {
      SafeV8Promise_Base nestedCall = func(obj1);
      exceptionThrown = nestedCall.GetIsExceptionThrown();
      err = nestedCall.GetException();
      return *this;
    }

    if (customException.IsEmpty())
    {
      err = customException;
    }
    return *this;
  }

  //Handle any errors caught so far. The error handling lambda provided does not marshal additional values inside
  template<typename F>
  typename std::enable_if<std::is_same<return_argument<F>, void>::value, void>::type OnErr(F func)
  {
    if (exceptionThrown)
    {
      func(err);
    }
  }

  //Handle any errors caught so far. The error handling lambda provided does marshal additional values inside
  template<typename F>
  V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_base_of<SafeV8Promise_Base, return_argument<F>>::value, SafeV8Promise_GetOutput1>::type OnErr(F func)
  {
    if (exceptionThrown)
    {
      SafeV8Promise_Base nestedCall = func(err);
      exceptionThrown = nestedCall.GetIsExceptionThrown();
      err = nestedCall.GetException();
    }
    return *this;
  }
};

/* Class which handles 2 output value case */
class SafeV8Promise_GetOutput2 : public SafeV8Promise_Base
{
private:
  Isolate* isolate;
  Local<Value> v1;
  Local<Value> v2;
public:
  SafeV8Promise_GetOutput2(Isolate* _isolate, Local<Value> _v1, Local<Value> _v2) : v1(_v1), v2(_v2), isolate(_isolate) {}

  //Returns the marshalled and converted values. The lambda provided does not marshal additional values inside
  template<typename F>
  V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_same<return_argument<F>, void>::value, SafeV8Promise_GetOutput2>::type OnVal(F func, v8::Local<v8::Value> customException = v8::Local<v8::Value>())
  {
    first_argument<F> obj1;
    second_argument<F> obj2;
    if (SafeV8ConvertVal(isolate, v1, obj1, err, exceptionThrown))
    {
      if (SafeV8ConvertVal(isolate, v2, obj2, err, exceptionThrown))
      {
        func(obj1, obj2);
        return *this;
      }
    }

    if (customException.IsEmpty())
    {
      err = customException;
    }
    return *this;
  }

  //Returns the marshalled and converted values. The lambda provided does marshal additional values inside
  template<typename F>
  V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_base_of<SafeV8Promise_Base, return_argument<F>>::value, SafeV8Promise_GetOutput2>::type OnVal(F func, v8::Local<v8::Value> customException = v8::Local<v8::Value>())
  {
    first_argument<F> obj1;
    second_argument<F> obj2;
    if (SafeV8ConvertVal(isolate, v1, obj1, err, exceptionThrown))
    {
      if (SafeV8ConvertVal(isolate, v2, obj2, err, exceptionThrown))
      {
        SafeV8Promise_Base nestedCall = func(obj1, obj2);
        exceptionThrown = nestedCall.GetIsExceptionThrown();
        err = nestedCall.GetException();
        return *this;
      }
    }

    if (customException.IsEmpty())
    {
      err = customException;
    }
    return *this;
  }

  //Handle any errors caught so far. The error handling lambda provided does not marshal additional values inside
  template<typename F>
  typename std::enable_if<std::is_same<return_argument<F>, void>::value, void>::type OnErr(F func)
  {
    if (exceptionThrown)
    {
      func(err);
    }
  }

  //Handle any errors caught so far. The error handling lambda provided does marshal additional values inside
  template<typename F>
  V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_base_of<SafeV8Promise_Base, return_argument<F>>::value, SafeV8Promise_GetOutput2>::type OnErr(F func)
  {
    if (exceptionThrown)
    {
      SafeV8Promise_Base nestedCall = func(err);
      exceptionThrown = nestedCall.GetIsExceptionThrown();
      err = nestedCall.GetException();
    }
    return *this;
  }
};

/* Class which handles 3 output value case */
class SafeV8Promise_GetOutput3 : public SafeV8Promise_Base
{
private:
  Isolate* isolate;
  Local<Value> v1;
  Local<Value> v2;
  Local<Value> v3;
public:
  SafeV8Promise_GetOutput3(Isolate* _isolate, Local<Value> _v1, Local<Value> _v2, Local<Value> _v3) : v1(_v1), v2(_v2), v3(_v3), isolate(_isolate) {}

  //Returns the marshalled and converted values. The lambda provided does not marshal additional values inside
  template<typename F>
  V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_same<return_argument<F>, void>::value, SafeV8Promise_GetOutput3>::type OnVal(F func, v8::Local<v8::Value> customException = v8::Local<v8::Value>())
  {
    first_argument<F> obj1;
    second_argument<F> obj2;
    third_argument<F> obj3;
    if (SafeV8ConvertVal(isolate, v1, obj1, err, exceptionThrown))
    {
      if (SafeV8ConvertVal(isolate, v2, obj2, err, exceptionThrown))
      {
        if (SafeV8ConvertVal(isolate, v3, obj3, err, exceptionThrown))
        {
          func(obj1, obj2, obj3);
          return *this;
        }
      }
    }

    if (customException.IsEmpty())
    {
      err = customException;
    }
    return *this;
  }

  //Returns the marshalled and converted values. The lambda provided does marshal additional values inside
  template<typename F>
  V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_base_of<SafeV8Promise_Base, return_argument<F>>::value, SafeV8Promise_GetOutput3>::type OnVal(F func, v8::Local<v8::Value> customException = v8::Local<v8::Value>())
  {
    first_argument<F> obj1;
    second_argument<F> obj2;
    third_argument<F> obj3;
    if (SafeV8ConvertVal(isolate, v1, obj1, err, exceptionThrown))
    {
      if (SafeV8ConvertVal(isolate, v2, obj2, err, exceptionThrown))
      {
        if (SafeV8ConvertVal(isolate, v3, obj3, err, exceptionThrown))
        {
          SafeV8Promise_Base nestedCall = func(obj1, obj2, obj3);
          exceptionThrown = nestedCall.GetIsExceptionThrown();
          err = nestedCall.GetException();
          return *this;
        }
      }
    }

    if (customException.IsEmpty())
    {
      err = customException;
    }
    return *this;
  }

  //Handle any errors caught so far. The error handling lambda provided does not marshal additional values inside
  template<typename F>
  typename std::enable_if<std::is_same<return_argument<F>, void>::value, void>::type OnErr(F func)
  {
    if (exceptionThrown)
    {
      func(err);
    }
  }

  //Handle any errors caught so far. The error handling lambda provided does marshal additional values inside
  template<typename F>
  V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_base_of<SafeV8Promise_Base, return_argument<F>>::value, SafeV8Promise_GetOutput3>::type OnErr(F func)
  {
    if (exceptionThrown)
    {
      SafeV8Promise_Base nestedCall = func(err);
      exceptionThrown = nestedCall.GetIsExceptionThrown();
      err = nestedCall.GetException();
    }
    return *this;
  }
};

/* Entry point to users who want to use the SafeV8 api api */
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

template<typename ObjectType, typename KeyType>
class SafeV8_GetterOutput : public SafeV8Promise_Base
{
private:
  Local<Context> context;
  ObjectType object;
  KeyType key;
public:
  SafeV8_GetterOutput(Local<Context> _context, ObjectType _object, KeyType _key) : context(_context), object(_object), key(_key){}

  //Returns the marshalled and converted values. The lambda provided does not marshal additional values inside
  template<typename F>
  V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_same<return_argument<F>, void>::value, SafeV8_GetterOutput>::type OnVal(F func, v8::Local<v8::Value> customException = v8::Local<v8::Value>())
  {
    Local<Value> outVal;
    if (SafeV8Get(context, object, key, outVal, err, exceptionThrown))
    {
      func(outVal);
      return *this;
    }

    if (customException.IsEmpty())
    {
      err = customException;
    }
    return *this;
  }

  //Returns the marshalled and converted values. The lambda provided does marshal additional values inside
  template<typename F>
  V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_base_of<SafeV8Promise_Base, return_argument<F>>::value, SafeV8_GetterOutput>::type OnVal(F func, v8::Local<v8::Value> customException = v8::Local<v8::Value>())
  {
    Local<Value> outVal;
    if (SafeV8Get(context, object, key, outVal, err, exceptionThrown))
    {
      SafeV8Promise_Base nestedCall = func(outVal);
      exceptionThrown = nestedCall.GetIsExceptionThrown();
      err = nestedCall.GetException();
      return *this;
    }

    if (customException.IsEmpty())
    {
      err = customException;
    }
    return *this;
  }

  //Handle any errors caught so far. The error handling lambda provided does not marshal additional values inside
  template<typename F>
  typename std::enable_if<std::is_same<return_argument<F>, void>::value, void>::type OnErr(F func)
  {
    if (exceptionThrown)
    {
      func(err);
    }
  }

  //Handle any errors caught so far. The error handling lambda provided does marshal additional values inside
  template<typename F>
  V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_base_of<SafeV8Promise_Base, return_argument<F>>::value, SafeV8_GetterOutput>::type OnErr(F func)
  {
    if (exceptionThrown)
    {
      SafeV8Promise_Base nestedCall = func(err);
      exceptionThrown = nestedCall.GetIsExceptionThrown();
      err = nestedCall.GetException();
    }
    return *this;
  }
};

template<typename ObjectType, typename KeyType>
V8_WARN_UNUSED_RESULT SafeV8_GetterOutput<ObjectType, KeyType> GetField(Local<Context> context, ObjectType object, KeyType key)
{
  return SafeV8_GetterOutput<ObjectType, KeyType>(context, object, key);
}


//////// Set API ////////////

template<typename ObjectType, typename KeyType>
bool SafeV8Set(Local<Context> context, ObjectType object, KeyType key, Local<Value> val, Local<Value>& err, bool& hasError) {
  if (object->Set(context, key, val).FromMaybe(false))
  {
    hasError = false;
    return true;
  }
  else
  {
    MaybeLocal<String> mErrMsg =
      v8::String::NewFromUtf8(context->GetIsolate(), "Set failed", v8::String::NewStringType::kNormalString);
    err = v8::Exception::TypeError(mErrMsg.ToLocalChecked());
    hasError = true;
    return false;
  }
}

template<typename ObjectType, typename KeyType>
class SafeV8_SetterOutput : public SafeV8Promise_Base
{
private:
  Local<Context> context;
  ObjectType object;
  KeyType key;
  Local<Value> val;
public:
  SafeV8_SetterOutput(Local<Context> _context, ObjectType _object, KeyType _key, Local<Value> _val) : context(_context), object(_object), key(_key), val(_val) {}

  V8_WARN_UNUSED_RESULT SafeV8_SetterOutput(Local<Value> exception)
  {
    exceptionThrown = true;
    err = exception;
  }

  //Returns the marshalled and converted values. The lambda provided does not marshal additional values inside
  template<typename F>
  V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_same<return_argument<F>, void>::value, SafeV8_SetterOutput>::type OnVal(F func, v8::Local<v8::Value> customException = v8::Local<v8::Value>())
  {
    if (SafeV8Set(context, object, key, val, err, exceptionThrown))
    {
      func();
      return *this;
    }

    if (customException.IsEmpty())
    {
      err = customException;
    }
    return *this;
  }

  //Returns the marshalled and converted values. The lambda provided does marshal additional values inside
  template<typename F>
  V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_base_of<SafeV8Promise_Base, return_argument<F>>::value, SafeV8_SetterOutput>::type OnVal(F func, v8::Local<v8::Value> customException = v8::Local<v8::Value>())
  {
    if (SafeV8Set(context, object, key, val, err, exceptionThrown))
    {
      SafeV8Promise_Base nestedCall = func();
      exceptionThrown = nestedCall.GetIsExceptionThrown();
      err = nestedCall.GetException();
      return *this;
    }

    if (customException.IsEmpty())
    {
      err = customException;
    }
    return *this;
  }

  //Handle any errors caught so far. The error handling lambda provided does not marshal additional values inside
  template<typename F>
  typename std::enable_if<std::is_same<return_argument<F>, void>::value, void>::type OnErr(F func)
  {
    if (exceptionThrown)
    {
      func(err);
    }
  }

  //Handle any errors caught so far. The error handling lambda provided does marshal additional values inside
  template<typename F>
  V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_base_of<SafeV8Promise_Base, return_argument<F>>::value, SafeV8_SetterOutput>::type OnErr(F func)
  {
    if (exceptionThrown)
    {
      SafeV8Promise_Base nestedCall = func(err);
      exceptionThrown = nestedCall.GetIsExceptionThrown();
      err = nestedCall.GetException();
    }
    return *this;
  }
};

template<typename ObjectType, typename KeyType>
V8_WARN_UNUSED_RESULT SafeV8_SetterOutput<ObjectType, KeyType> SetField(Local<Context> context, ObjectType object, KeyType key, Local<Value> val)
{
  return SafeV8_SetterOutput<ObjectType, KeyType>(context, object, key, val);
}

template<typename ObjectType, typename KeyType, typename GetObjectType, typename GetKeyType>
V8_WARN_UNUSED_RESULT SafeV8_SetterOutput<ObjectType, KeyType> SetField(Local<Context> context, ObjectType object, KeyType key, SafeV8_GetterOutput<GetObjectType, GetKeyType> val)
{
  SafeV8_SetterOutput<ObjectType, KeyType>* ptr;

  val.OnVal([&](Local<Value> result) {
    ptr = new SafeV8_SetterOutput<ObjectType, KeyType>(context, object, key, result);
  }).OnErr([&](Local<Value> exception) {
    ptr = new SafeV8_SetterOutput<ObjectType, KeyType>(exception);
  });

  SafeV8_SetterOutput<ObjectType, KeyType> ret(*ptr);
  return ret;
}




}
#endif  // SAFE_V8_H_
