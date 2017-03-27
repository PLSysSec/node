The safe API
-----------------------------------
With the Safe V8 API, we aim to to build a wrapper around V8 API such that

1. Users of the API are required to handle failures (eg: exception-throwing upcalls) in a crash-safe way, by propagating errors back to JavaScript instead of hard crashing.
2. Disallow developers from using JavaScript values before checking their types.
3. Make explicit which C++ functions can trigger JavaScript upcalls that may change data that you are working with (these are marked Unsafe)

The below examples give a practical illustration of how to use these APIs. For additional technical details, please refer to the paper submitted at the IEEE Symposium on Security and Privacy, Oakland17 - "Finding and Preventing Bugs in JavaScript Bindings"

The motivation
---------------

Consider a simple example, where you are using the V8 API to convert a JavaScript object to a Number, you may use

```
//Buggy snippet 1
void convertToNum(const FunctionCallbackInfo<Value>& args)
{
  Local<Number> ret = args[0].As<Number>();
  args.GetReturnValue().Set(ret);
}
```

or something like

```
//Buggy snippet 2
void convertToNum(const FunctionCallbackInfo<Value>& args)
{
  CHECK(args[0]->IsNumber());
  Local<Number> ret = args[0].As<Number>();
  args.GetReturnValue().Set(ret);
}
```

Both these options are bad. Snippet 1 does no checking before casting argument as a number. Snippet 2 checks that the argument is a number, but crashes if the check fails. This means that a when this function is operating on an ***input from JavaScript***, it could ***CRASH***, or worse lead to the problems such ***Buffer overflows, Use after frees*** etc. - problems that should not appear in JavaScript. 

You could correctly write the code as follows

```
void convertToNum(const FunctionCallbackInfo<Value>& args)
{
  Environment* env = Environment::GetCurrent(args);
  Isolate* isolate = env->isolate();

  if (args[0]->IsNumber()) {
    Local<Number> ret = args[0].As<Number>();
    args.GetReturnValue().Set(ret);
  }
  else {
    MaybeLocal<String> mErrMsg = v8::String::NewFromUtf8(isolate, "Invalid type", v8::String::NewStringType::kNormalString);
    Local<Value> err = v8::Exception::TypeError(mErrMsg.ToLocalChecked());
    isolate->ThrowException(err);
  }
}
```

But this sort of careful checking, when optional is easy to accidentally omit.

Compare this to how you could write this with the SafeV8 API.

```
void convertToNumSafe(const FunctionCallbackInfo<Value>& args)
{
  Environment* env = Environment::GetCurrent(args);
  Isolate* isolate = env->isolate();

  return safeV8::marshal(isolate, args[0])
  .onVal([&](Local<Number> ret) {
    args.GetReturnValue().Set(ret);
  })
  .onFail([&](Local<Value> exception) {
    isolate->ThrowException(safeV8::v8Err(isolate, "Invalid type", v8::Exception::TypeError));
  });
}

```

At first glance this may look similar, albeit using lambdas. But this API provides the following advantages
1. You cannot accidentally skip the checking prior to the cast (like Snippet 1)
2. A failing check will not crash the program (like Snippet 2)
3. For the most, you can not omit the error handling part of the code - this will result in a compilation error (or at least a warning)

Full List of APIs
-----------------

1. marshal API (i.e. if a JS object points to an array, attempt to convert the type to an array)
2. getProp API (i.e. get a particular field from a json object)
3. hasProp API (i.e. check for a particular field from a json object)
4. delProp API (i.e. delete a particular field from a json object)
5. hasOwnProp API (i.e. check for a particular field from a json object, without going up the inheritence tree)
6. getOwnPropDescriptor API
7. setProp API (i.e. set a particular field from a json object)
8. toString API (i.e. stringify object)
9. getPropNames API (i.e. get the list properties on this json object)
10. getOwnPropNames API (i.e. get the list properties on this json object, without going up the inheritence tree)
11. implicitCoerce (i.e. Unsafe casting APIs - such as converting to bool, double, uint32_t, int32_t)

Type Conversion examples
------------------------

Let's look at a bunch of snippets on how to convert API calls from to V8 APIs to starting with a similar example as before. Let's convert 2 JavaScript objects reference to a Number reference and an Array reference

```
void convertTypes(const FunctionCallbackInfo<Value>& args)
{
  Environment* env = Environment::GetCurrent(args);
  Isolate* isolate = env->isolate();

  if (args[0]->IsNumber() && args[1]->IsArray()) {
    Local<Number> ret = args[0].As<Number>();
    Local<Array> ret2 = args[1].As<Array>();
    //... some computation
    args.GetReturnValue().Set(ret);
  }
  else {
    MaybeLocal<String> mErrMsg = v8::String::NewFromUtf8(isolate, "Invalid type", v8::String::NewStringType::kNormalString);
    Local<Value> err = v8::Exception::TypeError(mErrMsg.ToLocalChecked());
    isolate->ThrowException(err);
  }
}

void convertTypesSafe(const FunctionCallbackInfo<Value>& args)
{
  Environment* env = Environment::GetCurrent(args);
  Isolate* isolate = env->isolate();

  return safeV8::marshal(isolate, args[0], args[1])
  .onVal([&](Local<Number> ret, Local<Array> ret2) {
    //... some computation
    args.GetReturnValue().Set(ret);
  })
  .onFail([&](Local<Value> exception) {
    isolate->ThrowException(safeV8::v8Err(isolate, "Invalid type", v8::Exception::TypeError));
  });
}
```

Similarly, just as we converted JavaScript object references to a Number references and Array references, in Node.js the following conversions are also allowed 
* Array
* ArrayBuffer
* ArrayBufferView
* Boolean
* DataView
* Date
* External
* Float32Array
* Float64Array
* Function
* Int16Array
* Int32
* Int32Array
* Int8Array
* Map
* Name
* Number
* Object
* Proxy
* RegExp
* Set
* SharedArrayBuffer
* String
* StringObject
* Symbol
* TypedArray
* Uint16Array
* Uint32
* Uint32Array
* Uint8Array
* Uint8ClampedArray

We can also check a particular argument for multiple types, as in the below code

```
void convertOverloaded(const FunctionCallbackInfo<Value>& args)
{
  Environment* env = Environment::GetCurrent(args);
  Isolate* isolate = env->isolate();

  if (args[0]->IsArray()) {
    Local<Array> ret = args[0].As<Array>();
    args.GetReturnValue().Set(ret);
  }
  else if (args[0]->IsNumber()) {
    Local<Number> ret = args[0].As<Number>();
    args.GetReturnValue().Set(ret);
  }
  else {
    MaybeLocal<String> mErrMsg = v8::String::NewFromUtf8(isolate, "Invalid type", v8::String::NewStringType::kNormalString);
    Local<Value> err = v8::Exception::TypeError(mErrMsg.ToLocalChecked());
    isolate->ThrowException(err);
  }
}

void convertOverloadedSafe(const FunctionCallbackInfo<Value>& args)
{
  Environment* env = Environment::GetCurrent(args);
  Isolate* isolate = env->isolate();

  return safeV8::marshal(isolate, args[0])
  .onVal([&](Local<Array> ret) {
    args.GetReturnValue().Set(ret);
  })
  .onVal([&](Local<Number> ret) {
    args.GetReturnValue().Set(ret);
  })
  .onFail([&](Local<Value> exception) {
    isolate->ThrowException(safeV8::v8Err(isolate, "Invalid type", v8::Exception::TypeError));
  });
}
```

Implicit Cast example (BooleanValue, Uint32Value etc.)
------------------------------------------------------

The list of implicit casts exist for the following 4 types

| C++ Type      | JavaScript Type |
| ------------- |:---------------:|
| bool          | Boolean         |
| double        | Number          |
| uint32_t      | Uint32          |
| int32_t       | Int32           |

A simple snippet showing how to cast this correctly

```
void implicitCast(const FunctionCallbackInfo<Value>& args)
{
    uint32_t value = args[0]->Uint32Value();
    someFnCall(value);
}

void implicitCastSafe(const FunctionCallbackInfo<Value>& args)
{
  return safeV8::implicitCoerce(isolate, args[0])
  .onVal([&](uint32_t value) {
    someFnCall(value);
  })
  .onFail([&](Local<Value> exception) {
    isolate->ThrowException(safeV8::v8Err(isolate, "Invalid type", v8::Exception::TypeError));
  });
}
```

GetProp example
----------------

A property from a JSON object can be retrieved using the Get API. The property could be a JavaScript string or an integer. For example

```
void GetFirstProp(const FunctionCallbackInfo<Value>& args)
{
  Environment* env = Environment::GetCurrent(args);
  Isolate* isolate = env->isolate();

  Local<Object> a1 = args[0].As<Object>();
  MaybeLocal<Value> retM = a1->Get(isolate->GetCurrentContext(), 0);
  Local<Value> ret;

  if (retM.ToLocal(&ret)) {
    args.GetReturnValue().Set(ret);
  }
  else {
    MaybeLocal<String> mErrMsg = v8::String::NewFromUtf8(isolate, "Invalid type", v8::String::NewStringType::kNormalString);
    Local<Value> err = v8::Exception::TypeError(mErrMsg.ToLocalChecked());
    isolate->ThrowException(err);
  }
}

void GetFirstPropSafe(const FunctionCallbackInfo<Value>& args)
{
  Environment* env = Environment::GetCurrent(args);
  Isolate* isolate = env->isolate();

  Local<Object> a1 = args[0].As<Object>();

  return safeV8::getProp(isolate, a1, 0)
  .onVal([&](Local<Value> ret) {
    args.GetReturnValue().Set(ret);
  })
  .onFail([&](Local<Value> exception) {
    isolate->ThrowException(safeV8::v8Err(isolate, "Invalid type", v8::Exception::TypeError));
  });
}
```

SetProp example
----------------

Let us now look at an example for setting a property
```
void SetProp(const FunctionCallbackInfo<Value>& args)
{
  Environment* env = Environment::GetCurrent(args);
  Isolate* isolate = env->isolate();

  //get the name and value
  if(!binding_object->Set(env->context(), name, value).FromJust())
  {
    MaybeLocal<String> mErrMsg = v8::String::NewFromUtf8(isolate, "Invalid type", v8::String::NewStringType::kNormalString);
    Local<Value> err = v8::Exception::TypeError(mErrMsg.ToLocalChecked());
    isolate->ThrowException(err);
  }
}

void SetPropSafe(const FunctionCallbackInfo<Value>& args)
{
  Environment* env = Environment::GetCurrent(args);
  Isolate* isolate = env->isolate();

  //get the name and value

  return safeV8::setProp(isolate, binding_object, name, value)
  .onVal([]() {
    args.GetReturnValue().Set(ret);
  })
  .onFail([&](Local<Value> exception) {
    isolate->ThrowException(safeV8::v8Err(isolate, "Invalid type", v8::Exception::TypeError));
  });
}
```

Note: For convenience there is a shortcut for when you have to call Get followed by a call to Set. The error handling below is as you would expect, the error handler catches errors that occur during the Set or the Get.

```
void GetAndThenSetProp(const FunctionCallbackInfo<Value>& args)
{
  Environment* env = Environment::GetCurrent(args);
  Isolate* isolate = env->isolate();

  Local<Object> a1 = args[0].As<Object>();
  MaybeLocal<Value> retM = a1->Get(isolate->GetCurrentContext(), 0);
  Local<Value> value;

  //get the name and value
  if(!retM.ToLocal(&value) || !binding_object->Set(env->context(), name, value).FromJust())
  {
    MaybeLocal<String> mErrMsg = v8::String::NewFromUtf8(isolate, "Invalid type", v8::String::NewStringType::kNormalString);
    Local<Value> err = v8::Exception::TypeError(mErrMsg.ToLocalChecked());
    isolate->ThrowException(err);
  }
}

void GetAndThenSetPropSafe(const FunctionCallbackInfo<Value>& args)
{
  Environment* env = Environment::GetCurrent(args);
  Isolate* isolate = env->isolate();

  //get the name and value

  return safeV8::setProp(isolate, binding_object, name, safeV8::getProp(isolate, a1, 0))
  .onVal([]() {
    args.GetReturnValue().Set(ret);
  })
  .onFail([&](Local<Value> exception) {
    isolate->ThrowException(safeV8::v8Err(isolate, "Invalid type", v8::Exception::TypeError));
  });
}
```

toString example
----------------

```
  void ToString(const FunctionCallbackInfo<Value>& args)
  {
    Local<String> str_obj = args[0]->ToString(env->isolate());
    foo(str_obj);
    args.GetReturnValue().Set(str_obj);
  }

  void ToStringSafe(const FunctionCallbackInfo<Value>& args)
  {
    return safeV8::toString(isolate, args[0])
    .onVal([&](Local<String> str_obj) {
      foo(str_obj);
      args.GetReturnValue().Set(str_obj);
    })
    .onFail([&](Local<Value> exception) {
      isolate->ThrowException(safeV8::v8Err(isolate, "Invalid type", v8::Exception::TypeError));
    });
  }
```

Error handling succinctly
------------------------

To avoid handling errors when calling the api multiple times, consider the following example. Notice in the below example, the safe version of the code has an inner lambda that returns some data instead of the void as in previous examples.

Also notice, that the second API call (the call to Set) does not have an error handler. This is OK because, by calling Set and returning to the parent safeV8 call (the call to With), the error handling for the call to Set is deferred to the error handling for the call to With (i.e. any error in calling With or Set would call the only error handler in the converted code).

```
void multipleApisTest(const FunctionCallbackInfo<Value>& args)
{
  Environment* env = Environment::GetCurrent(args);
  Isolate* isolate = env->isolate();

  if (args[0]->IsString() && args[1]->IsArray()) {
    Local<String> ret = args[0].As<String>();
    Local<Array> ret2 = args[1].As<Array>();
    
    MaybeLocal<Value> retM = ret2->Get(isolate->GetCurrentContext(), ret);
    Local<Value> ret;
  
    if (retM.ToLocal(&ret)) {
      return args.GetReturnValue().Set(ret);
    }
  }

  MaybeLocal<String> mErrMsg = v8::String::NewFromUtf8(isolate, "Invalid type", v8::String::NewStringType::kNormalString);
  Local<Value> err = v8::Exception::TypeError(mErrMsg.ToLocalChecked());
  isolate->ThrowException(err);
}

void multipleApisTestSafe(const FunctionCallbackInfo<Value>& args)
{
  Environment* env = Environment::GetCurrent(args);
  Isolate* isolate = env->isolate();

  return safeV8::marshal(isolate, args[0], args[1])
  .onVal([&](Local<String> ret, Local<Array> ret2) -> safeV8::SafeV8Promise_Base {

    return safeV8::getProp(isolate, ret2, ret)
    .onVal([]() {
      args.GetReturnValue().Set(ret);
    });
  })
  .onFail([&](Local<Value> exception) {
    isolate->ThrowException(safeV8::v8Err(isolate, "Invalid type", v8::Exception::TypeError));
  });
}

```

What if we call the Safe APIs while handling errors
----------------------------------------------------

Consider an example where we have converted code that looks like the following

```
void safeApiinFailCase(const FunctionCallbackInfo<Value>& args)
{
  Environment* env = Environment::GetCurrent(args);
  Isolate* isolate = env->isolate();

  return safeV8::marshal(isolate, args[0], args[1])
  .onVal([&](Local<String> ret, Local<Array> ret2) {
    //...    
  })
  .onFail([&](Local<Value> exception) {
    //... here
  });
}
```

Now suppose inside the lambda of the onFail case, we make a call to safeV8:: apis (next to 'here'), then this call needs an associated error handler. Here there are 2 options

Option 1 - add an additional top level error handler

```
void safeApiinFailCaseOption1(const FunctionCallbackInfo<Value>& args)
{
  Environment* env = Environment::GetCurrent(args);
  Isolate* isolate = env->isolate();

  return safeV8::marshal(isolate, args[0])
  .onVal([&](Local<String> ret) {
    //...    
  })
  .onFail([&](Local<Value> exception) {

    return safeV8::marshal(isolate, args[1])
    .onVal([&](Local<Array> ret2) {
      //...
    });

  })
  .onFail([&](Local<Value> exception) {
      isolate->ThrowException(safeV8::v8Err(isolate, "Invalid type", v8::Exception::TypeError));
  });
}
```

Option 2 - add an local error handler

```
void safeApiinFailCaseOption2(const FunctionCallbackInfo<Value>& args)
{
  Environment* env = Environment::GetCurrent(args);
  Isolate* isolate = env->isolate();

  return safeV8::marshal(isolate, args[0])
  .onVal([&](Local<String> ret) {
    //...    
  })
  .onFail([&](Local<Value> exception) {

    return safeV8::marshal(isolate, args[1])
    .onVal([&](Local<Array> ret2) {
      //...
    })
    .onFail([&](Local<Value> exception) {
      isolate->ThrowException(safeV8::v8Err(isolate, "Invalid type", v8::Exception::TypeError));
    });

  });
}
```

An example with Control Flows(aka if conditions, for loops etc.)
----------------------------------------------------------------

As you can see, sometimes you have to add some temporary variables namely 'done' to make this conversion work

```
void complexControlFlow(const FunctionCallbackInfo<Value>& args)
{

  if (args[0]->IsNumber()) {

    Local<Number> first = args[0].As<Number>();

    for(unsigned int i = 1; i < 10; i++) {
      if (args[i]->IsNumber()) {
        Local<Number> ret = args[i].As<Number>();
        if(someComputation(first, ret))
        {
          return args.GetReturnValue().Set(ret);        
        }
      }
      else {
        MaybeLocal<String> mErrMsg = v8::String::NewFromUtf8(isolate, "Invalid type", v8::String::NewStringType::kNormalString);
        Local<Value> err = v8::Exception::TypeError(mErrMsg.ToLocalChecked());
        isolate->ThrowException(err);
      }
    }
  }
}


void complexControlFlowSafe(const FunctionCallbackInfo<Value>& args)
{
  Environment* env = Environment::GetCurrent(args);
  Isolate* isolate = env->isolate();

  safeV8::marshal(isolate, args[0])
  .onVal([&](Local<Number> first) {

    bool errFound = false;
    Local<Value> exceptionValue;

    for(unsigned int i = 1; i < 10; i++) {
  
      safeV8::marshal(isolate, args[i])
      .onVal([&](Local<Number> ret) {
        if(someComputation(first,ret))
        {
          args.GetReturnValue().Set(ret);
        }
      })
      .onFail([&](Local<Value> exception) {
        exceptionFound = safeV8::safeV8Err(isolate, "Invalid type", v8::Exception::TypeError);
        errFound = true;
      });
  
      if(errFound)
      {
        return exceptionFound;
      }
    }

    return safeV8::Done;
  })
  .onFail([&](Local<Value> exception) {
    isolate->ThrowException(safeV8::v8Err(isolate, "Invalid type", v8::Exception::TypeError));
  });
}

```

Full Example
------------

See src/node_buffer.cc and src/node_http_parser.cc for more actual usage examples