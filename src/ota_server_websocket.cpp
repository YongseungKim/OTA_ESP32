#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include "littlefs_fun.h"
#include "web_config.h"

const char *host = "esp32";

const char *ssid = "SK_WiFiGIGAFE4B";
const char *password = "1704002831";

// const char *ssid = "VCOMM_LAB";
// const char *password = "vcomm13579";

WebServer server(80);
WebConfig conf;

// AsyncWebServer server(80);

bool ledState = 0;
const int ledPin = 2;


#define FORMAT_LITTLEFS_IF_FAILED true
static void handleNotFound();
String processor(const String &var);
String RestartESP(const String &var);

/*
 * setup function
 */
void setup(void)
{
    Serial.begin(115200);

    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);

    if (!LITTLEFS.begin(FORMAT_LITTLEFS_IF_FAILED))
    {
        Serial.println("LITTLEFS Mount Failed");
        return;
    }

    listDir(LITTLEFS, "/", 0);

    // FS mfs = FS(nullptr);
    // File file_js = mfs.open("/jquery.min.js", "r");

    // Connect to WiFi network
    WiFi.begin(ssid, password);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    /*use mdns for host name resolution*/
    if (!MDNS.begin(host))
    { //http://esp32.local
        Serial.println("Error setting up MDNS responder!");
        while (1)
        {
            delay(1000);
        }
    }
    Serial.println("mDNS responder started");

    server.on("/", HTTP_GET, []()
              {
                  File file_js = returnFile(LITTLEFS, "/login_index.html");

                  if (!file_js)
                  {
                      Serial.println("- failed to open file");
                      server.send(500, "text/plain", "Problem with filesystem!\n");
                      return;
                  }

                  String contentType = "text/html";
                  server.sendHeader("Connection", "close");
                    // server.send(200, "text/html", serverIndex);
                  server.streamFile(file_js, contentType);
                  file_js.close();

                //   server.sendHeader("Connection", "close");
                //   server.send(200, "text/html", loginIndex);
              });

    server.on("/serverIndex", HTTP_GET, []()
              {
                  File file_js = returnFile(LITTLEFS, "/sever_index.html");

                  if (!file_js)
                  {
                      Serial.println("- failed to open file");
                      server.send(500, "text/plain", "Problem with filesystem!\n");
                      return;
                  }

                  String contentType = "text/html";
                  server.sendHeader("Connection", "close");
                  //   server.send(200, "text/html", serverIndex);
                  server.streamFile(file_js, contentType);
                  file_js.close();
              });

    server.on(
        "/update", HTTP_POST, []()
        {
            server.sendHeader("Connection", "close");
            ESP.restart();
        },
        []()
        {
            HTTPUpload &upload = server.upload();
            if (upload.status == UPLOAD_FILE_START)
            {
                Serial.printf("Update: %s\n", upload.filename.c_str());
                if (!Update.begin(UPDATE_SIZE_UNKNOWN))
                { //start with max available size
                    Update.printError(Serial);
                }
            }
            else if (upload.status == UPLOAD_FILE_WRITE)
            {
                /* flashing firmware to ESP*/
                if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
                {
                    Update.printError(Serial);
                }
            }
            else if (upload.status == UPLOAD_FILE_END)
            {
                if (Update.end(true))
                { //true to set the size to the current progress
                    Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
                }
                else
                {
                    Update.printError(Serial);
                }
            }
        });

    server.onNotFound(handleNotFound);
    server.begin();
}

void loop(void)
{
    server.handleClient();
    delay(1);
}





static void handleNotFound()
{
    String path = server.uri(); // Important!

    if (!LITTLEFS.exists(path))
    {
        server.send(404, "text/plain", "Path " + path + " not found. Please double-check the URL");
        return;
    }
    String contentType = "text/plain";
    if (path.endsWith(".css"))
    {
        contentType = "text/css";
    }
    else if (path.endsWith(".html"))
    {
        contentType = "text/html";
    }
    else if (path.endsWith(".js"))
    {
        contentType = "application/javascript";
    }
    File file = LITTLEFS.open(path, "r");
    server.streamFile(file, contentType);
    file.close();
}


String processor(const String &var)
{
    Serial.println(var);
    if (var == "STATE")
    {
        if (ledState)
        {
            return "ON";
        }
        else
        {
            return "OFF";
        }
    }
    return String();
}

String RestartESP(const String &var)
{
    ESP.restart();
    return "Esp restart";
}