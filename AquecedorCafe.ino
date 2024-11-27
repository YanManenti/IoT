//
// A simple server implementation showing how to:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>

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
const char* PARAM_TEMPERATURA = "temperatura";


String output26State = "off";
String output27State = "off";

// On/Off state
boolean isOn = true;

// Sensor
float temperaturaSensor = 0;
float temperaturaDesejada = 25.20;

//https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-sensor-arduino-ide/

// Led
//usar um led real e nao LED_BUILTIN
#define LED 2




void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void setup() {
    Serial.begin(115200);
    pinMode(LED,OUTPUT);
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
              "._frameEstado {border: none;height: 45px;width: 100px;padding-left:28px;}"
              "._frame {border: none;height: 45px;width: 60px;padding-left:5px;}"
              ".temperaturaInput {padding-right: 10px;}"
              "body {display: flex; flex-direction:column;gap:10px;align-items:center;}"
              "</style>"
            "</head>"
            "<body>"
              "<h1>Controle Aquecedor de Cafe</h1>"
                "<p>Estado: </p>"
              "<iframe class=\"_frameEstado\" name=\"onoff\">"
              "</iframe>"
              "<form  action=\"/get\" method=\"get\" target=\"onoff\" >"
                "<input type=\"submit\" name=\"isOn\" value=\"Ligar\\Desligar\" />"
              "</form>"
              //"<p>"+String(temperaturaSensor)+"</p>"
              "<p>Temperatura Desejada:</p>"
              "<iframe class=\"_frame\" name=\"temp\">"
              "</iframe>"
              "<form action=\"/get\" method=\"get\" target=\"temp\" >"
                "<input class=\"temperaturaInput\" type=\"number\" name=\"temperatura\"/>"
                "<input type=\"submit\" value=\"Aplicar\" />"
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

        String message = "";
        
        if (request->hasParam(PARAM_ISON)) {
          isOn=!isOn;
          message += isOn ? "Ligado" : "Desligado";
        }
        if (request->hasParam(PARAM_TEMPERATURA)) {
          temperaturaDesejada = request->getParam(PARAM_TEMPERATURA)->value().toInt();
          message += String(temperaturaDesejada);
        }

        request->send(200, "text/plain", message);
    });
    server.onNotFound(notFound);

    server.begin();
}

void loop() {
  if(isOn){
    //Ler temperatura do sensor
    delay(2000);
    temperaturaSensor = dht.readTemperature();
    Serial.println(String(temperaturaSensor));
    Serial.println("temperaturaDesejada: " + String(temperaturaDesejada));
    if(temperaturaDesejada>temperaturaSensor){
      digitalWrite(LED, HIGH);
    }else{
      digitalWrite(LED, LOW);
    }
  }else{
    digitalWrite(LED, LOW);
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
