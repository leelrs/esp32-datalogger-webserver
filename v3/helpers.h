#ifndef HELPERS_H
#define HELPERS_H

String getNextFileName(fs::FS &fs, const char * dirname = "/") {
  int maxNum = 0;
  File root = fs.open(dirname);
  if(!root || !root.isDirectory()){
    Serial.println("Failed to open directory or not a directory");
    return "/axleStrain_1.bin";// Default name if something goes wrong
  }

  File file = root.openNextFile();
  while(file){
    if(!file.isDirectory()){
      String fileName = file.name();
      int numStartIndex = fileName.indexOf('_') + 1;
      int numEndIndex = fileName.indexOf('.', numStartIndex);
      if (numStartIndex > 0 && numEndIndex > numStartIndex) {
        int fileNum = fileName.substring(numStartIndex, numEndIndex).toInt();
        if (fileNum > maxNum) {
          maxNum = fileNum;
        }
      }
    }
    file = root.openNextFile();
  }
  return "/axleStrain_" + String(maxNum + 1) + ".bin";
}


#endif 