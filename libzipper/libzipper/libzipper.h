#pragma once

#include <stdint.h>
#include <stddef.h>

enum LibZipperOperaionType
{
    LibZipper_Zip = 0,
    LibZipper_UnZip,
};

enum LibZipFailedType
{
    LibZipper_NoError = 0,
    LibZipper_ComNotInited,
    LibZipper_SrcFolderNotExist,
    LibZipper_DestCreateFailed,
    LibZipper_GetFolderObjectFailed,
    LibZipper_GetSourceFolderItemsObjectFailed,
    LibZipper_GetFolderCountFailed,
    LibZipper_CopyFileFailed,
};

enum LibZipMethodReturnCode
{
    LibZipper_OperateSucceeded = 0,
    LibZipper_OperateFailed,
    LibZipper_OperateNotCreated,
};

typedef uint64_t ZipperHandle;

#ifdef __cplusplus
extern "C"
{
#endif

    __declspec(dllexport) ZipperHandle ZipperCreate(const wchar_t* strSrcPath, const wchar_t* strDestPath);
    __declspec(dllexport) void ZipperFree(const ZipperHandle hZipper);

    __declspec(dllexport) int ZipperOperate(const ZipperHandle hZipper, int* nFailedType);

#ifdef __cplusplus
}
#endif