#include <Napi.h>
#include <libzipper.h>

#include <optional>
#include <string>

#define CHECK_ARGUMENTS_MIN_COUNT(COUNT)                                                                                          \
    if (info.Length() < COUNT)                                                                                                    \
    {                                                                                                                             \
        Napi::TypeError::New(env, "invalid count of arguments, must at least " #COUNT " arguments").ThrowAsJavaScriptException(); \
        return env.Null();                                                                                                        \
    }

#define CHECK_ARGUMENTS_TYPE(TYPE, ASTYPE, POS)                                                                         \
    if (!info[POS].Is##TYPE())                                                                                          \
    {                                                                                                                   \
        Napi::TypeError::New(env, "parameter position " #POS " must be a " #TYPE " type").ThrowAsJavaScriptException(); \
        return env.Null();                                                                                              \
    }                                                                                                                   \
    auto arg##POS = info[POS].As<Napi::##ASTYPE>();

#define CONVERT_ARG0_TO_UHANDLE \
    uint64_t uHandle = std::stoull(arg0.Utf8Value());

#define NODE_FUNC(FUNCNAME) \
    exports.Set(Napi::String::New(env, #FUNCNAME), Napi::Function::New(env, NodeFunc_##FUNCNAME));

static inline std::optional<std::string> GetErrorStringFromRetCode(int nRetCode)
{
    switch (nRetCode)
    {
    case LibZipper_NoError:
    {
        return std::nullopt;
    }
    case LibZipper_ComNotInited:
    {
        return "COM instance is not initialized";
    }
    case LibZipper_SrcFolderNotExist:
    {
        return "source folder is not exists";
    }
    case LibZipper_DestCreateFailed:
    {
        return "destation creatation failed";
    }
    case LibZipper_GetFolderObjectFailed:
    {
        return "get folder COM instance failed";
    }
    case LibZipper_GetSourceFolderItemsObjectFailed:
    {
        return "get source folder items COM instance failed";
    }
    case LibZipper_GetFolderCountFailed:
    {
        return "get count infomation of folder failed";
    }
    case LibZipper_CopyFileFailed:
    {
        return "copy file operation failed";
    }
    }

    return "unknown error";
}

Napi::Value NodeFunc_ZipperCreate(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    CHECK_ARGUMENTS_MIN_COUNT(2);
    CHECK_ARGUMENTS_TYPE(String, String, 0);
    CHECK_ARGUMENTS_TYPE(String, String, 1);

    auto strSrcPath = arg0.Utf16Value();
    auto strDestSrcPath = arg1.Utf16Value();

    auto wstrSrcPath = std::wstring(strSrcPath.begin(), strSrcPath.end());
    auto wstrDestSrcPath = std::wstring(strDestSrcPath.begin(), strDestSrcPath.end());

    auto uHandle = ZipperCreate(wstrSrcPath.c_str(), wstrDestSrcPath.c_str());
    if (uHandle == 0)
    {
        Napi::Error::New(env, "cannot construct zipper from native dll, unknown reason").ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::String::New(env, std::to_string(uHandle));
}

Napi::Value NodeFunc_ZipperFree(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    CHECK_ARGUMENTS_MIN_COUNT(1);
    CHECK_ARGUMENTS_TYPE(String, String, 0);
    CONVERT_ARG0_TO_UHANDLE;

    ZipperFree(uHandle);
}

Napi::Value NodeFunc_ZipperOperate(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    CHECK_ARGUMENTS_MIN_COUNT(1);
    CHECK_ARGUMENTS_TYPE(String, String, 0);
    CONVERT_ARG0_TO_UHANDLE;

    int nOperationRetCode = LibZipper_NoError;
    int nCallRetCode = ZipperOperate(uHandle, &nOperationRetCode);
    if (nCallRetCode != LibZipper_OperateSucceeded)
    {
        if (nCallRetCode == LibZipper_OperateNotCreated)
        {
            Napi::Error::New(env, "zipper instance is not constructed").ThrowAsJavaScriptException();
            return env.Null();
        }

        auto kRetStringOptional = GetErrorStringFromRetCode(nOperationRetCode);

        Napi::Error::New(env, kRetStringOptional.value()).ThrowAsJavaScriptException();
        return env.Null();
    }

    return env.Null();
}

Napi::Object Initialize(Napi::Env env, Napi::Object exports)
{
    NODE_FUNC(ZipperCreate);
    NODE_FUNC(ZipperFree);
    NODE_FUNC(ZipperOperate);
    return exports;
}

NODE_API_MODULE(Zipper, Initialize)