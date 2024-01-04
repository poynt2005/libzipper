interface NativeBinding {
  ZipperCreate: (srcPath: string, destPath: string) => string;
  ZipperFree: (handle: string) => void;
  ZipperOperate: (handle: string) => void;
}
