#include "ESP32_OTA.h"
#include "index_html.h"

//Class constructor

WebServer _server(80);
WebSocketsServer _socket(81);

ESP32_OTA_class::ESP32_OTA_class(){}


long _elapsed_time, _uploaded;

void ESP32_OTA_class::manage_upload(){
  HTTPUpload& upload = _server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      functionPointer(false);
      _elapsed_time = millis();
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } 
    else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (millis() - _elapsed_time > _interval)
      {
        Serial.print(".");
        char _text[20];
        sprintf(_text, "%d", upload.totalSize);
        _socket.sendTXT(0, _text);
        _socket.loop();
        functionPointer(false);
        _elapsed_time = millis();
      }
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }  
    } 
    else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        functionPointer(true);
        _socket.sendTXT(0, "Upload OK");
        _socket.loop();
        delay(1000);
        _socket.loop();
        _elapsed_time = millis();
        while ((_socket.connectedClients() > 0) & (millis() - _elapsed_time < 5000)){
            Serial.print("*");
            delay(250);
        }
        Serial.println();
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
}

void ESP32_OTA_class::manage_upload_ended(){
  _server.sendHeader("Connection", "close");
  _server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
  ESP.restart();
}

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    // Figure out the type of WebSocket event
    switch(type) {

      // Client has disconnected
      case WStype_DISCONNECTED:
        Serial.printf("[%u] Disconnected!\n", num);
        break;

      // New client has connected
      case WStype_CONNECTED:
        {
          IPAddress ip = _socket.remoteIP(num);
          Serial.printf("[%u] Connection from ", num);
          Serial.println(ip.toString());
        }
        break;

      // Echo text message back to client
      case WStype_TEXT:
        Serial.printf("[%u] Text: %s\n", num, payload);
        _socket.sendTXT(num, payload);
        break;

      // For everything else: do nothing
      case WStype_BIN:
      case WStype_ERROR:
      case WStype_FRAGMENT_TEXT_START:
      case WStype_FRAGMENT_BIN_START:
      case WStype_FRAGMENT:
      case WStype_FRAGMENT_FIN:
      default:
        break;
    }
  }

void ESP32_OTA_class::begin(const char* pSSID, const char* pPASS, int16_t pInterval, void (*pFunction)(bool pEnd)){//} WebServer *server){
  functionPointer = pFunction;
  _interval = pInterval;
  WiFi.softAP(pSSID, pPASS);
  delay(1000);

  _server.on("/update", HTTP_GET, [&]() {
    _server.sendHeader("Connection", "close");
    _server.send(200, "text/html", indexHtml);
  });
  /*handling uploading firmware file */
  _server.on("/update", HTTP_POST, [&]() {
    manage_upload_ended();
  }, [&]() {
    manage_upload();
  });

  _server.begin();
  _socket.begin();
  _socket.onEvent(onWebSocketEvent);
}

void ESP32_OTA_class::handleClient(){
  _server.handleClient();
  _socket.loop();
}

void ESP32_OTA_class::update(){
   char _text[20];
    sprintf(_text, "Manu %d", millis());
  _socket.sendTXT(0, _text);
}

ESP32_OTA_class ESP32_OTA;
