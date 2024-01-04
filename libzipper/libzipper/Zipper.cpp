#include "zipper.h"

#include <algorithm>
#include <functional>
#include <iostream>


constexpr uint8_t pPkZipHeader[] = {'P', 'K', 0x05, 0x06, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};



CZipper::CZipper(const std::wstring& strSourcePath, const std::wstring& strDestPath) : m_strSourcePath(strSourcePath), m_strDestPath(strDestPath)
{
    if (SUCCEEDED(CoInitializeEx(nullptr, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE)))
    {
        CLSID shellApplicationId;
        if (SUCCEEDED(CLSIDFromProgID(L"Shell.Application", &shellApplicationId)))
        {
            if (SUCCEEDED(m_spSD.CoCreateInstance(shellApplicationId, nullptr, CLSCTX_ALL)))
            {
                m_bIsInitialize = true;
            }
        }
    }
}

CZipper::~CZipper()
{
    if (m_bIsInitialize)
    {
        CoUninitialize();
    }
}

EFailedCode CZipper::Operate() const
{
    if (!m_bIsInitialize)
    {
        return EFailedCode::ComNotInited;
    }

    auto eOperateType = GetOperationType();

    if (GetFileAttributesW(m_strSourcePath.c_str()) == INVALID_FILE_ATTRIBUTES)
    {
        return EFailedCode::SrcFolderNotExist;
    }

    if (GetFileAttributesW(m_strDestPath.c_str()) == INVALID_FILE_ATTRIBUTES)
    {
        if (eOperateType == EOperationType::UnZip)
        {
            if (!CreateDirectoryW(m_strDestPath.c_str(), nullptr))
            {
                return EFailedCode::DestCreateFailed;
            }
        } 
        else if (eOperateType == EOperationType::Zip)
        {
            auto hFile = CreateFileW(m_strDestPath.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
            
            if (hFile == nullptr)
            {
                return EFailedCode::DestCreateFailed;
            }

            if (!WriteFile(hFile, reinterpret_cast<LPCVOID>(pPkZipHeader), sizeof(pPkZipHeader), nullptr, nullptr))
            {
                return EFailedCode::DestCreateFailed;
            }

            CloseHandle(hFile);
        }
    }

    CComBSTR spBStrSrcPath(m_strSourcePath.c_str());
    CComVariant spSrcPathVariant(spBStrSrcPath);
    
    CComPtr<Folder> spSrcFolderComObject;
    if (!SUCCEEDED(m_spSD->NameSpace(spSrcPathVariant, &spSrcFolderComObject)))
    {
        return EFailedCode::GetFolderObjectFailed;
    }



    CComBSTR spBStrDestPath(m_strDestPath.c_str());
    CComVariant spDestVariant(spBStrDestPath);
    CComPtr<Folder> spDestFolderComObject;
    if (!SUCCEEDED(m_spSD->NameSpace(spDestVariant, &spDestFolderComObject)))
    {
        return EFailedCode::GetFolderObjectFailed;
    }

    CComVariant spCopyOptionVariant(FOF_NO_UI);

    
    CComPtr<FolderItems> spSrcFolderItemComObject;
    if (!SUCCEEDED(spSrcFolderComObject->Items(&spSrcFolderItemComObject)))
    {
        return EFailedCode::GetSourceFolderItemsObjectFailed;
    }

    auto kInfo = GetFileCount(spSrcFolderItemComObject);

    if (!kInfo.m_IsSuccess)
    {
        return EFailedCode::GetFolderCountFailed;
    }

    CComVariant spSrcFolderItemsVariant(spSrcFolderItemComObject);

    if (!SUCCEEDED(spDestFolderComObject->CopyHere(spSrcFolderItemsVariant, spCopyOptionVariant)))
    {
        return EFailedCode::CopyFileFailed;
    }

    if (!WaitUntilCopyDone(kInfo, spDestFolderComObject))
    {
        return EFailedCode::GetFolderCountFailed;
    }
    
    return EFailedCode::NoError;
}

EOperationType CZipper::GetOperationType() const
{
    std::wstring strSrcExt(PathFindExtensionW(m_strSourcePath.c_str()));
    
    std::transform(strSrcExt.cbegin(), strSrcExt.cend(), strSrcExt.begin(), [](const wchar_t wc)
        {
            return std::tolower(wc);
        });

    if (strSrcExt == L".zip")
    {
        return EOperationType::UnZip;
    }

    return EOperationType::Zip;
}

CFileCountInfomation CZipper::GetFileCount(const CComPtr<FolderItems>& spFolderItemsComObject)
{
    std::function<CFileCountInfomation(const CComPtr<FolderItems>&)> fnRecurGetCount = [&](const CComPtr<FolderItems>& spRootFI) -> CFileCountInfomation
        {
            CFileCountInfomation kInfo;

            long lItemCount = 0;
            if (SUCCEEDED(spRootFI->get_Count(&lItemCount)))
            {
                for (long i = 0; i < lItemCount; ++i)
                {
                    CComPtr<FolderItem> spSubFolderItemComObject;
                    CComVariant spItemIndexVariant(i);

                    if (SUCCEEDED(spRootFI->Item(spItemIndexVariant, &spSubFolderItemComObject)))
                    {
                        VARIANT_BOOL nOutIsFolder;
                        spSubFolderItemComObject->get_IsFolder(&nOutIsFolder);

                        if (nOutIsFolder == VARIANT_TRUE)
                        {
                            ++kInfo.m_nFolder;

                            CComPtr<IDispatch> spSubDispatchComObject;
                            if (SUCCEEDED(spSubFolderItemComObject->get_GetFolder(&spSubDispatchComObject)))
                            {
                                CComPtr<Folder> spSubFolderComObject;

                                if (SUCCEEDED(spSubDispatchComObject->QueryInterface(&spSubFolderComObject)))
                                {
                                    CComPtr<FolderItems> spSubFolderItemsComObject;

                                    if (SUCCEEDED(spSubFolderComObject->Items(&spSubFolderItemsComObject)))
                                    {
                                        auto kSubInfo = fnRecurGetCount(spSubFolderItemsComObject);

                                        if (!kSubInfo.m_IsSuccess)
                                        {
                                            kInfo.m_IsSuccess = false;
                                            break;
                                        }

                                        kInfo.m_nFile += kSubInfo.m_nFile;
                                        kInfo.m_nFolder += kSubInfo.m_nFolder;
                                    }
                                    else
                                    {
                                        kInfo.m_IsSuccess = false;
                                        break;
                                    }
                                }
                                else
                                {
                                    kInfo.m_IsSuccess = false;
                                    break;
                                }
                            }
                            else
                            {
                                kInfo.m_IsSuccess = false;
                                break;
                            }
                        }
                        else
                        {
                            ++kInfo.m_nFile;
                        }
                    }
                    else
                    {
                        kInfo.m_IsSuccess = false;
                        break;
                    }
                }
            }
            else
            {
                kInfo.m_IsSuccess = false;
            }

            return kInfo;
        };

    return fnRecurGetCount(spFolderItemsComObject);
}

bool CZipper::WaitUntilCopyDone(const CFileCountInfomation& kSrcInfo, const CComPtr<Folder>& spDestFolderComObject)
{
    for (;;)
    {
        CComPtr<FolderItems> spDestSubFolderItems;
        if (SUCCEEDED(spDestFolderComObject->Items(&spDestSubFolderItems)))
        {
            auto kDestInfo = GetFileCount(spDestSubFolderItems);

            if (!kDestInfo.m_IsSuccess)
            {
                return false;
            }

            if (kSrcInfo.m_nFolder == kDestInfo.m_nFolder && kSrcInfo.m_nFile == kDestInfo.m_nFile)
            {
                break;
            }
        }
        else
        {
            return false;
        }
    }

    return true;
}

