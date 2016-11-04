#pragma once

#include <assert.h>
#include <functional>
#include "v8.h"

using namespace v8;

namespace safeV8 {


//
//  template <typename E, typename V>
//  class MVal;
//
//  template <typename E>
//  class OnErr {
//  public:
//    OnErr();
//    OnErr(const E &e);
//
//    void onErr(std::function<void(E)> func) const;
//
//  private:
//    bool is_err;
//    E err;
//  };
//
//  template <typename E, typename V>
//  class MVal {
//  public:
//    MVal(const E& e);
//    MVal(const V& v);
//
//    bool IsError();
//    bool IsVal();
//
//    inline V Val(V defaultVal);
//
//    OnErr<E> onVal(std::function<void(V)> func);
//
//  public:
//    // Unsafe interface
//    inline E UnsafeError();
//
//    inline V UnsafeVal();
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
//  MVal<Local<Value>, Local<Type>> Type##Val(Isolate* isolate, Local<Value> v);
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
//  MVal<Local<Value>, CType> CType##Val(Isolate* isolate, Local<Value> v);
//
//  DEFINE_CTY_VAL(bool, Boolean)
//    DEFINE_CTY_VAL(double, Number)
//    DEFINE_CTY_VAL(uint32_t, Uint32)
//    DEFINE_CTY_VAL(int32_t, Int32)
//#undef DEFINE_CTY_VAL
//
//    MVal<Local<Value>, int64_t>
//    int64_tVal(Isolate* isolate, Local<Value> v);
//
//
//#define MARSHALL(mval) \
//  do { \
//    auto __safe_v8_local_mv = (mval); \
//    if (__safe_v8_local_mv.IsVal()) {
//
//#define AS(val) \
//      (val) =  __safe_v8_local_mv.UnsafeVal(); \
//    } else {
//
//#define OR_THROW(isolate, errorKind, errMsg) \
//      MaybeLocal<String> mErrMsg =  \
//        v8::String::NewFromUtf8(isolate, (errMsg), v8::String::NewStringType::kNormalString); \
//      isolate->ThrowException(v8::Exception::errorKind(mErrMsg.ToLocalChecked())); \
//      return; \
//    } \
//  } while(0);
//
//  ////////////////////////////////////////////////////////////////////////////////////////////
//
//#define DEFINE_TY_VAL(Type) \
//  bool SafeV8ConvertVal(Isolate* isolate, Local<Value> v, Local<Type>& outVal, Local<Value>& err, bool& hasError);
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
//  bool SafeV8ConvertVal(Isolate* isolate, Local<Value> v, CType& outVal, Local<Value>& err, bool& hasError);
//
//  DEFINE_CTY_VAL(bool, Boolean)
//    DEFINE_CTY_VAL(double, Number)
//    DEFINE_CTY_VAL(uint32_t, Uint32)
//    DEFINE_CTY_VAL(int32_t, Int32)
//#undef DEFINE_CTY_VAL
//
//    //int64 version
//    bool SafeV8ConvertVal(Isolate* isolate, Local<Value> v, int64_t& outVal, Local<Value>& err, bool& hasError);
//
//
//  /* Returns the first argument type of a given lambda */
//
//  template<typename Ret, typename Arg, typename... Rest>
//  Arg first_argument_helper(Ret(*) (Arg, Rest...));
//
//  template<typename Ret, typename F, typename Arg, typename... Rest>
//  Arg first_argument_helper(Ret(F::*) (Arg, Rest...));
//
//  template<typename Ret, typename F, typename Arg, typename... Rest>
//  Arg first_argument_helper(Ret(F::*) (Arg, Rest...) const);
//
//  template <typename F>
//  decltype(first_argument_helper(&F::operator())) first_argument_helper(F);
//
//  template <typename T>
//  using first_argument = decltype(first_argument_helper(std::declval<T>()));
//
//  /* Returns the second argument type of a given lambda */
//
//  template<typename Ret, typename Arg, typename Arg2, typename... Rest>
//  Arg2 second_argument_helper(Ret(*) (Arg, Arg2, Rest...));
//
//  template<typename Ret, typename F, typename Arg, typename Arg2, typename... Rest>
//  Arg2 second_argument_helper(Ret(F::*) (Arg, Arg2, Rest...));
//
//  template<typename Ret, typename F, typename Arg, typename Arg2, typename... Rest>
//  Arg2 second_argument_helper(Ret(F::*) (Arg, Arg2, Rest...) const);
//
//  template <typename F>
//  decltype(second_argument_helper(&F::operator())) second_argument_helper(F);
//
//  template <typename T>
//  using second_argument = decltype(second_argument_helper(std::declval<T>()));
//
//  /* Returns the third argument type of a given lambda */
//
//  template<typename Ret, typename Arg, typename Arg2, typename Arg3, typename... Rest>
//  Arg3 third_argument_helper(Ret(*) (Arg, Arg2, Arg3, Rest...));
//
//  template<typename Ret, typename F, typename Arg, typename Arg2, typename Arg3, typename... Rest>
//  Arg3 third_argument_helper(Ret(F::*) (Arg, Arg2, Arg3, Rest...));
//
//  template<typename Ret, typename F, typename Arg, typename Arg2, typename Arg3, typename... Rest>
//  Arg3 third_argument_helper(Ret(F::*) (Arg, Arg2, Arg3, Rest...) const);
//
//  template <typename F>
//  decltype(third_argument_helper(&F::operator())) third_argument_helper(F);
//
//  template <typename T>
//  using third_argument = decltype(third_argument_helper(std::declval<T>()));
//
//  /* Returns the return argument type of a given lambda */
//
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
//
//  /* Base class from which the various output classes derive from */
//  class SafeV8Promise_GetOutput
//  {
//  protected:
//    Local<Value> err;
//    bool exceptionThrown = false;
//  public:
//    Local<Value> GetException();
//    bool GetIsExceptionThrown();
//  };
//
//  /* Class which handles the single output value case */
//  class SafeV8Promise_GetOutput1 : public SafeV8Promise_GetOutput
//  {
//  private:
//    Isolate* isolate;
//    Local<Value> v1;
//  public:
//    SafeV8Promise_GetOutput1(Isolate* _isolate, Local<Value> _v1);
//
//    //Returns the marshalled and converted values. The lambda provided does not marshal additional values inside
//    template<typename F>
//    V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_same<return_argument<F>, void>::value, SafeV8Promise_GetOutput1>::type ToVal(F func);
//
//    //Returns the marshalled and converted values. The lambda provided does marshal additional values inside
//    template<typename F>
//    V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_base_of<SafeV8Promise_GetOutput, return_argument<F>>::value, SafeV8Promise_GetOutput1>::type ToVal(F func);
//
//    //Handle any errors caught so far. The error handling lambda provided does not marshal additional values inside
//    template<typename F>
//    typename std::enable_if<std::is_same<return_argument<F>, void>::value, void>::type OnErr(F func);
//
//    //Handle any errors caught so far. The error handling lambda provided does marshal additional values inside
//    template<typename F>
//    V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_base_of<SafeV8Promise_GetOutput, return_argument<F>>::value, SafeV8Promise_GetOutput1>::type OnErr(F func);
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
//    SafeV8Promise_GetOutput2(Isolate* _isolate, Local<Value> _v1, Local<Value> _v2);
//
//    //Returns the marshalled and converted values. The lambda provided does not marshal additional values inside
//    template<typename F>
//    V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_same<return_argument<F>, void>::value, SafeV8Promise_GetOutput2>::type ToVal(F func);
//
//    //Returns the marshalled and converted values. The lambda provided does marshal additional values inside
//    template<typename F>
//    V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_base_of<SafeV8Promise_GetOutput, return_argument<F>>::value, SafeV8Promise_GetOutput2>::type ToVal(F func);
//
//    //Handle any errors caught so far. The error handling lambda provided does not marshal additional values inside
//    template<typename F>
//    typename std::enable_if<std::is_same<return_argument<F>, void>::value, void>::type OnErr(F func);
//
//    //Handle any errors caught so far. The error handling lambda provided does marshal additional values inside
//    template<typename F>
//    V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_base_of<SafeV8Promise_GetOutput, return_argument<F>>::value, SafeV8Promise_GetOutput2>::type OnErr(F func);
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
//    SafeV8Promise_GetOutput3(Isolate* _isolate, Local<Value> _v1, Local<Value> _v2, Local<Value> _v3);
//
//    //Returns the marshalled and converted values. The lambda provided does not marshal additional values inside
//    template<typename F>
//    V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_same<return_argument<F>, void>::value, SafeV8Promise_GetOutput3>::type ToVal(F func);
//
//    //Returns the marshalled and converted values. The lambda provided does marshal additional values inside
//    template<typename F>
//    V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_base_of<SafeV8Promise_GetOutput, return_argument<F>>::value, SafeV8Promise_GetOutput3>::type ToVal(F func);
//
//    //Handle any errors caught so far. The error handling lambda provided does not marshal additional values inside
//    template<typename F>
//    typename std::enable_if<std::is_same<return_argument<F>, void>::value, void>::type OnErr(F func);
//
//    //Handle any errors caught so far. The error handling lambda provided does marshal additional values inside
//    template<typename F>
//    V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_base_of<SafeV8Promise_GetOutput, return_argument<F>>::value, SafeV8Promise_GetOutput3>::type OnErr(F func);
//  };
//
//  /* Entry point to users who want to use the SafeV8 api api */
//  V8_WARN_UNUSED_RESULT SafeV8Promise_GetOutput1 With(Isolate* isolate, Local<Value> first);
//
//  V8_WARN_UNUSED_RESULT SafeV8Promise_GetOutput2 With(Isolate* isolate, Local<Value> first, Local<Value> second);
//
//  V8_WARN_UNUSED_RESULT SafeV8Promise_GetOutput3 With(Isolate* isolate, Local<Value> first, Local<Value> second, Local<Value> third);
//
  //////// Get API ////////////

  template<typename ObjectType, typename KeyType>
  bool SafeV8Get(Local<Context> context, ObjectType object, KeyType key, Local<Value>& outVal, Local<Value>& err, bool& hasError);

  template<typename ObjectType, typename KeyType>
  class SafeV8_GetterOutput : public SafeV8Promise_GetOutput
  {
  private:
    Local<Context> context;
    ObjectType object;
    KeyType key;
  public:
    SafeV8_GetterOutput(Local<Context> _context, ObjectType _object, KeyType _key);

    //Returns the marshalled and converted values. The lambda provided does not marshal additional values inside
    template<typename F>
    V8_WARN_UNUSED_RESULT typename std::enable_if<std::is_same<return_argument<F>, void>::value, SafeV8_GetterOutput>::type ToVal(F func);

    template<typename F>
    void OnErr(F func);
  };

  template<typename ObjectType, typename KeyType>
  V8_WARN_UNUSED_RESULT SafeV8_GetterOutput<ObjectType, KeyType> GetField(Local<Context> context, ObjectType object, KeyType key);


  //////// Set API ////////////

  //template<typename ObjectType, typename KeyType>
  //bool SafeV8Set(Local<Context> context, ObjectType object, KeyType key, Local<Value> val, Local<Value>& err, bool& hasError);

  //template<typename ObjectType, typename KeyType>
  //class SafeV8_SetterOutput : public SafeV8Promise_GetOutput
  //{
  //public:
  //  V8_WARN_UNUSED_RESULT SafeV8_SetterOutput(Local<Context> context, ObjectType object, KeyType key, Local<Value> val);

  //  V8_WARN_UNUSED_RESULT SafeV8_SetterOutput(Local<Value> exception);

  //  template<typename F>
  //  void OnErr(F func);
  //};


  //template<typename ObjectType, typename KeyType>
  //V8_WARN_UNUSED_RESULT SafeV8_SetterOutput<ObjectType, KeyType> SetField(Local<Context> context, ObjectType object, KeyType key, Local<Value> val);

  //template<typename ObjectType, typename KeyType, typename GetObjectType, typename GetKeyType>
  //V8_WARN_UNUSED_RESULT SafeV8_SetterOutput<ObjectType, KeyType> SetField(Local<Context> context, ObjectType object, KeyType key, SafeV8_GetterOutput<GetObjectType, GetKeyType> val);




}
