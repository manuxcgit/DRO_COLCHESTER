#include "TFT.h"

#define display __tft

#ifdef SCREEN_ESP32
	#include <Arduino_GFX_Library.h>

	Arduino_RPi_DPI_RGBPanel_extended::Arduino_RPi_DPI_RGBPanel_extended( 
		Arduino_ESP32RGBPanel *bus,
		int16_t w, uint16_t hsync_polarity, uint16_t hsync_front_porch, uint16_t hsync_pulse_width, uint16_t hsync_back_porch,
		int16_t h, uint16_t vsync_polarity, uint16_t vsync_front_porch, uint16_t vsync_pulse_width, uint16_t vsync_back_porch,
		uint16_t pclk_active_neg, int32_t prefer_speed , bool auto_flush) : 
	  	Arduino_RPi_DPI_RGBPanel(
				bus,
                w,  hsync_polarity,  hsync_front_porch,  hsync_pulse_width,  hsync_back_porch,
                h,  vsync_polarity, vsync_front_porch, vsync_pulse_width, vsync_back_porch,
            	pclk_active_neg , prefer_speed , true)
		{}
	
	void Arduino_RPi_DPI_RGBPanel_extended::touchInit(){
		Wire.begin(TOUCH_GT911_SDA, TOUCH_GT911_SCL);
		_ts.begin();
		_ts.setRotation(TOUCH_GT911_ROTATION);
	}

	void Arduino_RPi_DPI_RGBPanel_extended::touchGet(Point *pPoint){
		_ts.read();
		if (_ts.isTouched){
			#if defined(TOUCH_SWAP_XY)
				_ts.touchX = map(_ts.points[0].y, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, gfx->width() - 1);
				_ts.touchY = map(_ts.points[0].x, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, gfx->height() - 1);
			#else
				_ts.touchXY.x = map(_ts.points[0].x, TOUCH_MAP_X1, TOUCH_MAP_X2, 0, 799);
				_ts.touchXY.y = map(_ts.points[0].y, TOUCH_MAP_Y1, TOUCH_MAP_Y2, 0, 479);
			#endif
			*pPoint = Point(_ts.touchXY.x, _ts.touchXY.y);
			}
  		else{
			*pPoint = Point(0,0);
			}
	}

	void Arduino_RPi_DPI_RGBPanel_extended::graphicsMode() {}
    void Arduino_RPi_DPI_RGBPanel_extended::textMode() {}
    void Arduino_RPi_DPI_RGBPanel_extended::textEnlarge(int pSize) { setTextSize(pSize); }
    void Arduino_RPi_DPI_RGBPanel_extended::textSetCursor(uint16 pX, uint16 pY) { setCursor( pX, pY); }
				
    void LCD_CmdWrite(uint8_t p){}
    void LCD_DataWrite(uint8_t p){}
    int LCD_DataRead() {return 0;}
    int LCD_StatusRead() {return 0;}
#endif

void TFT::Active_Window(uint16_t XL, uint16_t XR, uint16_t YT, uint16_t YB)
{
	//setting active window X

	LCD_CmdWrite(0x30);//HSAW0
	LCD_DataWrite(XL);
	LCD_CmdWrite(0x31);//HSAW1	   
	LCD_DataWrite(XL >> 8);

	LCD_CmdWrite(0x34);//HEAW0
	LCD_DataWrite(XR);
	LCD_CmdWrite(0x35);//HEAW1	   
	LCD_DataWrite(XR >> 8);

	//setting active window Y

	LCD_CmdWrite(0x32);//VSAW0
	LCD_DataWrite(YT);
	LCD_CmdWrite(0x33);//VSAW1	   
	LCD_DataWrite(YT >> 8);

	LCD_CmdWrite(0x36);//VEAW0
	LCD_DataWrite(YB);
	LCD_CmdWrite(0x37);//VEAW1	   
	LCD_DataWrite(YB >> 8);
}

void TFT::Clear_Active_Window(void)
{
	uchar temp;
	LCD_CmdWrite(0x8e);//MCLR
	temp = LCD_DataRead();
	temp |= cSetD6;
	LCD_DataWrite(temp);
}

static int32 _oldX, _oldY, _oldZ, _old_Y_Stop;
//conversion de int -> txt
char result[8] ;
///////////////////////////////////////////
/// REGION PUBLIC
///////////////////////////////////////////
bool TFT::Init(){
	bool _found = false;
	Serial.println("TFT start");
#ifdef RA8875	
	if (!__tft.begin(RA8875_800x480)) {
		Serial.println("TFT Not Found!");
	}
	else{
		Serial.println("Found TFT");	
		_found = true;
	}

	__tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
	__tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
	__tft.PWM1out(255);
	Active_Window(0, 799, 0, 479);
	Clear_Active_Window();

	Memory_Clear();
	__tft.displayOn(true);
	__tft.fillScreen(0);
	__tft.touchEnable(true);
	_oldX = _oldY = _oldZ = _old_Y_Stop = 0;
	return _found;
#endif
#ifdef SCREEN_ESP32
	__tft.begin();
#ifdef TFT_BL
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
#endif
	__tft.touchInit();
	return true;
#endif
}

void TFT::Buttons_Init(ScreenStates pScreenState, byte pNBr, cBUTTON *pButton, ...){
	va_list _button;
	va_start (_button, pButton);
	byte _index = 1;
	cBUTTON *b;

#ifdef DEBUG
	Serial.print("Screenstate ");
	Serial.println((byte) pScreenState, DEC);
	Serial.print("pNbr ");
	Serial.println(pNBr, DEC);
#endif

	Buttons[(byte) pScreenState][0] = pButton;
#ifdef DEBUG
	Serial.println(pButton->name);
#endif
	while(_index < pNBr)
	{
		b = va_arg(_button, cBUTTON*);
#ifdef DEBUG
		Serial.println((long)b);
#endif		
		Buttons[(byte) pScreenState][_index] = b;
#ifdef DEBUG
		Serial.print("Nom :");
		Serial.println(b->name);
#endif
		_index ++;
#ifdef DEBUG
	Serial.print("_index ");
	Serial.println(_index, DEC);
#endif
	}
#ifdef DEBUG
	Serial.print("Nbr buttons ");
	Serial.println(_index, DEC);
	Serial.print("Nbr total ");
#endif
	while(_index < (sizeof(Buttons[(byte)pScreenState]) / 4)){	
		Buttons[(byte) pScreenState][_index] = 0;// nullptr;
		_index++;
	}
	va_end(_button); // r
#ifdef DEBUG
	Serial.println(_index, DEC);
#endif
}

void TFT::Buttons_Init(){
#ifdef DEBUG
	Serial.println("Buttons Init");
#endif
	button_bCX = cBUTTON(Point(580, 60) , 100, 90, 7, bnCX, "C", WHITE, RED, m_ModifValue);
	button_bCY = cBUTTON(Point(580, 190) , 100, 90, 7, bnCY, "C", WHITE, RED, m_ModifValue);
	button_bCZ = cBUTTON(Point(580, 320) , 100, 90, 7, bnCZ, "C", WHITE, RED, m_ModifValue);
	button_bVX = cBUTTON(Point(720, 60) , 100, 90, 7, bnX, "X", WHITE, GREEN, m_ModifValue);
	button_bVY = cBUTTON(Point(720, 190) , 100, 90, 7, bnY, "Y", WHITE, GREEN, m_ModifValue);
	button_bVZ = cBUTTON(Point(720, 320) , 100, 90, 7, bnZ, "Z", WHITE, GREEN, m_ModifValue);
	button_bXYZ = cBUTTON(Point(100, 420) , 150, 80, 5, bnXYZ, "XYZ", BLACK, BLUE, m_ModifValue);
	button_bRPM = cBUTTON(Point(290, 420) , 150, 80, 5, bnRPM, "RPM", BLACK, BLUE, m_ModifValue);
	button_bCANGLE = cBUTTON(Point(580, 320) , 100, 90, 7, bnCAngle, "C", WHITE, RED, m_ModifValue);
	button_bFILETAGE = cBUTTON(Point(500, 420) , 150, 80, 5, bnFILETAGE, "FIL", BLACK, BLUE, m_ModifValue);
	button_bANGLE = cBUTTON(Point(700, 420) , 150, 80, 5, bnANGLE, "ANG", BLACK, BLUE, m_ModifValue);
	button_bANNULE = cBUTTON(Point(615, 420) , 150, 80, 5, bnANNULE, "ANNULE", WHITE, RED, m_ModifValue);
	button_bVALIDE = cBUTTON(Point(200, 420) , 150, 80, 5, bnVALIDE, "OK", WHITE, GREEN, m_ModifValue);
	button_b0 = cBUTTON(Point(635, 230) , 100, 50, 5, bn0, "0", BLACK, BLUE, m_ModifValue);
	button_b1 = cBUTTON(Point(525, 50) , 100, 50, 5, bn1, "1", BLACK, BLUE, m_ModifValue);
	button_b2 = cBUTTON(Point(635, 50) , 100, 50, 5, bn2, "2", BLACK, BLUE, m_ModifValue);
	button_b3 = cBUTTON(Point(745, 50) , 100, 50, 5, bn3, "3", BLACK, BLUE, m_ModifValue);
	button_b4 = cBUTTON(Point(525, 110) , 100, 50, 5, bn4, "4", BLACK, BLUE, m_ModifValue);
	button_b5 = cBUTTON(Point(635, 110) , 100, 50, 5, bn5, "5", BLACK, BLUE, m_ModifValue);
	button_b6 = cBUTTON(Point(745, 110) , 100, 50, 5, bn6, "6", BLACK, BLUE, m_ModifValue);
	button_b7 = cBUTTON(Point(525, 170) , 100, 50, 5, bn7, "7", BLACK, BLUE, m_ModifValue);
	button_b8 = cBUTTON(Point(635, 170) , 100, 50, 5, bn8, "8", BLACK, BLUE, m_ModifValue);
	button_b9 = cBUTTON(Point(745, 170) , 100, 50, 5, bn9, "9", BLACK, BLUE, m_ModifValue);
	button_bplus = cBUTTON(Point(745, 230) , 100, 50, 5, bnp, ">", BLACK, BLUE, m_ModifValue);
	button_bmoins = cBUTTON(Point(525, 230) , 100, 50, 5, bnm, "<", BLACK, BLUE, m_ModifValue);
	button_StopY = cBUTTON(Point(100, 260) , 150, 30, 3, bnStopY, "Y Stop", BLACK, WHITE, m_ModifValue);
	button_bplusmoins = cBUTTON(Point(635, 300) , 150, 50, 5, bnpm, "+/-", BLACK, BLUE, m_ModifValue);
#ifdef DEBUG
	Serial.println("Buttons Init ok");
#endif

	Buttons_Init(screen_Filetage, 2, &button_bANNULE, &button_bVALIDE);
	Buttons_Init(screen_RPM, 8, &button_bXYZ, &button_bFILETAGE, &button_bANGLE, 
            &button_bCX, &button_bVX, &button_bCY, &button_bVY, &button_StopY);
	Buttons_Init(screen_Position_Broche, 9, &button_bXYZ, &button_bRPM, &button_bFILETAGE, 
            &button_bCX, &button_bVX, &button_bCY, &button_bVY, &button_bCANGLE, &button_StopY);
	Buttons_Init(screen_XYZ, 10, &button_bRPM, &button_bFILETAGE, &button_bANGLE, 
            &button_bCX, &button_bVX, &button_bCY, &button_bVY, &button_bCZ, &button_bVZ, &button_StopY);
	Buttons_Init(screen_value_Axe, 13, &button_b1, &button_b2, &button_b3, &button_b4, &button_b5, &button_b6, &button_b7, &button_b8, &button_b9, &button_b0,
            &button_bmoins, &button_bplus, &button_bplusmoins);
}

void TFT::DessineButton( cBUTTON pButton, bool pInverted){
	__tft.graphicsMode();
	int16_t _fColor, _bColor;
	if (pInverted){
		_fColor = pButton.backColor;
		_bColor = pButton.textColor;
	}
	else{
		_fColor = pButton.textColor;
		_bColor = pButton.backColor;
	}
	__tft.fillRoundRect( pButton.center.x - (pButton.length / 2), pButton.center.y - (pButton.height / 2),
						pButton.length, pButton.height, pButton.textSize * 3, _bColor);
	__tft.setCursor(pButton.center.x - (pButton.textSize * 2.5 * pButton.texte.length()), pButton.center.y - (pButton.textSize * 3.5 ));
	__tft.setTextColor(_fColor, _bColor);
	__tft.setTextSize(pButton.textSize);
	__tft.print(pButton.texte);
}

void TFT::Buttons_Draw(ScreenStates pScreenState){
	byte _index = 0;
	while(_index < sizeof(Buttons[(byte)pScreenState])){	
		if (Buttons[(byte)pScreenState][_index] == 0){
			break;
		}
		DessineButton(*Buttons[(byte)pScreenState][_index], false);
		_index++;
	}
}
//enum ScreenStates {screen_XYZ, screen_RPM, screen_Filetage, screen_Position_Broche, screen_value_Axe};
void TFT::MenuMain(int32_t pX, int32_t pY, int32_t pZ, bool pRefresh, uint8_t pTypeZ, int32_t pStopY, byte pModifAxe){
	if (pRefresh){
		//__tft.graphicsMode();
		__tft.fillScreen(0);
		__tft.textEnlarge(3);
		printTXT("X:", 30, 40, RA8875_WHITE, RA8875_BLACK, 7);
		printTXT("Y:", 30, 170, RA8875_WHITE, RA8875_BLACK, 7);
		switch (pTypeZ){
			case 0: //XYZ
				printTXT("Z:", 30, 300, RA8875_WHITE, RA8875_BLACK, 7);
				break;
			case 1: //RPM
				printTXT("RPM:", 10, 300, RA8875_WHITE, RA8875_BLACK, 7);
				break;	
			case 2: //FIL
				printTXT("FIL:", 10, 300, RA8875_WHITE, RA8875_BLACK, 7);
				break;
			case 3: //ANG
				printTXT("ANG:", 10, 300, RA8875_WHITE, RA8875_BLACK, 7);
				break;	
			default:
				break;
		}
		__tft.textEnlarge(2);
		//printTXT("Y stop:", 30, 240, RA8875_WHITE, RA8875_BLACK, 3);
		toTXT(pStopY, true);
		printTXT(result, 230, 250, RA8875_WHITE, RA8875_BLACK, 3);
		__tft.textEnlarge(3);

		Buttons_Draw((ScreenStates) pTypeZ);
	}
	__tft.textMode();
	if ((_oldX != pX) | pRefresh){
		toTXT(pX, true);
		if (pModifAxe == axe_X){
			printTXT(result, 125, 40, BLACK, WHITE, 7);
		} 
		else	{
			printTXT(result, 125, 40, RA8875_WHITE, RA8875_BLACK, 7);
		}
		_oldX = pX;
	}
	if ((_oldY != pY) | pRefresh){
		toTXT(pY, true);
		if (pModifAxe == axe_Y){
			printTXT(result, 125, 170, BLACK, WHITE, 7);
		}
		else{
			printTXT(result, 125, 170, RA8875_WHITE, RA8875_BLACK, 7);
		}
		_oldY = pY;
	}
	if ((_oldZ != pZ) | pRefresh){
		toTXT(pZ, (pTypeZ == 0 | pTypeZ == 3));
		switch (pTypeZ){
			case 0: //XYZ
				printTXT("Z:", 30, 300, RA8875_WHITE, RA8875_BLACK, 7);
				//Button(450,300, 10, RA8875_WHITE, RA8875_RED, "C", 0);
				//Button(620,300, 10, RA8875_BLACK, RA8875_GREEN, "Z", 0);
				break;
			case 1: //RPM
				printTXT("RPM:", 10, 300, RA8875_WHITE, RA8875_BLACK, 7);
				break;	
			case 2: //FIL
				printTXT("FIL:", 10, 300, RA8875_WHITE, RA8875_BLACK, 7);
				break;
			case 3: //ANG
				printTXT("ANG:", 10, 300, RA8875_WHITE, RA8875_BLACK, 7);
				break;	
			default:
				break;
		}
		if (pModifAxe == axe_Z){
			printTXT(result, 125, 300, BLACK, WHITE, 7);
		}
		else {
			printTXT(result, 125, 300, RA8875_WHITE, RA8875_BLACK, 7);
		}
		_oldZ = pZ;
	}
	if ((_old_Y_Stop != pStopY) | pRefresh){
		__tft.textEnlarge(2);
		toTXT(pStopY, true);
		if (pModifAxe == axe_STOP_Y){
			printTXT(result, 230, 250, BLACK, WHITE, 3);	
		} 
		else {
			printTXT(result, 230, 250, RA8875_WHITE, RA8875_BLACK, 3);
		}
		__tft.textEnlarge(3);
	}
}

void TFT::MenuValue(uint8_t pAxe){
	//affiche clavier numerique
	char _c[2];
	_c[1] = 0;
	__tft.fillRect(455,0,350,480,RA8875_BLACK);
	__tft.fillRect(0,370,500,110,RA8875_BLACK);
	Buttons_Draw(screen_value_Axe);
	if (pAxe<3){
		__tft.fillRect(110, pAxe * 150, 350, 95, RA8875_BLACK);
		__tft.textEnlarge(3);
	} else {
		__tft.fillRect(200, 250, 200, 50, RA8875_BLACK);
		__tft.textEnlarge(2);
	}
	//SetTextColor(RA8875_WHITE, true);
#ifdef RA8875
	SetTextColor(RA8875_WHITE, true);
#endif
#ifdef SCREEN_ESP32
	SetTextColor32(WHITE, BLACK);
#endif
	delay(500);
}

int8_t _button, _GCD;
int32_t  _nextRetombee;
double 	_decalage, _decalageOld, _calcul1, _calcul2, _calcul3;
Point _PointFiletage(0, 0);
/// @brief -1 = RIEN, 0 = OK, 1 = ANNULER, 2 = ENCOURS
/// @return 
int8_t TFT::MenuFiletage(uint8_t pIndexMenu, bool pRefresh, int32_t pPosX,  int32_t pPosY, int32_t pPosBroche){
	switch (pIndexMenu)
	{
	case 0: {//Calcul du pas de filetage, il faut au moins 10 tours de broche pour valider un pas
		if (pRefresh){
			__tft.fillScreen(RA8875_BLACK);
			__tft.textEnlarge(2);
			printTXT("CALCUL DU PAS DE FILETAGE", 30, 40, RA8875_WHITE, RA8875_BLACK, 7);
			printTXT("Valeur Y :", 30, 120, RA8875_WHITE, RA8875_BLACK, 7);
			printTXT("Broche :         /  10.000", 30, 200, RA8875_WHITE, RA8875_BLACK, 7);
			printTXT("PAS CALCULE :", 30, 280, RA8875_WHITE, RA8875_BLACK, 7);
			Button(120, 400, 10, RA8875_BLACK, RA8875_RED, "ANN", 8);
			Button(560, 400, 10, RA8875_BLACK, RA8875_GREEN, "OK",8);
			Button(550, 280, 8, RA8875_BLACK, RA8875_BLUE, " +", 8);
			Button(700, 280, 8, RA8875_BLACK, RA8875_BLUE, " -",8);
			_pasFiletage = 0;
		}
		if ((_oldY != pPosY)){
			__tft.textEnlarge(2);
			toTXT(pPosY, true);
			printTXT(result, 280, 120, RA8875_WHITE, RA8875_BLACK, 7);
			_oldY = pPosY;
		}
		if ((_oldZ != pPosBroche)){
			__tft.textEnlarge(2);
			toTXT((pPosBroche * 10) / 4, true);
			printTXT(result, 220, 200, RA8875_WHITE, RA8875_BLACK, 7);
			_oldZ = pPosBroche;
		}
		if (pPosY>0){
			if ((pPosBroche -_posBrocheMovY)<4000){
				if (pPosBroche - _posBrocheMovY != 0){
					_pasFiletage = (((pPosY ) / ((pPosBroche - _posBrocheMovY) / 400)) / 10) * 10;
				}
			}
		}
		else{
			_posBrocheMovY = pPosBroche;
		}
		__tft.textEnlarge(2);
		toTXT(_pasFiletage, true);
		printTXT(result, 300, 280, RA8875_WHITE, RA8875_BLACK, 7);
		if (pRefresh){
			return PAS_DEMMARE;
		}
		_button = 0;//IfTouched(10, 6, 10, false);
		switch (_button){
			case 38:
			case 48:
				if ((pPosBroche -_posBrocheMovY)>4000){
					_pasFiletage += 10;
					return 2;
				}
				break;
			case 40:
			case 50:
				if ((pPosBroche -_posBrocheMovY)>4000){
					_pasFiletage -= 10;
					return 2;
				}
				break;
			case 52:
			case 53:
				return ANNULER; //ANNULE
			case 58:
			case 59: //ok
				if ((pPosBroche -_posBrocheMovY)>4000){
					return OK;
				}
				break;
			
			default:
				break;
		}
		if (_button == 3)
			{return ANNULER;} //annule
		if (pPosY < 0)
			{return PAS_DEMMARE;} //pas demarré	
		return EN_COURS;	//calcul en cours
		break;
	}
	case 1: {//affiche la retombee dans le pas
		if (pRefresh){
			__tft.fillScreen(RA8875_BLACK);
			__tft.textEnlarge(2);
			printTXT("FILETAGE en cours", 30, 40, RA8875_WHITE, RA8875_BLACK, 8);
			printTXT("PAS:", 30, 120, RA8875_WHITE, RA8875_BLACK, 8);
			printTXT(" X :", 30, 200, RA8875_WHITE, RA8875_BLACK, 8);
			printTXT(" Y :", 30, 280, RA8875_WHITE, RA8875_BLACK, 7);
			printTXT(" B :", 30, 360, RA8875_WHITE, RA8875_BLACK, 7);
			Button(360, 400, 10, RA8875_BLACK, RA8875_RED, "ANN", 8);
			toTXT(_pasFiletage, true);
			__tft.textEnlarge(2);
			printTXT(result, 170, 120, RA8875_WHITE, RA8875_BLACK, 7);
			__tft.graphicsMode();
			__tft.drawCircle(650,240,130,WHITE);
			__tft.drawLine(650,100,650,120,RA8875_GREEN);
			__tft.textMode();
			/*
			button(620, 400, 10, RA8875_BLACK, RA8875_GREEN, "OK",8);
			button(550, 280, 8, RA8875_BLACK, RA8875_BLUE, " +", 8);
			button(700, 280, 8, RA8875_BLACK, RA8875_BLUE, " -",8);
			*/
			_GCD = _pasFiletage /  m_GCD(PAS_VIS_MERE, _pasFiletage);
			_nextRetombee = _GCD * 6000;
			//__tft.textEnlarge(3);
			//__tft.textSetCursor(420,220);
			//__tft.print(_GCD, DEC);
			//__tft.textSetCursor(620,220);
			//__tft.print(_nextRetombee, DEC);
		}
		if ((_oldX != pPosX)){
			__tft.textEnlarge(2);
			toTXT(pPosX, true);
			printTXT(result, 170, 200, RA8875_WHITE, RA8875_BLACK, 7);
			_oldX = pPosX;
		}
		if ((_oldY != pPosY)){
			__tft.textEnlarge(2);
			toTXT(pPosY, true);
			printTXT(result, 170, 280, RA8875_WHITE, RA8875_BLACK, 7);
			_oldY = pPosY;
		}
		if (_oldZ != pPosBroche){
			__tft.textEnlarge(2);
			toTXT(pPosBroche, true);
			printTXT(result, 170, 360, RA8875_WHITE, RA8875_BLACK, 7);
			_oldZ = pPosBroche;
		}
		if (pRefresh){
			return PAS_DEMMARE;
		}
		//calcul
		__tft.graphicsMode();
		_calcul1 = ((double)pPosBroche - (double)_posBrocheMovY) / 400;// * (double)_pasFiletage;
		_calcul2 = _calcul1 * _pasFiletage;// 400;
		_calcul3 = (int)_calcul2 % _nextRetombee; 
		_decalage = (int)_calcul3 % 6000; //_nextRetombee - ((double)pPosY - (double)((((double)pPosBroche - (double)_posBrocheMovY) * (double)_pasFiletage) / 400.0));
		//Trace le curseur
		Calcul_Coordonnees(_decalageOld / 6000);
		__tft.graphicsMode();
		__tft.drawLine(CENTRE_X, CENTRE_Y, _PointFiletage.x, _PointFiletage.y, RA8875_BLACK);
		Calcul_Coordonnees(_decalage / 6000);
		__tft.drawLine(CENTRE_X, CENTRE_Y, _PointFiletage.x, _PointFiletage.y, RA8875_WHITE);
		__tft.textMode();
		_decalageOld = _decalage;
		//Affiche Index de retombée
		__tft.textMode();
		//SetTextColor(WHITE, true);
#ifdef RA8875
	SetTextColor(WHITE, true);
#endif
#ifdef SCREEN_ESP32
	SetTextColor32(WHITE, BLACK);
#endif
		__tft.textEnlarge(3);
		__tft.textSetCursor(420,220);
		__tft.print((int)((_nextRetombee - _calcul3) / 6000), DEC);
		/*
		
		__tft.textSetCursor(420,280);
		__tft.print(_calcul1, DEC);
				__tft.textSetCursor(620,280);
		__tft.print(_calcul2, DEC);
				__tft.textSetCursor(420,360);
		__tft.print(_decalage, DEC);
			__tft.textEnlarge(3);
			__tft.textSetCursor(420,220);
			__tft.print((int)(_calcul3 / 6000), DEC);

*/
		_button = 0;// IfTouched(10, 6, 10, false);
		if ((_button == 56) | (_button == 57)){
			return ANNULER;
		}
		return EN_COURS;
	}
	default:
		break;
	}
	return PAS_DEMMARE;
}

void TFT::Calcul_Coordonnees(double pvalue){
	_PointFiletage.x =  (RAYON * sin(PI * pvalue * 2)) + CENTRE_X;
	_PointFiletage.y = -(RAYON * cos(PI * pvalue * 2 )) + CENTRE_Y;
}

int16_t _result;
ButtonNames TFT::IfTouched(ScreenStates pScreenState, uint16_t pDelay){//(uint16 pNbrColonnes, uint8_t pNbrLignes, uint16_t pDelay, bool pWriteTXT){//retourne le nom du button
	if (m_readTouch()){
		delay(pDelay);
		if (!m_readTouch()){
			return bnNULL;
		}
		byte _index=0;
		while ((Buttons[(byte)pScreenState][_index] != 0) & (_index < sizeof(Buttons[(byte)pScreenState])/4)){
			if (Buttons[(byte)pScreenState][_index]->center.InRange(_Touched, Buttons[(byte)pScreenState][_index]->length, Buttons[(byte)pScreenState][_index]->height)){
				DessineButton(*Buttons[(byte)pScreenState][_index], true);
				ButtonLast = Buttons[(byte)pScreenState][_index];
				return Buttons[(byte)pScreenState][_index]->name;
			}
#ifdef DEBUG
		Serial.print("Index: ");
		Serial.println(_index, DEC);
#endif
			_index++;
		}
#ifdef DEBUG
		Serial.println("NON Trouvé");
#endif
		return bnNULL;
		/*
		_result = ((v_XTouch  * pNbrColonnes) / 800) + (pNbrColonnes *  ((v_YTouch * pNbrLignes) / 480)) + 1;
		if (pWriteTXT){
			toTXT(_result, true);
			printTXT(result, 0,400,RA8875_RED, RA8875_BLACK, 3);
		}
		//toTXT((v_YTouch * pNbrLignes) / 480);
	//	printTXT(result, 400 ,400,RA8875_RED, RA8875_BLACK, 3);
		Serial.print("Touched:");
		Serial.println(_result, DEC);
		return _result;
		*/
	}
	else 
		return bnNULL;
}

///////////////////////////////////////////
/// REGION PRIVATE
///////////////////////////////////////////
char _caractere;
//return -XXX.XXX
void TFT::toTXT(int32 pValue, bool pPoint){
	if (pPoint){_caractere = '0';}
	else {_caractere = ' ';}
	if (pValue>=0){result[0] =' ';}
	else{result[0] = '-';}
	pValue = abs(pValue);
	if (pValue>99999){result[1] = char(48 + (pValue/100000));}
	else{
		result[1] = ' ';
		}
	pValue -= (pValue/100000) * 100000;
	if (pValue>9999){result[2] = char(48 + (pValue/10000));}
	else{
		result[2] = ' ';
		if (pPoint){
			if (result[1] != ' '){
				result[2] = '0';
			}
		}
	}
	pValue -= (pValue/10000) * 10000;
	if (pValue>999){
		result[3] = char(48 + (pValue/1000));
		result[4] = '.';
	}
	else{
		result[3] = _caractere;
		if(pPoint){result[4] = '.';}
		else {result[4] = ' ';}
	}
	pValue -= (pValue/1000) * 1000;
	if (pValue>99){result[5] = char(48 + (pValue/100));}
	else{result[5] = '0';}
	pValue -= (pValue/100) * 100;
	if (pValue>9){result[6] = char(48 + (pValue/10));}
	else{result[6] = '0';}
	pValue -= (pValue/10) * 10;
	result[7] = char(48 + pValue);
}

void TFT::SetTextColor(uint16_t pColor, bool pFront){
#ifdef SCREEN_ESP32
	//#error SET_TEXT_COLOR
#endif
	switch (pColor)	{
#ifdef RA8875
		case  RA8875_RED:
			if (pFront) {Text_Foreground_Color (255,0,0);}
			else {Text_Background_Color(255,0,0);}
			break;
		case RA8875_GREEN:
			if (pFront) {Text_Foreground_Color (0,255,0);}
			else {Text_Background_Color(0,255,0);}
			break;
		case RA8875_BLUE:
			if (pFront) {Text_Foreground_Color (0,0,255);}
			else {Text_Background_Color(0,0,255);}
			break;
		case RA8875_WHITE:
			if (pFront) {Text_Foreground_Color (255,255,255);}
			else {Text_Background_Color(255,255,255);}
			break;
		case RA8875_BLACK:
			if (pFront) {Text_Foreground_Color (0,0,0);}
			else {Text_Background_Color(0,0,0);}
			break;
#endif
		default:
		break;
	}
}

void TFT::SetTextColor32(uint16_t pFColor, uint16_t pBColor){
	__tft.setTextColor(pFColor, pBColor);
}

void TFT::Button(uint16 pX, uint16 pY, uint8_t pSize, uint16_t pFrontColor, uint16_t pBackColor, char* pText, uint8_t pDecalageY){
	__tft.graphicsMode();
	__tft.fillRoundRect(pX, pY, 12 * pSize, 7 * pSize, pSize, pBackColor);
	Text_Foreground_Color(255, 255, 255);
#ifdef RA8875
	SetTextColor(pFrontColor, true);
	SetTextColor(pBackColor, false);
#endif
#ifdef SCREEN_ESP32
	SetTextColor32(pFrontColor, pBackColor);
#endif
	__tft.textMode();
	if (pSize>8){__tft.textEnlarge(3);}
	else {__tft.textEnlarge(2);}
	__tft.textSetCursor(pX + (4 * (pSize + 1 - pDecalageY)) , pY);
	__tft.print(pText);
}

void TFT::printTXT(char* pText, uint16 pX, uint16 pY, uint16_t pTextColor, uint16_t pBackColor, int pSize){
	__tft.textMode();
	__tft.textSetCursor(pX, pY);
	__tft.setTextSize(pSize);
#ifdef RA8875
	SetTextColor(pTextColor, true);
	SetTextColor(pBackColor, false);
#endif
#ifdef SCREEN_ESP32
	SetTextColor32(pTextColor, pBackColor);
#endif
	__tft.print(pText);
}

void TFT::Text_Background_Color(uchar setR, uchar setG, uchar setB)
{
	LCD_CmdWrite(0x60);//BGCR0
	LCD_DataWrite(setR);
	LCD_CmdWrite(0x61);//BGCR0
	LCD_DataWrite(setG);
	LCD_CmdWrite(0x62);//BGCR0
	LCD_DataWrite(setB);
}

void TFT::Text_Foreground_Color(uchar setR, uchar setG, uchar setB)
{
	LCD_CmdWrite(0x63);//BGCR0
	LCD_DataWrite(setR);
	LCD_CmdWrite(0x64);//BGCR0
	LCD_DataWrite(setG);
	LCD_CmdWrite(0x65);//BGCR0
	LCD_DataWrite(setB);
}

void TFT::Memory_Clear(void)
{
	uchar temp;
	LCD_CmdWrite(0x8e);//MCLR
	temp = LCD_DataRead();
	temp |= cSetD7;
	LCD_DataWrite(temp);
	Chk_Busy();
}

void TFT::Chk_Busy(void)
{
	uchar temp;
	do
	{
		temp = LCD_StatusRead();
	} while ((temp & 0x80) == 0x80);
}

bool TFT::m_readTouch() { //retrun true si detecte ecran touch�
	__tft.touchGet(&_Touched);
	if ((_Touched.x != 0) | (_Touched.y != 0)) {
		//delay(10);
		//__tft.touchGet(&v_XTouch, &v_YTouch);
		/*
		v_XTouch = map(v_XTouch, 70, 970, 0, 800);
		v_YTouch = map(v_YTouch, 104, 880, 0, 480);
		if (v_XTouch <= 0) { v_XTouch = 1; }
		if (v_XTouch >= 800) { v_XTouch = 799; }
		if (v_YTouch <= 0) { v_YTouch = 1; }
		if (v_YTouch >= 480) { v_YTouch = 479; 
		*/
#ifdef DEBUG
	Serial.print("Touch: ");
	Serial.print(_Touched.x, DEC);
	Serial.print('.');
	Serial.println(_Touched.y, DEC);
#endif
		return true;
	}
	else
	{

		return false;
	}
return false;
}

/*
void TFT::m_dessinerClavier() {
	__tft.fillScreen(0);
	__tft.textMode();
	__tft.textSetCursor(200, 10);
	__tft.textEnlarge(1);
	Text_Background_Color(0, 0, 0);
	Text_Foreground_Color(255, 255, 255);
	__tft.print("Entrez le TEST de deblocage");
	Text_Background_Color(0, 255, 0);
	__tft.textEnlarge(3);
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			__tft.graphicsMode();
			__tft.fillRoundRect(100 + (j * 220), 60 + (i * 80), 160, 64, 10, RA8875_GREEN);
			Text_Foreground_Color(255, 255, 255);
			__tft.textMode();
			__tft.textSetCursor(160 + (j * 220), 60 + (i * 80));
			if (i == 3) {
				switch (j)
				{
				case 0:
					__tft.print("<");
					break;
				case 1:
					__tft.print("0");
					break;
				case 2:
					__tft.print(">");
					break;
				default:
					break;
				}
			}
			else
			{
				__tft.print((j + 1) + (i * 3), DEC);
			}
		}
	}
	Text_Foreground_Color(255, 255, 255);
}

void TFT::m_deblocageCodePIN() {
	String v_code = "";
	char v_char = 0, v_x, v_y;
	char v_clavier[12] = { '1', '2','3','4','5','6','7','8','9','<','0','>' };
	while (v_code != "0551>") {
		if (m_readTouch()) {
			if ((v_XTouch > 100)& (v_XTouch < 700)& (v_YTouch > 60)& (v_YTouch < 400)) {
				v_x = map(v_XTouch, 100, 700, 0, 3);
				v_y = map(v_YTouch, 60, 400, 0, 4);
				if ((v_x == 0) & (v_y == 3)) {//appuie sur <
					if (v_code.length() > 0) { v_code.remove(v_code.length() - 1); }
				}
				else
				{
					v_code += v_clavier[(v_y * 3) + v_x];
				}
				__tft.textMode();
				__tft.textSetCursor(300, 400);
				__tft.print("      ");
				__tft.textSetCursor(300, 400);
				for (size_t i = 0; i < v_code.length(); i++)
				{
					__tft.print("*");
				}
				while (m_readTouch()) {}
				delay(100);
			}
		}
	}
	__tft.fillScreen(0);
	Text_Foreground_Color(255, 255, 255);
	__tft.println("Started");
	delay(500);
}
*/