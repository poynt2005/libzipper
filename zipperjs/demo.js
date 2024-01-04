process.chdir("./dist");
var ZipperCore = require("./dist/Zipper");
var path = require("path");
var fs = require("fs");
var readline = require("readline");

var rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout,
});

rl.question(
  "[Zipper][Info ] Please enter the path name of you want to zip/unzip\n",
  (answer) => {
    var srcFileAbsPath = path.resolve(answer.trim());

    if (!fs.existsSync(srcFileAbsPath)) {
      throw new Error(`file path you entered: ${srcFileAbsPath} is not exists`);
    }

    rl.close();

    var rl2 = readline.createInterface({
      input: process.stdin,
      output: process.stdout,
    });

    rl2.question(
      "[Zipper][Info ] Please enter the path name of destation file output\n",
      (answer) => {
        var destFileAbsPath = path.resolve(answer.trim());

        var zipper = new ZipperCore(srcFileAbsPath, destFileAbsPath);
        zipper.Operate();
        zipper.Dispose();

        console.log("[Zipper][Info ] Operated done");

        rl2.close();
      }
    );
  }
);
