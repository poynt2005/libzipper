#pragma once

#include <string>

#include <Windows.h>
#include <shldisp.h>
#include <ExDisp.h>
#include <ShObjIdl.h>
#include <ShlGuid.h>
#include <atlbase.h>

enum class EOperationType
{
    Zip = 0,
    UnZip,
};

enum class EFailedCode
{
    NoError = 0,
    ComNotInited,
    SrcFolderNotExist,
    DestCreateFailed,
    GetFolderObjectFailed,
    GetSourceFolderItemsObjectFailed,
    GetFolderCountFailed,
    CopyFileFailed,
};

using CFileCountInfomation = struct cFileCountInfomation
{
    bool m_IsSuccess = true;
    int m_nFolder = 0;
    int m_nFile = 0;
};

class CZipper
{
public:
    CZipper(const std::wstring& strSourcePath, const std::wstring& strDestPath);

    bool m_bIsInitialize = false;
    
    EOperationType GetOperationType() const;

    EFailedCode Operate() const;

    ~CZipper();
private:
    std::wstring m_strSourcePath;
    std::wstring m_strDestPath;
    CComPtr<IShellDispatch> m_spSD = nullptr;

    static CFileCountInfomation GetFileCount(const CComPtr<FolderItems>& spFolderItemsComObject);
    static bool WaitUntilCopyDone(const CFileCountInfomation& kSrcInfo, const CComPtr<Folder>& spDestFolder);
};