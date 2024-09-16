#include <ESP32_OTA\ESP32_OTA.h>
#include "DRO.h"
#include "LS7366/LS7366.h"
#include "CRC/CRC.h"


const char* ssid = "ESP32_DRO_READ";
const char* password = "12345678";
LS7366 _LS[4];
uint8_t _VALUE[21], _CRC, _received;
uint16_t _Loop_Value;
uint32_t *_P_value, _value, _elapsed, _elapsedTest, _nbrSent;
String _Serial_Received;

void m_FirmwareUpdate(bool pEnd){
    TOGGLE(PIN_LED);
}

void setup(void){
	Serial.begin(57600);
	ESP32_OTA.begin(ssid, password, 250, m_FirmwareUpdate);
	_LS[axe_X].init(PIN_CS_X, QUADRX4);
	_LS[axe_Y].init(PIN_CS_Y, QUADRX4);
	_LS[axe_Z].init(PIN_CS_Z, QUADRX4);
	_LS[axe_BROCHE].init(PIN_CS_BROCHE, NQUAD);
    pinMode(PIN_LED, OUTPUT);
	_Loop_Value = 250;
	_nbrSent = 0;
}

void loop(void) {

	ESP32_OTA.handleClient();

	//Lit et transmet valeurs
	if ((millis() - _elapsed) > _Loop_Value){
		TOGGLE(PIN_LED);		
		for (int i = 0 ; i<4; i++){
			_value = (int32_t)_LS[i].read_counter();
			_VALUE[(i * 4)] = (_value & 0xff000000) >> 24;
			_VALUE[(i * 4) + 1] = (_value & 0xff0000) >> 16;
			_VALUE[(i * 4) + 2] = (_value & 0xff00) >> 8;
			_VALUE[(i * 4) + 3] = (_value & 0xff);			
		}
		_elapsed = millis();
		_VALUE[16] =(_elapsed & 0xff000000) >> 24;
		_VALUE[17] = (_elapsed & 0xff0000) >> 16;
 		_VALUE[18] = (_elapsed & 0xff00) >> 8;
		_VALUE[19] = (-_elapsed & 0xff);
		_CRC = crc8(_VALUE, 20, 0x07, 0, 0, false, false);
		_VALUE[20] = _CRC;
		
		for (int i=0; i<21; i++){
			Serial.write(_VALUE[i]);
		}	
		_elapsed = millis();
	}

	//Lit messages depuis SCREEN
	while (Serial.available()){
		// "Cn" = RAZ compteur 'axe'
		_received = Serial.read();
		if (_received != '\n'){
			_Serial_Received += (char)_received;
		}	
		else{
			switch ( _Serial_Received[0])
			{
			case 'C':
				byte _compteur = (byte)_Serial_Received[1] - '0';
				if ((_compteur>=0) & (_compteur<4)){
					_LS[_compteur].clear_counter();
				}
				break;			
			default:
				break;
			}
		}	
		
	}

	delay(10);
}
