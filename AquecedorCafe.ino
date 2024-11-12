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

AsyncWebServer server(80);

// Wifi
const char* ssid = "";
const char* password = "";

// HTTP params
const char* PARAM_MESSAGE = "message";
const char* PARAM_ISON = "isOn";
const char* PARAM_SETTEMPERATURADESEJADA = "setTemperaturaDesejada";

String output26State = "off";
String output27State = "off";

// On/Off state
boolean isOn = false;

// Sensor
//https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-sensor-arduino-ide/

// Led
//usar um led real e nao LED_BUILTIN
int ledPin = 0;

// Temperatura
int temperaturaDesejada = 0;
// Ler do sensor a temperatura inicial
int temperaturaSensor = 0;


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
              "<link rel=\"icon\" href=\"data:,\">"
              "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}"
              ".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}"
              ".button2 {background-color: #555555;}"
              "</style>"
            "</head>"
            "<body>"
              "<h1>ESP32 Web Server</h1>"
              "<p>isOn: " + isOn + "</p>"
              "<form action=\"\" method=\"post\">"
                "<input type=\"submit\" name=\"isOn\" value="!isOn">" + isOn + "<\input>"
              "</form>"
              "<p>temperaturaSensor: " + temperaturaSensor + "</p>"
              "<form action=\"\" method=\"post\">"
                "<input type=\"submit\" name=\"setTemperaturaDesejada\" value="temperaturaDesejada-1">-<\input>"
              "</form>"
              "<p>Temperatura:" + temperaturaDesejada + "<\p>"
              "<form action=\"\" method=\"post\">"
                "<input type=\"submit\" name=\"setTemperaturaDesejada\" value="temperaturaDesejada+1">+<\input>"
              "</form>"
            "</body>"
        "</html>");
    });

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

    // Send a POST request to <IP>/post with a form field message set to <message>
    server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
        String message = "";
        String temperatura = "";
        // if (request->hasParam(PARAM_MESSAGE, true)) {
        //     message = request->getParam(PARAM_MESSAGE, true)->value();
        // }
        if (request->hasParam(PARAM_ISON, true)) {
          isOn=!isOn;
        }
        if (request->hasParam(PARAM_SETTEMPERATURADESEJADA, true)) {
          temperatura = request->getParam(PARAM_SETTEMPERATURADESEJADA, true)->value();
          temperaturaDesejada = temperatura.toInt();
        } 
        // else {
        //     message = "No message sent";
        // }
        request->send(200, "text/plain", "Hello, POST: Temperatura: " + temperatura + " isOn: " + !isOn);
    });

    server.onNotFound(notFound);

    server.begin();
}

void loop() {
  if(isOn){
    //Ler temperatura do sensor
    Serial.println("temperaturaSensor: " + temperaturaSensor);
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
