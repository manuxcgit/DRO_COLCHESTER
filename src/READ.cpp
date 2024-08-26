#include "ESP32_OTA\ESP32_OTA.h"
#include "DRO.h"
#include "LS7366/LS7366.h"
#include "CRC/CRC.h"


const char* ssid = "ESP32_DRO_READ";
const char* password = "12345678";
LS7366 _LS[4];
uint8_t _VALUE[21], _CRC, _received;
uint16_t _Loop_Value;
uint32_t _value, _elapsed;
String _Serial_Received;

void m_FirmwareUpdate(bool pEnd){
    TOGGLE(PIN_LED);
}


void setup(void){
	Serial.begin(115200);
	ESP32_OTA.begin(ssid, password, 250, m_FirmwareUpdate);
	_LS[axe_X].init(PIN_CS_X);
	_LS[axe_Y].init(PIN_CS_Y);
	_LS[axe_Z].init(PIN_CS_Z);
	_LS[axe_BROCHE].init(PIN_CS_BROCHE);
    pinMode(PIN_LED, OUTPUT);
	_Loop_Value = 250;
}

void loop(void) {
	ESP32_OTA.handleClient();
	//Lit et transmet valeurs
	if ((millis() - _elapsed) > _Loop_Value){
		TOGGLE(PIN_LED);
		for (int i = 0 ; i<4; i++){
			_value = (int32_t)_LS[i].read_counter();
			Serial.println(_value, DEC);
			_VALUE[(i * 4)] = *(&_value);
			_VALUE[(i * 4) + 1] = *(&_value + 1);
			_VALUE[(i * 4) + 2] = *(&_value + 2);
			_VALUE[(i * 4) + 3] = *(&_value + 3);
		}
		_elapsed = millis();
		_VALUE[16] = *(&_elapsed);
		_VALUE[17] = *(&_elapsed + 1);
		_VALUE[18] = *(&_elapsed + 2);
		_VALUE[19] = *(&_elapsed + 3);
		_CRC = crc8(_VALUE, 20, 0x07, 0, 0, false, false);
		_VALUE[20] = _CRC;
		Serial.println("................");
		_elapsed = millis();
	}
	//Lit messages depuis SCREEN
	while (Serial.available()){
		_received = Serial.read();
		if (_received != '\n'){
			_Serial_Received += (char)_received;
		}	
		else{
			Serial.print("Received:");
			Serial.println(_Serial_Received);
			_Serial_Received = "";
		}	
	}
	delay(10);
}
