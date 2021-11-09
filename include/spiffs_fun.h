#ifndef _SPIFFS_FUN_H__
#define _SPIFFS_FUN_H__
#include "FS.h"
#include "SPIFFS.h"   // ESP32

void listDir(const char *dirname);
void readFile(const char *path);
void writeFile(const char *path, const char *message);
void appendFile(const char *path, const char *message);
void renameFile(const char *path1, const char *path2);
void deleteFile(const char *path);
File returnFile(const char *path);
void testFileIO(const char *path);

#endif 
