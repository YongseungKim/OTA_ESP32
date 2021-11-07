#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

// #include <AsyncTCP.h>
// #include <ESPAsyncWebServer.h>
// #include "littlefs_fun.h"


const char *host = "esp32";

const char* ssid = "SK_WiFiGIGAFE4B";
const char* password = "1704002831";

WebServer server(80);
// AsyncWebServer server(80);



bool ledState = 0;
const int ledPin = 2;

/*
 * Login page
 */

const char *loginIndex =
    "<form name='loginForm'>"
    "   <table width='20%' bgcolor='A09F9F' align='center'>"
    "       <tr>"
    "           <td colspan=2>"
    "               <center><font size=4><b>ESP32 Login Page</b></font></center>"
    "               <br>"
    "           </td>"
    "           <br>"
    "           <br>"
    "       </tr>"
    "       <td>Username:</td>"
    "       <td><input type='text' size=25 name='userid'><br></td>"
    "       </tr>"
    "       <br>"
    "       <br>"
    "       <tr>"
    "           <td>Password:</td>"
    "           <td><input type='Password' size=25 name='pwd'><br></td>"
    "           <br>"
    "           <br>"
    "       </tr>"
    "       <tr>"
    "           <td><input type='submit' onclick='check(this.form)' value='Login'></td>"
    "       </tr>"
    "   </table>"
    "</form>"
    "<script>"
    "   function check(form)"
    "   {"
    "       if(form.userid.value=='admin' && form.pwd.value=='admin')"
    "       {"
    "           window.open('/serverIndex')"
    "       }"
    "       else"
    "       {"
    "           alert('Error Password or Username')/*displays error message*/"
    "       }"
    "   }"
    "</script>";

/*
 * Server Index Page
 */
// 
// https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js
const char *serverIndex =
    "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
    "<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
    "   <input type='file' name='update'>"
    "   <input type='submit' value='Update'>"
    "</form>"
    "<div id='prg'>progress: 0%</div>"
    "   <script>"
    "   $('form').submit(function(e){"
    "           e.preventDefault();"
    "           var form = $('#upload_form')[0];"
    "           var data = new FormData(form);"
    "       $.ajax({"
    "           url: '/update',"
    "           type: 'POST',"
    "           data: data,"
    "           contentType: false,"
    "           processData:false,"
    "           xhr: function() {"
    "               var xhr = new window.XMLHttpRequest();"
    "               xhr.upload.addEventListener('progress', function(evt) {"
    "                   if (evt.lengthComputable) {"
    "                       var per = evt.loaded / evt.total;"
    "                       $('#prg').html('progress: ' + Math.round(per*100) + '%');"
    "                   }"
    "               }, false);"
    "               return xhr;"
    "           },"
    "           success:function(d, s) {"
    "               console.log('success!')"
    "           },"
    "           error: function (a, b, c) {"
    "           }"
    "       });"
    "   });"
    "   </script>";

#define FORMAT_LITTLEFS_IF_FAILED true


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

/*
 * setup function
 */
void setup(void)
{
    Serial.begin(115200);

    
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);


    // if(!LITTLEFS.begin(FORMAT_LITTLEFS_IF_FAILED)){
    //     Serial.println("LITTLEFS Mount Failed");
    //     return;
    // }

    // listDir(LITTLEFS, "/", 0);
    
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
    /*return index page which is stored in serverIndex */
    // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    //           { request->send_P(200, "text/html", loginIndex, processor); });

    // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    //           { request->send_P(200, "text/html", serverIndex, processor); });

    // //
    // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    //           { request->send_P(200, "text/html", (Update.hasError()) ? "FAIL" : "OK", RestartESP); },
    //           {
    //             HTTPUpload &upload = server.upload();
    //             if (upload.status == UPLOAD_FILE_START)
    //             {
    //                 Serial.printf("Update: %s\n", upload.filename.c_str());
    //                 if (!Update.begin(UPDATE_SIZE_UNKNOWN))
    //                 { //start with max available size
    //                     Update.printError(Serial);
    //                 }
    //             }
    //             else if (upload.status == UPLOAD_FILE_WRITE)
    //             {
    //                 /* flashing firmware to ESP*/
    //                 if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
    //                 {
    //                     Update.printError(Serial);
    //                 }
    //             }
    //             else if (upload.status == UPLOAD_FILE_END)
    //             {
    //                 if (Update.end(true))
    //                 { //true to set the size to the current progress
    //                     Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
    //                 }
    //                 else
    //                 {
    //                     Update.printError(Serial);
    //                 }
    //             }
    //         });

    server.on("/", HTTP_GET, []()
              {
                  server.sendHeader("Connection", "close");
                  server.send(200, "text/html", loginIndex);
              });
    server.on("/serverIndex", HTTP_GET, []()
              {
                  server.sendHeader("Connection", "close");
                  server.send(200, "text/html", serverIndex);
                // server.sendP(200, "text/html", "/serverIndex.html");
              });
    /*handling uploading firmware file */
    //   void send_P(int code, PGM_P content_type, PGM_P content);
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

    server.begin();
}

void loop(void)
{
    server.handleClient();
    delay(1);
}