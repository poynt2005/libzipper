var path = require("path");

var zipperIncl = path
  .resolve("../libzipper/libzipper/")
  .replace(/\\/gm, "\\\\");

var zipperLib = path
  .resolve("../libzipper/x64/Release/libzipper.lib")
  .replace(/\\/gm, "\\\\");

module.exports = {
  zipperLib: zipperLib,
  zipperIncl: zipperIncl,
};
