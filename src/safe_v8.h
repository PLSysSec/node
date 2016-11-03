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

/* Base class from which the various output classes derive from */
class SafeV8Promise_GetOutput
{
protected:
  Local<Value> err;
  bool exceptionThrown = false;
public:
  Local<Value> GetException()
  {
    return err;
  }
  bool GetIsExceptionThrown()
  {
    return exceptionThrown;
  }
};

/* Class which handles the single output value case */
class SafeV8Promise_GetOutput1 : public SafeV8Promise_GetOutput
{
private:
  Isolate* isolate;
  Local<Value> v1;
public:
  SafeV8Promise_GetOutput1(Isolate* _isolate, Local<Value> _v1) : v1(_v1), isolate(_isolate){}

  //Returns the marshalled and converted values. The lambda provided does not marshal additional values inside
  template<typename F>
  std::enable_if_t<std::is_same<return_argument<F>, void>::value, SafeV8Promise_GetOutput1> ToVal(F func)
  {
    first_argument<F> obj1;
    if (SafeV8ConvertVal(isolate, v1, obj1, err, exceptionThrown))
    {
      func(obj1);
    }

    return *this;
  }

  //Returns the marshalled and converted values. The lambda provided does marshal additional values inside
  template<typename F>
  std::enable_if_t<std::is_base_of<SafeV8Promise_GetOutput, return_argument<F>>::value, SafeV8Promise_GetOutput1> ToVal(F func)
  {
    first_argument<F> obj1;
    if (SafeV8ConvertVal(isolate, v1, obj1, err, exceptionThrown))
    {
      SafeV8Promise_GetOutput nestedCall = func(obj1);
      exceptionThrown = nestedCall.GetIsExceptionThrown();
      err = nestedCall.GetException();
    }

    return *this;
  }

  //Handle any errors caught so far. The error handling lambda provided does not marshal additional values inside
  template<typename F>
  std::enable_if_t<std::is_same<return_argument<F>, void>::value, void> OnErr(F func)
  {
    if (exceptionThrown)
    {
      func(err);
    }
  }

  //Handle any errors caught so far. The error handling lambda provided does marshal additional values inside
  template<typename F>
  std::enable_if_t<std::is_base_of<SafeV8Promise_GetOutput, return_argument<F>>::value, SafeV8Promise_GetOutput1> OnErr(F func)
  {
    if (exceptionThrown)
    {
      SafeV8Promise_GetOutput nestedCall = func(err);
      exceptionThrown = nestedCall.GetIsExceptionThrown();
      err = nestedCall.GetException();
    }
    return *this;
  }
};

/* Class which handles 2 output value case */
class SafeV8Promise_GetOutput2 : public SafeV8Promise_GetOutput
{
private:
  Isolate* isolate;
  Local<Value> v1;
  Local<Value> v2;
public:
  SafeV8Promise_GetOutput2(Isolate* _isolate, Local<Value> _v1, Local<Value> _v2) : v1(_v1), v2(_v2), isolate(_isolate) {}

  //Returns the marshalled and converted values. The lambda provided does not marshal additional values inside
  template<typename F>
  std::enable_if_t<std::is_same<return_argument<F>, void>::value, SafeV8Promise_GetOutput2> ToVal(F func)
  {
    first_argument<F> obj1;
    second_argument<F> obj2;
    if (SafeV8ConvertVal(isolate, v1, obj1, err, exceptionThrown))
    {
      if (SafeV8ConvertVal(isolate, v2, obj2, err, exceptionThrown))
      {
        func(obj1, obj2);
      }
    }

    return *this;
  }

  //Returns the marshalled and converted values. The lambda provided does marshal additional values inside
  template<typename F>
  std::enable_if_t<std::is_base_of<SafeV8Promise_GetOutput, return_argument<F>>::value, SafeV8Promise_GetOutput2> ToVal(F func)
  {
    first_argument<F> obj1;
    second_argument<F> obj2;
    if (SafeV8ConvertVal(isolate, v1, obj1, err, exceptionThrown))
    {
      if (SafeV8ConvertVal(isolate, v2, obj2, err, exceptionThrown))
      {
        SafeV8Promise_GetOutput nestedCall = func(obj1, obj2);
        exceptionThrown = nestedCall.GetIsExceptionThrown();
        err = nestedCall.GetException();
      }
    }

    return *this;
  }

  //Handle any errors caught so far. The error handling lambda provided does not marshal additional values inside
  template<typename F>
  std::enable_if_t<std::is_same<return_argument<F>, void>::value, void> OnErr(F func)
  {
    if (exceptionThrown)
    {
      func(err);
    }
  }

  //Handle any errors caught so far. The error handling lambda provided does marshal additional values inside
  template<typename F>
  std::enable_if_t<std::is_base_of<SafeV8Promise_GetOutput, return_argument<F>>::value, SafeV8Promise_GetOutput2> OnErr(F func)
  {
    if (exceptionThrown)
    {
      SafeV8Promise_GetOutput nestedCall = func(err);
      exceptionThrown = nestedCall.GetIsExceptionThrown();
      err = nestedCall.GetException();
    }
    return *this;
  }
};

/* Class which handles 3 output value case */
class SafeV8Promise_GetOutput3 : public SafeV8Promise_GetOutput
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
  std::enable_if_t<std::is_same<return_argument<F>, void>::value, SafeV8Promise_GetOutput3> ToVal(F func)
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
        }
      }
    }

    return *this;
  }

  //Returns the marshalled and converted values. The lambda provided does marshal additional values inside
  template<typename F>
  std::enable_if_t<std::is_base_of<SafeV8Promise_GetOutput, return_argument<F>>::value, SafeV8Promise_GetOutput3> ToVal(F func)
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
          SafeV8Promise_GetOutput nestedCall = func(obj1, obj2, obj3);
          exceptionThrown = nestedCall.GetIsExceptionThrown();
          err = nestedCall.GetException();
        }
      }
    }

    return *this;
  }

  //Handle any errors caught so far. The error handling lambda provided does not marshal additional values inside
  template<typename F>
  std::enable_if_t<std::is_same<return_argument<F>, void>::value, void> OnErr(F func)
  {
    if (exceptionThrown)
    {
      func(err);
    }
  }

  //Handle any errors caught so far. The error handling lambda provided does marshal additional values inside
  template<typename F>
  std::enable_if_t<std::is_base_of<SafeV8Promise_GetOutput, return_argument<F>>::value, SafeV8Promise_GetOutput3> OnErr(F func)
  {
    if (exceptionThrown)
    {
      SafeV8Promise_GetOutput nestedCall = func(err);
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

}
#endif  // SAFE_V8_H_
