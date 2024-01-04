import { checkRuntimeAndGetNativeBinding } from "./RuntimeChecker";

const nativeBinding: NativeBinding = checkRuntimeAndGetNativeBinding();

class ZipperCore {
  /**
   * This is a pointer to zipper instance in native context
   * @memberof ZipperCore
   */
  private m_strNativeHandle: string = "0";

  /**
   * Creates a zipper instance, it will also create a handle point to native context
   * @param strSrcPath - Source file path
   * @param strDestPath - Destation file path
   *
   * @constructor
   */
  constructor(strSrcPath: string, strDestPath: string) {
    this.m_strNativeHandle = nativeBinding.ZipperCreate(
      strSrcPath,
      strDestPath
    );
  }

  /**
   * Remember to call dispose when end of the operation,
   * This method will terminate and free the instance in native context
   * @memberof ZipperCore
   */
  public Dispose() {
    if (this.m_strNativeHandle == "0") {
      throw new Error("zipper instance has been disposed");
    }

    nativeBinding.ZipperFree(this.m_strNativeHandle);
    this.m_strNativeHandle = "0";
  }

  /**
   * Start to zip/unzip, if operation failed, it will throw exception
   *
   * @memberof ZipperCore
   */
  public Operate() {
    if (this.m_strNativeHandle == "0") {
      throw new Error("zipper instance not created");
    }
    nativeBinding.ZipperOperate(this.m_strNativeHandle);
  }
}

export { ZipperCore };
