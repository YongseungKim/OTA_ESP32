#include "FS.h"
#include "SPIFFS.h"   // ESP32

#ifndef CONFIG_LITTLEFS_FOR_IDF_3_2
 #include <time.h>
#endif

void listDir(const char *dirname)
{
    Serial.printf("Listing directory: %s\r\n", dirname);
    File root = SPIFFS.open(dirname); // ESP8266은 확장자 "Dir"과 "File"로 구분해서 사용, ESP32는 "File"로 통합
    File file = root.openNextFile();
    while (file)
    { // 다음 파일이 있으면(디렉토리 또는 파일)
        if (file.isDirectory())
        { // 다음 파일이 디렉토리 이면
            Serial.print("  DIR : ");
            Serial.println(file.name()); // 디렉토리 이름 출력
        }
        else
        { // 파일이면
            Serial.print("  FILE: ");
            Serial.print(file.name()); // 파일이름
            Serial.print("\tSIZE: ");
            Serial.println(file.size()); // 파일 크기
        }
        file = root.openNextFile();
    }
}

void readFile(const char *path)
{
    Serial.printf("Reading file: %s\r\n", path);
    File file = SPIFFS.open(path, "r");
    if (!file || file.isDirectory())
    {
        Serial.println("- failed to open file for reading");
        return;
    }
    Serial.println("read from file:");
    while (file.available())
    {
        Serial.write(file.read());
    }
}

void writeFile(const char *path, const char *message)
{
    Serial.printf("Writing file: %s\r\n", path);
    File file = SPIFFS.open(path, "w");
    if (!file)
    {
        Serial.println("failed to open file for writing");
        return;
    }
    if (file.print(message))
    {
        Serial.println("file written");
    }
    else
    {
        Serial.println("frite failed");
    }
}

void appendFile(const char *path, const char *message)
{
    Serial.printf("Appending to file: %s\r\n", path);
    File file = SPIFFS.open(path, "a");
    if (!file)
    {
        Serial.println("failed to open file for appending");
        return;
    }
    if (file.print(message))
    {
        Serial.println("message appended");
    }
    else
    {
        Serial.println("append failed");
    }
}

void renameFile(const char *path1, const char *path2)
{
    Serial.printf("Renaming file %s to %s\r\n", path1, path2);
    if (SPIFFS.rename(path1, path2))
    {
        Serial.println("file renamed");
    }
    else
    {
        Serial.println("rename failed");
    }
}

void deleteFile(const char *path)
{
    Serial.printf("Deleting file: %s\r\n", path);
    if (SPIFFS.remove(path))
    {
        Serial.println("file deleted");
    }
    else
    {
        Serial.println("delete failed");
    }
}

File returnFile(const char *path)
{
    Serial.printf("Reading file: %s\r\n", path);
    File file = SPIFFS.open(path, "r");
    if (!file || file.isDirectory())
    {
        Serial.println("- failed to open file for reading");
        return file;
    }
    // Serial.println("read from file:");
    // while (file.available())
    // {
    //     Serial.write(file.read());
    // }
    return file;
}


void testFileIO(const char *path)
{
    Serial.printf("Testing file I/O with %s\r\n", path);
    static uint8_t buf[512];
    size_t len = 0;
    File file = SPIFFS.open(path, "w");
    if (!file)
    {
        Serial.println("failed to open file for writing");
        return;
    }
    size_t i;
    Serial.print("writing");
    uint32_t start = millis();
    for (i = 0; i < 1024; i++)
    {
        // ESP8266 - 1024
        if ((i & 0x001F) == 0x001F)
        {
            Serial.print(".");
        }
        file.write(buf, 512);
    }
    Serial.println("");
    uint32_t end = millis() - start;
    Serial.printf("%u bytes written in %u ms\r\n", 1024 * 512, end);
    file.close();
    file = SPIFFS.open(path, "r");
    start = millis();
    end = start;
    i = 0;
    if (file && !file.isDirectory())
    {
        len = file.size();
        size_t flen = len;
        start = millis();
        Serial.print("reading");
        while (len)
        {
            size_t toRead = len;
            if (toRead > 512)
            {
                toRead = 512;
            }
            file.read(buf, toRead);
            if ((i++ & 0x001F) == 0x001F)
            {
                Serial.print(".");
            }
            len -= toRead;
        }
        Serial.println("");
        end = millis() - start;
        Serial.printf("%u bytes read in %u ms\r\n", flen, end);
        file.close();
    }
    else
    {
        Serial.println("failed to open file for reading");
    }
}


