#include "js_stream.h"

#include "async-wrap.h"
#include "env.h"
#include "env-inl.h"
#include "node_buffer.h"
#include "stream_base.h"
#include "stream_base-inl.h"
#include "v8.h"
#include "safe_v8.h"
namespace node {

using v8::Array;
using v8::Context;
using v8::External;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::HandleScope;
using v8::Local;
using v8::Object;
using v8::Value;


JSStream::JSStream(Environment* env, Local<Object> obj, AsyncWrap* parent)
    : AsyncWrap(env, obj, AsyncWrap::PROVIDER_JSSTREAM, parent),
      StreamBase(env) {
  node::Wrap(obj, this);
  MakeWeak<JSStream>(this);
}


JSStream::~JSStream( ) {
}


void* JSStream::Cast( ) {
  return static_cast<void*>(this);
}


AsyncWrap* JSStream::GetAsyncWrap( ) {
  return static_cast<AsyncWrap*>(this);
}


bool JSStream::IsAlive( ) {
  v8::Local<v8::Value> fn = object()->Get(env()->isalive_string());
  if (!fn->IsFunction())
    return false;
  return MakeCallback(fn.As<v8::Function>(), 0, nullptr)->IsTrue();
}


bool JSStream::IsClosing( ) {
  return MakeCallback(env()->isclosing_string(), 0, nullptr)->IsTrue();
}


int JSStream::ReadStart( ) {
  return MakeCallback(env()->onreadstart_string(), 0, nullptr)->Int32Value();
}


int JSStream::ReadStop( ) {
  return MakeCallback(env()->onreadstop_string(), 0, nullptr)->Int32Value();
}


int JSStream::DoShutdown(ShutdownWrap* req_wrap) {
  HandleScope scope(env()->isolate());

  Local<Value> argv[] = {
    req_wrap->object()
  };

  req_wrap->Dispatched();
  Local<Value> res =
      MakeCallback(env()->onshutdown_string(), arraysize(argv), argv);

  return res->Int32Value();
}


int JSStream::DoWrite(WriteWrap* w,
                      uv_buf_t* bufs,
                      size_t count,
                      uv_stream_t* send_handle) {
  CHECK_EQ(send_handle, nullptr);

  HandleScope scope(env()->isolate());

  Local<Array> bufs_arr = Array::New(env()->isolate(), count);
  Local<Object> buf;
  for (size_t i = 0; i < count; i++) {
    buf = Buffer::Copy(env(), bufs[i].base, bufs[i].len).ToLocalChecked();
    bufs_arr->Set(i, buf);
  }

  Local<Value> argv[] = {
    w->object(),
    bufs_arr
  };

  w->Dispatched();
  Local<Value> res =
      MakeCallback(env()->onwrite_string(), arraysize(argv), argv);

  return res->Int32Value();
}


void JSStream::New(const FunctionCallbackInfo<Value>& args) {
  // This constructor should not be exposed to public javascript.
  // Therefore we assert that we are not trying to call this as a
  // normal function.
  v8::Isolate* isolate = Environment::GetCurrent(args)->isolate();
  if(!(args.IsConstructCall())) {
    return Environment::GetCurrent(args)->ThrowTypeError("Failed CHECK(args.IsConstructCall());");
  }
  Environment* env = Environment::GetCurrent(args);
  JSStream* wrap;

  if (args.Length() == 0) {
    wrap = new JSStream(env, args.This(), nullptr);
  } else if (args[0]->IsExternal()) {
    void* ptr = args[0].As<External>()->Value();
    wrap = new JSStream(env, args.This(), static_cast<AsyncWrap*>(ptr));
  } else {
    UNREACHABLE();
  }
  if(!(wrap)) {
    return Environment::GetCurrent(args)->ThrowTypeError("Failed CHECK(wrap);");
  }
}


static void FreeCallback(char* data, void* hint) {
  // Intentional no-op
}


void JSStream::DoAlloc(const FunctionCallbackInfo<Value>& args) {
  JSStream* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap, args.Holder());

  uv_buf_t buf;
  wrap->OnAlloc(args[0]->Int32Value(), &buf);
  Local<Object> vbuf = Buffer::New(
      wrap->env(),
      buf.base,
      buf.len,
      FreeCallback,
      nullptr).ToLocalChecked();
  return args.GetReturnValue().Set(vbuf);
}


void JSStream::DoRead(const FunctionCallbackInfo<Value>& args) {
  JSStream* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap, args.Holder());

  CHECK(Buffer::HasInstance(args[1]));
  uv_buf_t buf = uv_buf_init(Buffer::Data(args[1]), Buffer::Length(args[1]));
  wrap->OnRead(args[0]->Int32Value(), &buf);
}


void JSStream::DoAfterWrite(const FunctionCallbackInfo<Value>& args) {
  v8::Isolate* isolate = Environment::GetCurrent(args)->isolate();
  JSStream* wrap;
  
  safeV8::With(isolate, args[0])
  .OnVal([&](Local<Object> args0) -> safeV8::SafeV8Promise_Base {
  WriteWrap* w;
  ASSIGN_OR_RETURN_UNWRAP(&wrap, args.Holder(), safeV8::Done);
  ASSIGN_OR_RETURN_UNWRAP(&w, args0, safeV8::Done);

  wrap->OnAfterWrite(w);
return safeV8::Done;
})
  .OnErr([&isolate](Local<Value> exception){
    isolate->ThrowException(exception);
  });
}


template <class Wrap>
void JSStream::Finish(const FunctionCallbackInfo<Value>& args) {
  v8::Isolate* isolate = Environment::GetCurrent(args)->isolate();
  Wrap* w;
  
  safeV8::With(isolate, args[0])
  .OnVal([&](Local<Object> args0) -> safeV8::SafeV8Promise_Base {
  ASSIGN_OR_RETURN_UNWRAP(&w, args0, safeV8::Done);

  w->Done(args[1]->Int32Value());
return safeV8::Done;
})
  .OnErr([&isolate](Local<Value> exception){
    isolate->ThrowException(exception);
  });
}


void JSStream::ReadBuffer(const FunctionCallbackInfo<Value>& args) {
  JSStream* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap, args.Holder());

  CHECK(Buffer::HasInstance(args[0]));
  char* data = Buffer::Data(args[0]);
  int len = Buffer::Length(args[0]);

  do {
    uv_buf_t buf;
    ssize_t avail = len;
    wrap->OnAlloc(len, &buf);
    if (static_cast<ssize_t>(buf.len) < avail)
      avail = buf.len;

    memcpy(buf.base, data, avail);
    data += avail;
    len -= avail;
    wrap->OnRead(avail, &buf);
  } while (len != 0);
}


void JSStream::EmitEOF(const FunctionCallbackInfo<Value>& args) {
  JSStream* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap, args.Holder());

  wrap->OnRead(UV_EOF, nullptr);
}


void JSStream::Initialize(Local<Object> target,
                          Local<Value> unused,
                          Local<Context> context) {
  Environment* env = Environment::GetCurrent(context);

  Local<FunctionTemplate> t = env->NewFunctionTemplate(New);
  t->SetClassName(FIXED_ONE_BYTE_STRING(env->isolate(), "JSStream"));
  t->InstanceTemplate()->SetInternalFieldCount(1);

  env->SetProtoMethod(t, "doAlloc", DoAlloc);
  env->SetProtoMethod(t, "doRead", DoRead);
  env->SetProtoMethod(t, "doAfterWrite", DoAfterWrite);
  env->SetProtoMethod(t, "finishWrite", Finish<WriteWrap>);
  env->SetProtoMethod(t, "finishShutdown", Finish<ShutdownWrap>);
  env->SetProtoMethod(t, "readBuffer", ReadBuffer);
  env->SetProtoMethod(t, "emitEOF", EmitEOF);

  StreamBase::AddMethods<JSStream>(env, t, StreamBase::kFlagHasWritev);
  target->Set(FIXED_ONE_BYTE_STRING(env->isolate(), "JSStream"),
              t->GetFunction());
  env->set_jsstream_constructor_template(t);
}

}  // namespace node

NODE_MODULE_CONTEXT_AWARE_BUILTIN(js_stream, node::JSStream::Initialize)
