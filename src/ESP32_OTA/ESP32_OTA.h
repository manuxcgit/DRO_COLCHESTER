#ifndef ESP32_OTA_h

  #define ESP32_OTA_h
  #include "WebServer.h"
  #include "WEBSOCKET\WebSocketsServer.h"
  #include "Update.h"

  class ESP32_OTA_class{
    public:
      ESP32_OTA_class();
      void begin(const char* pSSID, const char* pPASS, int16_t pInterval, void (*pFunction)(bool pEnd));
      void update();
      void handleClient();
    private:
      int16_t _interval = 1000; 
      void manage_upload();
      void manage_upload_ended();
      void (*functionPointer)(bool pEnd);
  };
  extern ESP32_OTA_class ESP32_OTA;
#endif
