#include <Arduino.h>
#include <ESP32_OTA\ESP32_OTA.h>
#include <SPI.h>
#include "FRAM\FRAM.h"
#include "FRAM\TEST_FRAME.h"


#define TOGGLE(x) digitalWrite(x, !digitalRead(x))

const char* ssid = "ESP32_DRO_TEST_FRAM";
const char* password = "12345678";
long _elapsed;
String _received, _temp;

void m_FirmwareUpdate(bool pEnd){
}

String strs[20];

int split(String str ){
    int StringCount = 0;
    int _count = 0;
  // Split the string into substrings
  while (str.length() > 0)
  {
    int index = str.indexOf('\n');
    if (index == -1) // No space found
    {
      strs[StringCount++] = str;
      break;
    }
    else
    {
      strs[StringCount++] = str.substring(0, index);
      str = str.substring(index+1);
      _count ++;
    }
  }
  return _count;
}

void setup(){
	Serial.begin(115200);
	ESP32_OTA.begin(ssid, password, 250, m_FirmwareUpdate, customHtml);
  /*  pinMode(FRAM_HOLD, OUTPUT);
    pinMode(FRAM_WRITE, OUTPUT);
    digitalWrite(FRAM_HOLD, LOW);
    digitalWrite(FRAM_WRITE, LOW);
    */
	_elapsed = millis();
    SPI.begin();
    Memory_Start();
}
unsigned long _value = 0;
void loop(){
    ESP32_OTA.handleClient();
    _temp = ESP32_OTA.getReceived();
    if (_temp != ""){
        _received += _temp + "\r\n";
    }
    if (millis() - _elapsed > 500){
       //fram.begin();
     //  Serial.print("received :>");
     //  Serial.print(_received);
     //  Serial.println("<");
       
        if (_received != ""){
            int _number = split(_received);
            for (int n = 0; n < _number; n++){
                Serial.print("SPLIT :");
                Serial.println(strs[n]);
                /*
                if (String(strs[n]).endsWith("Hold")){
                    TOGGLE(FRAM_HOLD);
                    if (digitalRead(FRAM_HOLD) == HIGH){
                        ESP32_OTA.print("1");
                    }
                    else{
                        ESP32_OTA.print("0");
                    }
                }
                if (String(strs[n]).endsWith("Write")){
                    TOGGLE(FRAM_WRITE);
                    if (digitalRead(FRAM_WRITE) == HIGH){
                        ESP32_OTA.print("3");
                    }
                    else{
                        ESP32_OTA.print("2");
                    }
                }
                */
                if (String(strs[n]).endsWith("TEST")){
                    //fram.begin();
                    Serial.print("READ:");
                    Serial.println(Memory_ReadULong(10), DEC);
                    _value = random();
                    Memory_WriteULong( 10,_value);
                    Serial.print("WRITE:");
                    Serial.println(_value, DEC);
                }
            }
        }
        _received = "";
       ESP32_OTA.println("ok");
      _elapsed = millis();
    }
    delay(10);
}