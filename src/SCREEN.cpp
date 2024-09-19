#include "DRO.h"
#include "TFT\TFT.h"
#include "CRC\CRC.h"
#include <ESP32_OTA\ESP32_OTA.h>
#include <SoftwareSerial.h>
#include "FRAM\FRAM.h"

const char* ssid = "ESP32_DRO_SCREEN";
const char* password = "12345678";

#ifdef RA8875
    TFT __tft;
#endif
#ifdef SCREEN_ESP32
	TFT _tft;
	SoftwareSerial _SoftUart;	
	//#define Serial ESP32_OTA
#endif
uint8_t _valuesSerial[21]; //XXXX, YYYY, ZZZZ, BBBB, Timer, CRC
uint8_t _CRC, _axe_modif_value, _RPM_count, _Serial_Got; //calcul CRC, axe dont on modifie la valeur, conteur pour RAZ compteur de broche en mode RPM toutes les 10 secondes
ScreenStates _screenState, _screenState_Last;
uint32_t AXE::TimerDRO = 0;
uint32_t AXE::TimerDRO_LAST = 0;
AXE _Axe[5];
int32_t _tempValue, _tempValueLast, _elapsed;
int32_t _posY_Depart, _posBrocheMovY, _pasFiletage = 0, _Y_stop = 1000, _Y_Stop_Old; //
//pour MAJ valeur axe / clavier 
bool _ok,  _firmwareUpdate;
uint8_t addrSizeInBytes = 2; // Default to address size of two bytes
uint32_t memSize;
int v_count;

void m_AfficheXYZ(bool pRefresh, ScreenStates pTypeZ){
	#ifdef DEBUG
		Serial.print("Screenstate :");
		Serial.println(pTypeZ, DEC);
	#endif
	switch (pTypeZ){
		case screen_XYZ:
			_tft.MenuMain(_Axe[axe_X].get(), _Axe[axe_Y].get(), _Axe[axe_Z].get(),
				pRefresh, (uint8_t) pTypeZ, _Y_stop, 99);
			break;
		case screen_RPM:
			#ifdef DEBUG
				Serial.println("demarre rpm");
			#endif
			_tft.MenuMain(_Axe[axe_X].get(), _Axe[axe_Y].get(), _Axe[axe_BROCHE].get_RPM(),
				pRefresh, (uint8_t) pTypeZ, _Y_stop, 99);				
			break;	
		case screen_Filetage:
			//_tft.MenuMain(_Axe[axe_X].get(), _Axe[axe_Y].get(), _Axe[axe_Z].get(),
			//	pRefresh, (uint8_t) pTypeZ);
			m_Filetage(20000);
			break;
		case screen_Position_Broche:
			_tft.MenuMain(_Axe[axe_X].get(), _Axe[axe_Y].get(), _Axe[axe_BROCHE].get_ANG(),
				pRefresh, (uint8_t) pTypeZ, _Y_stop, 99);
			break;
	}
}

//Passe à ecran d'ajustage de valeur
void m_initValue(uint8_t pAxe){
	_screenState_Last = _screenState;
	_screenState = screen_value_Axe;
	_ok = false;
	_axe_modif_value = pAxe;
	_tempValue = 0;
	_tempValueLast = 0;
	_Axe[axe_X].modif(_Axe[axe_X].get());
	_Axe[axe_Y].modif(_Axe[axe_Y].get());
	_Axe[axe_Z].modif(_Axe[axe_Z].get());
	_Y_Stop_Old = _Y_stop;
	_Y_stop = 0;
	_tft.MenuValue(pAxe);
}

void m_Filetage(int16_t pTempo){
	//demarre un filetage
	//calcule d'abord l'avance selectionnée 
	//demande si ok
	uint32_t _start = millis();
	int8_t _reponse = -1;
	int16_t _debugY = -1000, _debugB = 0;
	_tft.MenuFiletage(0, true, 0, 0, 0);
	delay(250);
	_SoftUart.write('A'); //reset position broche
	_posY_Depart = _Axe[axe_Y].get();
	_posBrocheMovY = 0;
	_elapsed = millis();
	while (_reponse != OK){
		if ((m_Get_Serial(false)) | (millis() - _elapsed > 1000)){
			_reponse = _tft.MenuFiletage(0, false, 0, _Axe[axe_Y].get() - _posY_Depart + _debugY, abs(_Axe[axe_BROCHE].get()));
			_elapsed = millis();
		}
		if ((_reponse != EN_COURS) && (millis() - _start) > pTempo){
			break;
		}
		if (_reponse == ANNULER){
			break;
		}
		delay(10);
		_debugY += 10;
	}
	if (_reponse != OK){
		_screenState = _screenState_Last;
		m_AfficheXYZ(true, _screenState);
		delay(500);
		m_videSerial();
		return;
	}
	//Lance la surveillance du filetage
	_reponse = _tft.MenuFiletage(1, true, _Axe[axe_X].get(), _Axe[axe_Y].get() - _posY_Depart + _debugY,  abs(_Axe[axe_BROCHE].get()));
	delay(250);
	while (_reponse != ANNULER){
		if (m_Get_Serial(false)){
			_reponse = _tft.MenuFiletage(1, false, _Axe[axe_X].get(), _Axe[axe_Y].get() - _posY_Depart + _debugY, abs(_Axe[axe_BROCHE].get()) + _debugB );
			_debugB += 10;
			_debugY += 10;
		}
	}
	_screenState = _screenState_Last;
	m_AfficheXYZ(true, _screenState);
	delay(500);
	m_videSerial();
}

void m_videSerial(){
	while (_SoftUart.available()){
		_SoftUart.read();
	}
}

bool m_Get_Serial(bool pAfficheXYZ){
	if (_SoftUart.available()){
		Serial.println("test serial");
		_Serial_Got = 0;
		while (_SoftUart.available()){
			_valuesSerial[_Serial_Got] =  _SoftUart.read();
			_Serial_Got++;
			if (_Serial_Got>21){
				Serial.print(" serial > 21 ");
				m_videSerial();
				return false;
			}
		}		
		_CRC = crc8(_valuesSerial, 20, 0x07, 0, 0, false, false);
		if (_CRC == _valuesSerial[20]){	
			_Axe[axe_X].set((_valuesSerial[0] << 24) + (_valuesSerial[1] << 16) + (_valuesSerial[2] << 8) + _valuesSerial[3]);
			_Axe[axe_Y].set((_valuesSerial[4] << 24) + (_valuesSerial[5] << 16) + (_valuesSerial[6] << 8) + _valuesSerial[7]);
			_Axe[axe_Z].set((_valuesSerial[8] << 24) + (_valuesSerial[9] << 16) + (_valuesSerial[10] << 8) + _valuesSerial[11]);
			_Axe[axe_BROCHE].set((_valuesSerial[12] << 24) + (_valuesSerial[13] << 16) + (_valuesSerial[14] << 8) + _valuesSerial[15]);
			AXE::TimerDRO = ((_valuesSerial[16] << 24) + (_valuesSerial[17] << 16) + (_valuesSerial[18] << 8) + _valuesSerial[19]);
			if (pAfficheXYZ){
				m_AfficheXYZ(false, _screenState);
			}		
			m_videSerial();
			return true;
		}
	}
    m_videSerial();
	return false;
}

void m_FirmwareUpdate(bool pEnd){
	if(!pEnd){
			if (!_firmwareUpdate){
			_tft.clr(1);
			_tft.printTXT("Firmware Update ", 10, 200, RA8875_GREEN, RA8875_BLACK, 2);
			_firmwareUpdate = true;
			return;
			}
#ifdef DEBUG
		//_tft.print(".");
#endif
#ifdef RA8875
		TOGGLE(PIN_LED);
#endif
	}
	else{
		_tft.printTXT("REBOOTING", 10, 280, RA8875_BLACK, RA8875_GREEN, 2);
	}
}

void m_ModifValue(int32_t *pValue, String pButtonName){

}

ButtonNames _touch;
void m_loop_screen(){
	switch (_screenState){
		//teste appui ecran
		case screen_XYZ:
		case screen_RPM:
		case screen_Filetage:
		case screen_Position_Broche:{
			m_Get_Serial(true);
			//Teste appui ecran
			_touch = _tft.IfTouched(_screenState, 100);
			switch (_touch){
				case bnCX: //C X
					_Axe[axe_X].reset();
					m_AfficheXYZ(false, _screenState);
					delay(500);
					break;
				case bnCY: //C Y
					_Axe[axe_Y].reset();
					m_AfficheXYZ(false, _screenState);
					delay(500);
					break;
				case bnStopY  : // STOP Y
					m_initValue(axe_STOP_Y);
					break;
				case bnCZ: //C Z
					_Axe[axe_Z].reset();
					m_AfficheXYZ(false, _screenState);
					delay(500);
					break;
				case bnCAngle:
					_Axe[axe_BROCHE].reset();
					m_AfficheXYZ(false, _screenState);
					delay(500);
					break;
				case bnX: //Value X
					delay(250);
					m_initValue(axe_X);
					break;
				case bnY: //Value Y
					delay(250);
					m_initValue(axe_Y);
					break;
				case bnZ: //Value Z
					delay(250);
					if (_screenState == screen_XYZ){
						m_initValue(axe_Z);
					}
					break;	
				case bnXYZ: //XYZ
					_screenState = screen_XYZ;
					m_AfficheXYZ(true, _screenState);
					delay(500);
					break;
				case bnRPM: //RPM
					_SoftUart.write('z');
					_screenState = screen_RPM;
					m_AfficheXYZ(true, _screenState);
					delay(500);
					break;
				case bnFILETAGE: //FIL
					_screenState_Last = _screenState;
					_screenState = screen_Filetage;
					m_AfficheXYZ(true, _screenState);
					delay(500);
					break;
				case bnANGLE: //ANG
					_screenState = screen_Position_Broche;
					m_AfficheXYZ(true, _screenState);
					delay(500);
					break;		
				default:
					break;
			}
			break;
		}
		//Modifie les valeurs de l'axe
		case screen_value_Axe:{
			if (!_ok){
				_touch = _tft.IfTouched(_screenState, 100);
				switch (_touch){
					case bn0: //0
						_tempValue *= 10;
						break;
					case bn1: //1
						_tempValue *= 10;
						_tempValue += 1; 
						break;	
					case bn2: //2
						_tempValue *= 10;
						_tempValue += 2; 
						break;
					case bn3: //3
						_tempValue *= 10;
						_tempValue += 3; 
						break;
					case bn4: //4
						_tempValue *= 10;
						_tempValue += 4; 
						break;
					case bn5: //5
						_tempValue *= 10;
						_tempValue += 5; 
						break;
					case bn6: //6
						_tempValue *= 10;
						_tempValue += 6; 
						break;
					case bn7: //7
						_tempValue *= 10;
						_tempValue += 7; 
						break;
					case bn8: //8
						_tempValue *= 10;
						_tempValue += 8; 
						break;
					case bn9: //9
						_tempValue *= 10;
						_tempValue += 9; 
						break;
					case bnpm: //+/-
						_tempValue = (-_tempValue);
						break;
					case bnm: // <
						_tempValue /= 10;
						if (_tempValue == 0){
							_ok = true;
						}
						break;				
					case bnp: // >
						_ok = true;
					default:
						break;
				}
				if (_tempValue > 99999){
					_ok = true;
				}
				if((_tempValue != _tempValueLast) & !_ok){
					_tempValueLast = _tempValue;
					if (_axe_modif_value < 3){
						_Axe[_axe_modif_value].modif(_tempValue);
						_tft.MenuMain(_Axe[axe_X].get_modif(), _Axe[axe_Y].get_modif(), _Axe[axe_Z].get_modif(), false, MODE_TOUR, _Y_Stop_Old, _axe_modif_value);
					}
					else{
						_tft.MenuMain(_Axe[axe_X].get_modif(), _Axe[axe_Y].get_modif(), _Axe[axe_Z].get_modif(), false, MODE_TOUR, _tempValue, _axe_modif_value);
					}
					delay(250);
				}
			}
			else{
				if (_tempValue != 0){
					if (_axe_modif_value < 3){
						_Axe[_axe_modif_value].corrige(_tempValue);
						_Y_stop = _Y_Stop_Old;
					}
					else{
						_Y_stop = _tempValue;
					}
				}
				else{
					_Y_stop = _Y_Stop_Old;
				}
				_screenState = _screenState_Last;
				m_AfficheXYZ(true, _screenState);
				delay(200);
				m_videSerial();
			}
		break;
		}
	}
	if (_tft.ButtonLast != 0){
		byte _index = 0;
		while ((_tft.Buttons[(byte)_screenState][_index] != 0) & (_index < sizeof(_tft.Buttons[(byte)_screenState]))){
			if (_tft.ButtonLast == _tft.Buttons[(byte)_screenState][_index]){
						_tft.DessineButton(*_tft.ButtonLast, false);
			}
		_index++;
		}
		_tft.ButtonLast = 0;
	}
}

void setup(void) {
	Serial.begin(115200);
	_SoftUart.begin(57600, SWSERIAL_8N1, GPIO_NUM_18, GPIO_NUM_17, false);
	ESP32_OTA.begin(ssid, password, 250, m_FirmwareUpdate);
	_elapsed = millis();
	
#ifdef RA8875
	pinMode(PIN_LED, OUTPUT);
	digitalWrite(PIN_LED, HIGH);
#endif
	delay(250);
	_tft.Init();
	delay(500);
	for (size_t i = 0; i < 20; i++)
	{
		_valuesSerial[i] = 0;
	}
	//Test FRAM
	SPI.begin();
	Memory_Start();
	delay(10);
	if (Memory_ReadULong(0)!=12345678){
		Memory_WriteULong(0, 12345678);
		_tft.printTXT("PROBLEME D\'ACCES A LA FRAM !!",50,180,RED, BLACK,3);
		_tft.printTXT("Appuyez sur l'ecran pour redemarrer",50,250,WHITE, BLACK,2);
		while (!_tft.Touched()){delay(5);}
		ESP.restart();
	}
	else{
		_tft.printTXT("FRAM OK !!",100,180,GREEN, BLACK, 3);
		_tft.printTXT("Appuyez sur l'ecran" ,100, 250,WHITE, BLACK,2);
		while (!_tft.Touched()){delay(5);}
	}
	_Axe[axe_X].init(axe_X, 5);
	_Axe[axe_Y].init(axe_Y, 5);
	_Axe[axe_Z].init(axe_Z, 5);
	_Axe[axe_STOP_Y].init(axe_STOP_Y, 1);
	_Axe[axe_BROCHE].init(axe_BROCHE, 1);
	_tft.Buttons_Init();
	_tft.MenuMain(0, 0, 0, true, screen_XYZ, _Y_stop, 99);
	_screenState = screen_XYZ;// (ScreenStates) MODE_TOUR;		
	_RPM_count = 0;
}

void loop(void) {
	ESP32_OTA.handleClient();
	AXE::TimerDRO = millis();
	
	m_loop_screen();
	
	//Toggle LED + RAZ Broche si RPM
	if (millis() - _elapsed > 1000){
#ifdef RA8875
		TOGGLE(PIN_LED);
#endif
		_elapsed = millis();
		if ((_screenState == screen_RPM) & (_RPM_count > 9)){
			_SoftUart.write('A');
			_RPM_count = 0;
		}
		_RPM_count ++;
		Serial.println("Hello");
	}
/*
	if (Serial.available()){
		String _test = Serial.readString();
		if (_test = "REBOOT"){
			ESP.restart();
		} 
	}
*/	
	delay(10);
}

