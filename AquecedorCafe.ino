//
// A simple server implementation showing how to:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//

#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include "DHT.h"

#define DHTTYPE DHT11
uint8_t DHTPin = 4; 
DHT dht(DHTPin, DHTTYPE);

AsyncWebServer server(80);

// Wifi
const char* ssid = "Yan-J5";
const char* password = "3524172300";

// HTTP params
const char* PARAM_MESSAGE = "message";
const char* PARAM_ISON = "isOn";
const char* PARAM_MAIS = "mais";
const char* PARAM_MENOS = "menos";


String output26State = "off";
String output27State = "off";

// On/Off state
boolean isOn = true;

// Sensor

//https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-sensor-arduino-ide/

// Led
//usar um led real e nao LED_BUILTIN
int ledPin = 0;




void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("WiFi Failed!\n");
        return;
    }

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", ""
        "<!DOCTYPE html>"
          "<html>"
            "<head>"
              "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
              //"<meta http-equiv=\"refresh\" content=\"5\">" ??????????????????????????????????????
              "<link rel=\"icon\" href=\"data:,\">"
              "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}"
              ".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}"
              ".button2 {background-color: #555555;}"
              "</style>"
            "</head>"
            "<body>"
              "<h1>ESP32 Web Server</h1>"
              "<p>isOn: " + String(isOn? "true":"false") + "</p>"
              "<form action=\"/get\" method=\"get\" >"
                "<input type=\"submit\" name=\"isOn\" value="+String(isOn ? "Desligar" : "Ligar")+" />"
              "</form>"
              "<p>temperaturaSensor: " + String(temperaturaSensor) + "</p>"
              "<form action=\"/get\" method=\"get\" >"
                "<input type=\"hidden\" name=\"menos\" value=\"1\"/>"
                "<input type=\"submit\"  value=\"-\" />"
              "</form>"
              "<p>Temperatura:" + String(temperaturaDesejada) + "<\p>"
              "<form action=\"/get\" method=\"get\" >"
                "<input type=\"hidden\" name=\"mais\" value=\"1\"/>"
                "<input type=\"submit\" value=\"+\" />"
              "</form>"
            "</body>"
        "</html>");
    });

    /*
    // Send a GET request to <IP>/get?message=<message>
    server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String message;
        if (request->hasParam(PARAM_MESSAGE)) {
            message = request->getParam(PARAM_MESSAGE)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, GET: " + message);
    });
    */

    // Send a POST request to <IP>/post with a form field message set to <message>
    server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request){

        String message = "NEW VALUE FOR ";
        String temperatura = "";

        if (request->hasParam(PARAM_ISON, true)) {
          isOn=!isOn;
          message += "isOn: "+isOn;
        }
        if (request->hasParam(PARAM_MENOS, true)) {
          temperaturaDesejada--;
          message += "temperaturaDesejada: "+temperaturaDesejada;
        } 
        if (request->hasParam(PARAM_MAIS, true)) {
          temperaturaDesejada++;
          message += "temperaturaDesejada: "+temperaturaDesejada;
        }

        request->send(200, "text/html", ""
        "<!DOCTYPE html>"
          "<html>"
            "<head>"
              "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
              //"<meta http-equiv=\"refresh\" content=\"5\">" ??????????????????????????????????????
              "<link rel=\"icon\" href=\"data:,\">"
              "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}"
              ".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}"
              ".button2 {background-color: #555555;}"
              "</style>"
            "</head>"
            "<body>"
              "<h1>ESP32 Web Server</h1>"
              "<p>isOn: " + String(isOn? "true":"false") + "</p>"
              "<form action=\"/get\" method=\"get\" >"
                "<input type=\"submit\" name=\"isOn\" value="+String(isOn ? "Desligar" : "Ligar")+" />"
              "</form>"
              "<p>temperaturaSensor: " + String(temperaturaSensor) + "</p>"
              "<form action=\"/get\" method=\"get\" >"
                "<input type=\"hidden\" name=\"menos\" value=\"1\"/>"
                "<input type=\"submit\"  value=\"-\" />"
              "</form>"
              "<p>Temperatura:" + String(temperaturaDesejada) + "<\p>"
              "<form action=\"/get\" method=\"get\" >"
                "<input type=\"hidden\" name=\"mais\" value=\"1\"/>"
                "<input type=\"submit\" value=\"+\" />"
              "</form>"
            "</body>"
        "</html>");

    });

    server.onNotFound(notFound);

    server.begin();
}

void loop() {
  if(isOn){
    //Ler temperatura do sensor
    delay(2000);
    temperaturaSensor = dht.readTemperature();
    Serial.println(temperaturaSensor);
    Serial.println("temperaturaDesejada: " + temperaturaDesejada);
    if(temperaturaDesejada>temperaturaSensor){
      digitalWrite(LED_BUILTIN, HIGH);
    }else{
      digitalWrite(LED_BUILTIN, LOW);
    }
  }else{
    digitalWrite(LED_BUILTIN, LOW);
  }
}

/*
//Index page HTML
const char index_html[] PROGMEM = R"rawliteral(
  <!DOCTYPE html>
          <html>
            <head>
              <meta name="viewport" content="width=device-width, initial-scale=1">
              <link rel="icon" href="data:,">
              <style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}
              .button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}
              .button2 {background-color: #555555;}
              </style>
            </head>
            <body>
              <h1>ESP32 Web Server</h1>
              <p>isOn: " + String(isOn? "true":"false") + "</p>
              <form action="" method="post">
                <input type="submit" name="isOn" value="">" + String(isOn ? "Desligar" : "Ligar") + "<\input>
              </form>
              <p>temperaturaSensor: " + String(temperaturaSensor) + "</p>
              <form action="" method="post">
                //<input type="submit" name="setTemperaturaDesejada" value="(temperaturaDesejada-1)">-<\input>
              </form>
              <p>Temperatura:" + String(temperaturaDesejada) + "<\p>
              <form action="" method="post">
                //<input type="submit" name="setTemperaturaDesejada" value="(temperaturaDesejada+1)">+<\input>
              </form>
            </body>
        </html>)rawliteral";
*/
