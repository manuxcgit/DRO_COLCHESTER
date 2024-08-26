#include "TFT.h"

#define display __tft
#define BLACK RA8875_BLACK
#define WHITE RA8875_WHITE

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
}

//enum ScreenStates {screen_XYZ, screen_RPM, screen_Filetage, screen_Position_Broche, screen_value_Axe};
void TFT::MenuMain(int32_t pX, int32_t pY, int32_t pZ, bool pRefresh, uint8_t pTypeZ, int32_t pStopY){
	if (pRefresh){
		//__tft.graphicsMode();
		__tft.fillScreen(0);
		__tft.textEnlarge(3);
		printTXT("X:", 30, 40, RA8875_WHITE, RA8875_BLACK, 9);
		printTXT("Y:", 30, 170, RA8875_WHITE, RA8875_BLACK, 9);
		Button(450,40, 10, RA8875_WHITE, RA8875_RED, "C", 0);
		Button(450,170, 10, RA8875_WHITE, RA8875_RED, "C", 0);
		Button(620,40, 10, RA8875_BLACK, RA8875_GREEN, "X", 0);
		Button(620,170, 10, RA8875_BLACK, RA8875_GREEN, "Y", 0);
		switch (pTypeZ){
			case 0: //XYZ
				printTXT("Z:", 30, 300, RA8875_WHITE, RA8875_BLACK, 9);
				Button(450,300, 10, RA8875_WHITE, RA8875_RED, "C", 0);
				Button(620,300, 10, RA8875_BLACK, RA8875_GREEN, "Z", 0);
				break;
			case 1: //RPM
				printTXT("RPM:", 10, 300, RA8875_WHITE, RA8875_BLACK, 9);
				break;	
			case 2: //FIL
				printTXT("FIL:", 10, 300, RA8875_WHITE, RA8875_BLACK, 9);
				break;
			case 3: //ANG
				printTXT("ANG:", 10, 300, RA8875_WHITE, RA8875_BLACK, 9);
				Button(450,300, 10, RA8875_WHITE, RA8875_RED, "C", 0);
				break;	
			default:
				break;
		}
		Button(60, 400, 10, RA8875_BLACK, RA8875_BLUE, "XYZ", 8);
		Button(245, 400, 10, RA8875_BLACK, RA8875_BLUE, "RPM", 8);
		Button(430, 400, 10, RA8875_BLACK, RA8875_BLUE, "FIL",8);
		Button(615, 400, 10, RA8875_BLACK, RA8875_BLUE, "ANG", 8);
		__tft.textEnlarge(2);
		printTXT("Y stop:", 30, 240, RA8875_WHITE, RA8875_BLACK, 9);
		toTXT(pStopY, true);
		printTXT(result, 185, 240, RA8875_WHITE, RA8875_BLACK, 9);
		__tft.textEnlarge(3);
	}
	__tft.textMode();
	if ((_oldX != pX) | pRefresh){
		toTXT(pX, true);
		printTXT(result, 125, 40, RA8875_WHITE, RA8875_BLACK, 9);
		_oldX = pX;
	}
	if ((_oldY != pY) | pRefresh){
		toTXT(pY, true);
		printTXT(result, 125, 170, RA8875_WHITE, RA8875_BLACK, 9);
		_oldY = pY;
	}
	if ((_oldZ != pZ) | pRefresh){
		toTXT(pZ, (pTypeZ == 0 | pTypeZ == 3));
		switch (pTypeZ){
			case 0: //XYZ
				printTXT("Z:", 30, 300, RA8875_WHITE, RA8875_BLACK, 9);
				//Button(450,300, 10, RA8875_WHITE, RA8875_RED, "C", 0);
				//Button(620,300, 10, RA8875_BLACK, RA8875_GREEN, "Z", 0);
				break;
			case 1: //RPM
				printTXT("RPM:", 10, 300, RA8875_WHITE, RA8875_BLACK, 9);
				break;	
			case 2: //FIL
				printTXT("FIL:", 10, 300, RA8875_WHITE, RA8875_BLACK, 9);
				break;
			case 3: //ANG
				printTXT("ANG:", 10, 300, RA8875_WHITE, RA8875_BLACK, 9);
				break;	
			default:
				break;
		}
		printTXT(result, 125, 300, RA8875_WHITE, RA8875_BLACK, 9);
		_oldZ = pZ;
	}
	if ((_old_Y_Stop != pStopY) | pRefresh){
		__tft.textEnlarge(2);
		toTXT(pStopY, true);
		printTXT(result, 185, 240, RA8875_WHITE, RA8875_BLACK, 9);
		__tft.textEnlarge(3);
	}
}
void TFT::MenuValue(uint8_t pAxe){
	//affiche clavier numerique
	char _c[2];
	_c[1] = 0;
	__tft.fillRect(400,0,400,480,RA8875_BLACK);
	__tft.fillRect(0,370,400,110,RA8875_BLACK);
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			_c[0] = '9' - (2  - j + (i * 3));
			if (i == 3){ 
				switch (j)
				{
				case 0:
					_c[0] = '<';
					break;
				case 1:
					_c[0] = '0';
					break;
				case 2:
					_c[0] = '>';
					break;				
				default:
					break;
				}	
			}
			Button(400 + (j * 150), 60 + (i * 80), 8, RA8875_BLACK, RA8875_BLUE, _c, 0);
		}		
	}	
	Button(550, 380, 8, RA8875_BLACK, RA8875_BLUE, "+/-", 6);
	if (pAxe<3){
		__tft.fillRect(100, pAxe * 150, 300, 100, RA8875_BLACK);
		__tft.textEnlarge(3);
	} else {
		__tft.fillRect(200, 250, 200, 50, RA8875_BLACK);
		__tft.textEnlarge(2);
	}
	SetTextColor(RA8875_WHITE, true);
	delay(500);
}

int8_t _button, _GCD;
int32_t  _nextRetombee;
double 	_decalage, _decalageOld, _calcul1, _calcul2, _calcul3;
Point _PointFiletage;
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
		_button = IfTouched(10, 6, 10, false);
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
			printTXT("FILETAGE en cours", 30, 40, RA8875_WHITE, RA8875_BLACK, 7);
			printTXT("PAS:", 30, 120, RA8875_WHITE, RA8875_BLACK, 7);
			printTXT(" X :", 30, 200, RA8875_WHITE, RA8875_BLACK, 7);
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
		SetTextColor(WHITE, true);
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
		_button = IfTouched(10, 6, 10, false);
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
int8_t TFT::IfTouched(uint16 pNbrColonnes, uint8_t pNbrLignes, uint16_t pDelay, bool pWriteTXT){//retourne la position du button touché, 0 en haut à droite
	if (m_readTouch()){
		delay(pDelay);
		if (!m_readTouch()){
			return -1;
		}
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
	}
	else 
		return -1;
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
	switch (pColor)	{
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
		default:
		break;
	}
}

void TFT::Button(uint16 pX, uint16 pY, uint8_t pSize, uint16_t pFrontColor, uint16_t pBackColor, char* pText, uint8_t pDecalageY){
	__tft.graphicsMode();
	__tft.fillRoundRect(pX, pY, 12 * pSize, 7 * pSize, pSize, pBackColor);
	Text_Foreground_Color(255, 255, 255);
	SetTextColor(pFrontColor, true);
	SetTextColor(pBackColor, false);
	__tft.textMode();
	if (pSize>8){__tft.textEnlarge(3);}
	else {__tft.textEnlarge(2);}
	__tft.textSetCursor(pX + (4 * (pSize + 1 - pDecalageY)) , pY);
	__tft.print(pText);
}

void TFT::printTXT(char* pText, int pX, int pY, uint16_t pTextColor, uint16_t pBackColor, int pSize){
	__tft.textMode();
	__tft.textSetCursor(pX, pY);
	SetTextColor(pTextColor, true);
	SetTextColor(pBackColor, false);
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
	__tft.touchRead(&v_XTouch, &v_YTouch);
	delay(1);
	if (__tft.touched()) {
		__tft.touchRead(&v_XTouch, &v_YTouch);
		v_XTouch = map(v_XTouch, 70, 970, 0, 800);
		v_YTouch = map(v_YTouch, 104, 880, 0, 480);
		if (v_XTouch <= 0) { v_XTouch = 1; }
		if (v_XTouch >= 800) { v_XTouch = 799; }
		if (v_YTouch <= 0) { v_YTouch = 1; }
		if (v_YTouch >= 480) { v_YTouch = 479; }
		return true;
	}
	else
	{
		return false;
	}
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