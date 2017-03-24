#include "node.h"
#include "v8.h"
#include "env.h"
#include "env-inl.h"
#include "string_bytes.h"
#include "safe_v8.h"

#include <errno.h>
#include <string.h>

#ifdef __MINGW32__
# include <io.h>
#endif  // __MINGW32__

#ifdef __POSIX__
# include <limits.h>        // PATH_MAX on Solaris.
# include <netdb.h>         // MAXHOSTNAMELEN on Solaris.
# include <unistd.h>        // gethostname, sysconf
# include <sys/param.h>     // MAXHOSTNAMELEN on Linux and the BSDs.
# include <sys/utsname.h>
#endif  // __POSIX__

// Add Windows fallback.
#ifndef MAXHOSTNAMELEN
# define MAXHOSTNAMELEN 256
#endif  // MAXHOSTNAMELEN

namespace node {
namespace os {

using v8::Array;
using v8::Boolean;
using v8::Context;
using v8::FunctionCallbackInfo;
using v8::Integer;
using v8::Local;
using v8::Null;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Value;


static void GetHostname(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  char buf[MAXHOSTNAMELEN + 1];

  if (gethostname(buf, sizeof(buf))) {
#ifdef __POSIX__
    int errorno = errno;
#else  // __MINGW32__
    int errorno = WSAGetLastError();
#endif  // __POSIX__
    return env->ThrowErrnoException(errorno, "gethostname");
  }
  buf[sizeof(buf) - 1] = '\0';

  args.GetReturnValue().Set(OneByteString(env->isolate(), buf));
}


static void GetOSType(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  const char* rval;

#ifdef __POSIX__
  struct utsname info;
  if (uname(&info) < 0) {
    return env->ThrowErrnoException(errno, "uname");
  }
  rval = info.sysname;
#else  // __MINGW32__
  rval ="Windows_NT";
#endif  // __POSIX__

  args.GetReturnValue().Set(OneByteString(env->isolate(), rval));
}


static void GetOSRelease(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  const char* rval;

#ifdef __POSIX__
  struct utsname info;
  if (uname(&info) < 0) {
    return env->ThrowErrnoException(errno, "uname");
  }
  rval = info.release;
#else  // Windows
  char release[256];
  OSVERSIONINFOW info;

  info.dwOSVersionInfoSize = sizeof(info);

  // Don't complain that GetVersionEx is deprecated; there is no alternative.
  #pragma warning(suppress : 4996)
  if (GetVersionExW(&info) == 0)
    return;

  snprintf(release,
           sizeof(release),
           "%d.%d.%d",
           static_cast<int>(info.dwMajorVersion),
           static_cast<int>(info.dwMinorVersion),
           static_cast<int>(info.dwBuildNumber));
  rval = release;
#endif  // __POSIX__

  args.GetReturnValue().Set(OneByteString(env->isolate(), rval));
}


static void GetCPUInfo(const FunctionCallbackInfo<Value>& args) {
  v8::Isolate* isolate = Environment::GetCurrent(args)->isolate();
  Environment* env = Environment::GetCurrent(args);
  uv_cpu_info_t* cpu_infos;
  int count, i;

  int err = uv_cpu_info(&cpu_infos, &count);
  if (err)
    return;

  Local<Array> cpus = Array::New(env->isolate());
  for (i = 0; i < count; i++) {
    uv_cpu_info_t* ci = cpu_infos + i;

    Local<Object> times_info = Object::New(env->isolate());
      safeV8::Set(isolate, times_info,env->user_string(),Number::New(env->isolate(),ci->cpu_times.user))
  .OnVal([&]()-> safeV8::SafeV8Promise_Base {

      {
    bool safeV8_Failed8 = false;
    Local<Value> safeV8_exceptionThrown8;
safeV8::Set(isolate, times_info,env->nice_string(),Number::New(env->isolate(),ci->cpu_times.nice))
  .OnVal([&]()-> safeV8::SafeV8Promise_Base {

      {
    bool safeV8_Failed7 = false;
    Local<Value> safeV8_exceptionThrown7;
safeV8::Set(isolate, times_info,env->sys_string(),Number::New(env->isolate(),ci->cpu_times.sys))
  .OnVal([&]()-> safeV8::SafeV8Promise_Base {

      {
    bool safeV8_Failed6 = false;
    Local<Value> safeV8_exceptionThrown6;
safeV8::Set(isolate, times_info,env->idle_string(),Number::New(env->isolate(),ci->cpu_times.idle))
  .OnVal([&]()-> safeV8::SafeV8Promise_Base {

      {
    bool safeV8_Failed5 = false;
    Local<Value> safeV8_exceptionThrown5;
safeV8::Set(isolate, times_info,env->irq_string(),Number::New(env->isolate(),ci->cpu_times.irq))
  .OnVal([&]()-> safeV8::SafeV8Promise_Base {


    Local<Object> cpu_info = Object::New(env->isolate());
      {
    bool safeV8_Failed4 = false;
    Local<Value> safeV8_exceptionThrown4;
safeV8::Set(isolate, cpu_info,env->model_string(),OneByteString(env->isolate(),ci->model))
  .OnVal([&]()-> safeV8::SafeV8Promise_Base {

      {
    bool safeV8_Failed3 = false;
    Local<Value> safeV8_exceptionThrown3;
safeV8::Set(isolate, cpu_info,env->speed_string(),Number::New(env->isolate(),ci->speed))
  .OnVal([&]()-> safeV8::SafeV8Promise_Base {

      {
    bool safeV8_Failed2 = false;
    Local<Value> safeV8_exceptionThrown2;
safeV8::Set(isolate, cpu_info,env->times_string(),times_info)
  .OnVal([&]()-> safeV8::SafeV8Promise_Base {


      {
    bool safeV8_Failed1 = false;
    Local<Value> safeV8_exceptionThrown1;
safeV8::Set(isolate, (*cpus),i,cpu_info)
  .OnVal([&]()-> safeV8::SafeV8Promise_Base {
    return safeV8::Done;
  })
    .OnErr([&](Local<Value> exception){ safeV8_Failed1 = true; safeV8_exceptionThrown1 = exception; });
    if(safeV8_Failed1) return safeV8::Err(safeV8_exceptionThrown1);

  
}
return safeV8::Done;
})
    .OnErr([&](Local<Value> exception){ safeV8_Failed2 = true; safeV8_exceptionThrown2 = exception; });
    if(safeV8_Failed2) return safeV8::Err(safeV8_exceptionThrown2);

  
}
return safeV8::Done;
})
    .OnErr([&](Local<Value> exception){ safeV8_Failed3 = true; safeV8_exceptionThrown3 = exception; });
    if(safeV8_Failed3) return safeV8::Err(safeV8_exceptionThrown3);

  
}
return safeV8::Done;
})
    .OnErr([&](Local<Value> exception){ safeV8_Failed4 = true; safeV8_exceptionThrown4 = exception; });
    if(safeV8_Failed4) return safeV8::Err(safeV8_exceptionThrown4);

  
}
return safeV8::Done;
})
    .OnErr([&](Local<Value> exception){ safeV8_Failed5 = true; safeV8_exceptionThrown5 = exception; });
    if(safeV8_Failed5) return safeV8::Err(safeV8_exceptionThrown5);

  
}
return safeV8::Done;
})
    .OnErr([&](Local<Value> exception){ safeV8_Failed6 = true; safeV8_exceptionThrown6 = exception; });
    if(safeV8_Failed6) return safeV8::Err(safeV8_exceptionThrown6);

  
}
return safeV8::Done;
})
    .OnErr([&](Local<Value> exception){ safeV8_Failed7 = true; safeV8_exceptionThrown7 = exception; });
    if(safeV8_Failed7) return safeV8::Err(safeV8_exceptionThrown7);

  
}
return safeV8::Done;
})
    .OnErr([&](Local<Value> exception){ safeV8_Failed8 = true; safeV8_exceptionThrown8 = exception; });
    if(safeV8_Failed8) return safeV8::Err(safeV8_exceptionThrown8);

  
}
return safeV8::Done;
})
  .OnErr([&isolate](Local<Value> exception){
    isolate->ThrowException(exception);
  });
}

  uv_free_cpu_info(cpu_infos, count);
  args.GetReturnValue().Set(cpus);
}


static void GetFreeMemory(const FunctionCallbackInfo<Value>& args) {
  double amount = uv_get_free_memory();
  if (amount < 0)
    return;
  args.GetReturnValue().Set(amount);
}


static void GetTotalMemory(const FunctionCallbackInfo<Value>& args) {
  double amount = uv_get_total_memory();
  if (amount < 0)
    return;
  args.GetReturnValue().Set(amount);
}


static void GetUptime(const FunctionCallbackInfo<Value>& args) {
  double uptime;
  int err = uv_uptime(&uptime);
  if (err == 0)
    args.GetReturnValue().Set(uptime);
}


static void GetLoadAvg(const FunctionCallbackInfo<Value>& args) {
  v8::Isolate* isolate = Environment::GetCurrent(args)->isolate();
  Environment* env = Environment::GetCurrent(args);
  double loadavg[3];
  uv_loadavg(loadavg);
  Local<Array> loads = Array::New(env->isolate(), 3);
    safeV8::Set(isolate, loads,0,Number::New(env->isolate(),loadavg[0]))
  .OnVal([&]()-> safeV8::SafeV8Promise_Base {

    {
    bool safeV8_Failed2 = false;
    Local<Value> safeV8_exceptionThrown2;
safeV8::Set(isolate, loads,1,Number::New(env->isolate(),loadavg[1]))
  .OnVal([&]()-> safeV8::SafeV8Promise_Base {

    {
    bool safeV8_Failed1 = false;
    Local<Value> safeV8_exceptionThrown1;
safeV8::Set(isolate, loads,2,Number::New(env->isolate(),loadavg[2]))
  .OnVal([&]()-> safeV8::SafeV8Promise_Base {

  args.GetReturnValue().Set(loads);

  return safeV8::Done;
})
    .OnErr([&](Local<Value> exception){ safeV8_Failed1 = true; safeV8_exceptionThrown1 = exception; });
    if(safeV8_Failed1) return safeV8::Err(safeV8_exceptionThrown1);

  
}
return safeV8::Done;
})
    .OnErr([&](Local<Value> exception){ safeV8_Failed2 = true; safeV8_exceptionThrown2 = exception; });
    if(safeV8_Failed2) return safeV8::Err(safeV8_exceptionThrown2);

  
}
return safeV8::Done;
})
  .OnErr([&isolate](Local<Value> exception){
    isolate->ThrowException(exception);
  });
}


static void GetInterfaceAddresses(const FunctionCallbackInfo<Value>& args) {
  v8::Isolate* isolate = Environment::GetCurrent(args)->isolate();
  Environment* env = Environment::GetCurrent(args);
  uv_interface_address_t* interfaces;
  int count, i;
  char ip[INET6_ADDRSTRLEN];
  char netmask[INET6_ADDRSTRLEN];
  char mac[18];
  Local<Object> ret, o;
  Local<String> name, family;
  Local<Array> ifarr;

  int err = uv_interface_addresses(&interfaces, &count);

  ret = Object::New(env->isolate());

  if (err == UV_ENOSYS) {
    return args.GetReturnValue().Set(ret);
  } else if (err) {
    return env->ThrowUVException(err, "uv_interface_addresses");
  }

  for (i = 0; i < count; i++) {
    const char* const raw_name = interfaces[i].name;

    // On Windows, the interface name is the UTF8-encoded friendly name and may
    // contain non-ASCII characters.  On UNIX, it's just a binary string with
    // no particular encoding but we treat it as a one-byte Latin-1 string.
#ifdef _WIN32
    name = String::NewFromUtf8(env->isolate(), raw_name);
#else
    name = OneByteString(env->isolate(), raw_name);
#endif

    if (ret->Has(env->context(), name).FromJust()) {
        safeV8::Get(isolate, ret,name)
  .OnVal([&](Local<Value> ret_name)-> safeV8::SafeV8Promise_Base {
ifarr = Local<Array>::Cast(ret_name);
    
  return safeV8::Done;
})
  .OnErr([&isolate](Local<Value> exception){
    isolate->ThrowException(exception);
  });
} else {
      ifarr = Array::New(env->isolate());
        safeV8::Set(isolate, ret,name,ifarr)
  .OnVal([&]()-> safeV8::SafeV8Promise_Base {
    return safeV8::Done;
  })
  .OnErr([&isolate](Local<Value> exception){
    isolate->ThrowException(exception);
  });
}

    snprintf(mac,
             18,
             "%02x:%02x:%02x:%02x:%02x:%02x",
             static_cast<unsigned char>(interfaces[i].phys_addr[0]),
             static_cast<unsigned char>(interfaces[i].phys_addr[1]),
             static_cast<unsigned char>(interfaces[i].phys_addr[2]),
             static_cast<unsigned char>(interfaces[i].phys_addr[3]),
             static_cast<unsigned char>(interfaces[i].phys_addr[4]),
             static_cast<unsigned char>(interfaces[i].phys_addr[5]));

    if (interfaces[i].address.address4.sin_family == AF_INET) {
      uv_ip4_name(&interfaces[i].address.address4, ip, sizeof(ip));
      uv_ip4_name(&interfaces[i].netmask.netmask4, netmask, sizeof(netmask));
      family = env->ipv4_string();
    } else if (interfaces[i].address.address4.sin_family == AF_INET6) {
      uv_ip6_name(&interfaces[i].address.address6, ip, sizeof(ip));
      uv_ip6_name(&interfaces[i].netmask.netmask6, netmask, sizeof(netmask));
      family = env->ipv6_string();
    } else {
      strncpy(ip, "<unknown sa family>", INET6_ADDRSTRLEN);
      family = env->unknown_string();
    }

    o = Object::New(env->isolate());
      safeV8::Set(isolate, o,env->address_string(),OneByteString(env->isolate(),ip))
  .OnVal([&]()-> safeV8::SafeV8Promise_Base {

      {
    bool safeV8_Failed5 = false;
    Local<Value> safeV8_exceptionThrown5;
safeV8::Set(isolate, o,env->netmask_string(),OneByteString(env->isolate(),netmask))
  .OnVal([&]()-> safeV8::SafeV8Promise_Base {

      {
    bool safeV8_Failed4 = false;
    Local<Value> safeV8_exceptionThrown4;
safeV8::Set(isolate, o,env->family_string(),family)
  .OnVal([&]()-> safeV8::SafeV8Promise_Base {

      {
    bool safeV8_Failed3 = false;
    Local<Value> safeV8_exceptionThrown3;
safeV8::Set(isolate, o,env->mac_string(),FIXED_ONE_BYTE_STRING(env->isolate(),mac))
  .OnVal([&]()-> safeV8::SafeV8Promise_Base {


    if (interfaces[i].address.address4.sin_family == AF_INET6) {
      uint32_t scopeid = interfaces[i].address.address6.sin6_scope_id;
        {
    bool safeV8_Failed1 = false;
    Local<Value> safeV8_exceptionThrown1;
safeV8::Set(isolate, o,env->scopeid_string(),Integer::NewFromUnsigned(env->isolate(),scopeid))
  .OnVal([&]()-> safeV8::SafeV8Promise_Base {
    return safeV8::Done;
  })
    .OnErr([&](Local<Value> exception){ safeV8_Failed1 = true; safeV8_exceptionThrown1 = exception; });
    if(safeV8_Failed1) return safeV8::Err(safeV8_exceptionThrown1);

}
}

    const bool internal = interfaces[i].is_internal;
    o->Set(env->internal_string(),
           internal ? True(env->isolate()) : False(env->isolate()));

      {
    bool safeV8_Failed2 = false;
    Local<Value> safeV8_exceptionThrown2;
safeV8::Set(isolate, ifarr,ifarr->Length(),o)
  .OnVal([&]()-> safeV8::SafeV8Promise_Base {
    return safeV8::Done;
  })
    .OnErr([&](Local<Value> exception){ safeV8_Failed2 = true; safeV8_exceptionThrown2 = exception; });
    if(safeV8_Failed2) return safeV8::Err(safeV8_exceptionThrown2);

  
}
return safeV8::Done;
})
    .OnErr([&](Local<Value> exception){ safeV8_Failed3 = true; safeV8_exceptionThrown3 = exception; });
    if(safeV8_Failed3) return safeV8::Err(safeV8_exceptionThrown3);

  
}
return safeV8::Done;
})
    .OnErr([&](Local<Value> exception){ safeV8_Failed4 = true; safeV8_exceptionThrown4 = exception; });
    if(safeV8_Failed4) return safeV8::Err(safeV8_exceptionThrown4);

  
}
return safeV8::Done;
})
    .OnErr([&](Local<Value> exception){ safeV8_Failed5 = true; safeV8_exceptionThrown5 = exception; });
    if(safeV8_Failed5) return safeV8::Err(safeV8_exceptionThrown5);

  
}
return safeV8::Done;
})
  .OnErr([&isolate](Local<Value> exception){
    isolate->ThrowException(exception);
  });
}

  uv_free_interface_addresses(interfaces, count);
  args.GetReturnValue().Set(ret);
}


static void GetHomeDirectory(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  char buf[PATH_MAX];

  size_t len = sizeof(buf);
  const int err = uv_os_homedir(buf, &len);

  if (err) {
    return env->ThrowUVException(err, "uv_os_homedir");
  }

  Local<String> home = String::NewFromUtf8(env->isolate(),
                                           buf,
                                           String::kNormalString,
                                           len);
  args.GetReturnValue().Set(home);
}


static void GetUserInfo(const FunctionCallbackInfo<Value>& args) {
  v8::Isolate* isolate = Environment::GetCurrent(args)->isolate();
  Environment* env = Environment::GetCurrent(args);
  uv_passwd_t pwd;
  enum encoding encoding;

  if (args[0]->IsObject()) {
    Local<Object> options = args[0].As<Object>();
      safeV8::Get(isolate, options,env->encoding_string())
  .OnVal([&](Local<Value> options_envencoding_string)-> safeV8::SafeV8Promise_Base {
Local<Value> encoding_opt = options_envencoding_string;
    encoding = ParseEncoding(env->isolate(), encoding_opt, UTF8);
  
  return safeV8::Done;
})
  .OnErr([&isolate](Local<Value> exception){
    isolate->ThrowException(exception);
  });
} else {
    encoding = UTF8;
  }

  const int err = uv_os_get_passwd(&pwd);

  if (err) {
    return env->ThrowUVException(err, "uv_os_get_passwd");
  }

  Local<Value> uid = Number::New(env->isolate(), pwd.uid);
  Local<Value> gid = Number::New(env->isolate(), pwd.gid);
  Local<Value> username = StringBytes::Encode(env->isolate(),
                                              pwd.username,
                                              encoding);
  Local<Value> homedir = StringBytes::Encode(env->isolate(),
                                             pwd.homedir,
                                             encoding);
  Local<Value> shell;

  if (pwd.shell == NULL)
    shell = Null(env->isolate());
  else
    shell = StringBytes::Encode(env->isolate(), pwd.shell, encoding);

  uv_os_free_passwd(&pwd);

  if (username.IsEmpty()) {
    return env->ThrowUVException(UV_EINVAL,
                                 "uv_os_get_passwd",
                                 "Invalid character encoding for username");
  }

  if (homedir.IsEmpty()) {
    return env->ThrowUVException(UV_EINVAL,
                                 "uv_os_get_passwd",
                                 "Invalid character encoding for homedir");
  }

  if (shell.IsEmpty()) {
    return env->ThrowUVException(UV_EINVAL,
                                 "uv_os_get_passwd",
                                 "Invalid character encoding for shell");
  }

  Local<Object> entry = Object::New(env->isolate());

    safeV8::Set(isolate, entry,env->uid_string(),uid)
  .OnVal([&]()-> safeV8::SafeV8Promise_Base {

    {
    bool safeV8_Failed4 = false;
    Local<Value> safeV8_exceptionThrown4;
safeV8::Set(isolate, entry,env->gid_string(),gid)
  .OnVal([&]()-> safeV8::SafeV8Promise_Base {

    {
    bool safeV8_Failed3 = false;
    Local<Value> safeV8_exceptionThrown3;
safeV8::Set(isolate, entry,env->username_string(),username)
  .OnVal([&]()-> safeV8::SafeV8Promise_Base {

    {
    bool safeV8_Failed2 = false;
    Local<Value> safeV8_exceptionThrown2;
safeV8::Set(isolate, entry,env->homedir_string(),homedir)
  .OnVal([&]()-> safeV8::SafeV8Promise_Base {

    {
    bool safeV8_Failed1 = false;
    Local<Value> safeV8_exceptionThrown1;
safeV8::Set(isolate, entry,env->shell_string(),shell)
  .OnVal([&]()-> safeV8::SafeV8Promise_Base {


  args.GetReturnValue().Set(entry);

  return safeV8::Done;
})
    .OnErr([&](Local<Value> exception){ safeV8_Failed1 = true; safeV8_exceptionThrown1 = exception; });
    if(safeV8_Failed1) return safeV8::Err(safeV8_exceptionThrown1);

  
}
return safeV8::Done;
})
    .OnErr([&](Local<Value> exception){ safeV8_Failed2 = true; safeV8_exceptionThrown2 = exception; });
    if(safeV8_Failed2) return safeV8::Err(safeV8_exceptionThrown2);

  
}
return safeV8::Done;
})
    .OnErr([&](Local<Value> exception){ safeV8_Failed3 = true; safeV8_exceptionThrown3 = exception; });
    if(safeV8_Failed3) return safeV8::Err(safeV8_exceptionThrown3);

  
}
return safeV8::Done;
})
    .OnErr([&](Local<Value> exception){ safeV8_Failed4 = true; safeV8_exceptionThrown4 = exception; });
    if(safeV8_Failed4) return safeV8::Err(safeV8_exceptionThrown4);

  
}
return safeV8::Done;
})
  .OnErr([&isolate](Local<Value> exception){
    isolate->ThrowException(exception);
  });
}


void Initialize(Local<Object> target,
                Local<Value> unused,
                Local<Context> context) {
  Environment* env = Environment::GetCurrent(context);
  env->SetMethod(target, "getHostname", GetHostname);
  env->SetMethod(target, "getLoadAvg", GetLoadAvg);
  env->SetMethod(target, "getUptime", GetUptime);
  env->SetMethod(target, "getTotalMem", GetTotalMemory);
  env->SetMethod(target, "getFreeMem", GetFreeMemory);
  env->SetMethod(target, "getCPUs", GetCPUInfo);
  env->SetMethod(target, "getOSType", GetOSType);
  env->SetMethod(target, "getOSRelease", GetOSRelease);
  env->SetMethod(target, "getInterfaceAddresses", GetInterfaceAddresses);
  env->SetMethod(target, "getHomeDirectory", GetHomeDirectory);
  env->SetMethod(target, "getUserInfo", GetUserInfo);
  target->Set(FIXED_ONE_BYTE_STRING(env->isolate(), "isBigEndian"),
              Boolean::New(env->isolate(), IsBigEndian()));
}

}  // namespace os
}  // namespace node

NODE_MODULE_CONTEXT_AWARE_BUILTIN(os, node::os::Initialize)
