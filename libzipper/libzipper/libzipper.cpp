#include "libzipper.h"
#include "zipper.h"

#include <unordered_map>
#include <memory>

std::unordered_map<ZipperHandle, std::unique_ptr<CZipper>> kZipperContextStore;

ZipperHandle ZipperCreate(const wchar_t* strSrcPath, const wchar_t* strDestPath)
{
    auto spZipper = std::make_unique<CZipper>(strSrcPath, strDestPath);
    auto uZipperHandle = reinterpret_cast<ZipperHandle>(spZipper.get());
    kZipperContextStore.insert(std::pair<ZipperHandle, std::unique_ptr<CZipper>>(uZipperHandle, std::move(spZipper)));
    return uZipperHandle;
}

void ZipperFree(const ZipperHandle hZipper) 
{
    kZipperContextStore.erase(hZipper);
}

int ZipperOperate(const ZipperHandle hZipper, int* nFailedType) 
{
    const auto& spZipper = kZipperContextStore[hZipper];

    if (spZipper == nullptr)
    {
        return LibZipper_OperateNotCreated;
    }

    EFailedCode eFailedCode = spZipper->Operate();
    *nFailedType = static_cast<int>(eFailedCode);

    if (eFailedCode != EFailedCode::NoError)
    {
        return LibZipper_OperateFailed;
    }

    return LibZipper_OperateSucceeded;
}