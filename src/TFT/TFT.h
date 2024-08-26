#pragma once 

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include "Adafruit_RA8875.h"
#include "..\DRO.h"


#define PIN_TFT_CS GPIO_NUM_5
#define PIN_TFT_RESET GPIO_NUM_17


#define LCD_DataWrite __tft.writeData
#define LCD_CmdWrite __tft.writeCommand
#define uchar byte 
#define LCD_DataRead __tft.readData
#define bit boolean
#define LCD_StatusRead __tft.readStatus
#define Delay1ms(X) delay(1*X)
#define Delay10ms(X) delay(10*X)
#define Delay100ms(X) delay(100*X)
#define ulong long

#define LCD_DataWrite __tft.writeData
#define LCD_CmdWrite __tft.writeCommand
#define uchar byte 
#define LCD_DataRead __tft.readData
#define bit boolean
#define LCD_StatusRead __tft.readStatus
#define Delay1ms(X) delay(1*X)
#define Delay10ms(X) delay(10*X)
#define Delay100ms(X) delay(100*X)
#define ulong long


#define	cSetD0		     0x01
#define	cSetD1		     0x02
#define	cSetD2		     0x04
#define	cSetD3		     0x08
#define	cSetD4		     0x10
#define	cSetD5		     0x20
#define	cSetD6		     0x40
#define	cSetD7		     0x80

#define	cClrD0		     0xfe
#define	cClrD1		     0xfd
#define	cClrD2		     0xfb
#define	cClrD3		     0xf7
#define	cClrD4		     0xef
#define	cClrD5		     0xdf
#define	cClrD6		     0xbf
#define	cClrD7		     0x7f

#define cClrD1D0         0xfc
#define cClrD1SetD0	     0x01
#define cSetD1ClrD0	     0x02
#define cSetD1D0		 0x03
#define cClrD3D2         0xf3
#define cClrD3SetD2      0x04
#define cSetD3ClrD2      0x08
#define cSetD3D2		 0x0c

#define cClrD2D1D0		 0xf8
#define cClrD2D1SetD0	 0x01
#define cClrD2SetD1ClrD0 0x02
#define cClrD2SetD1D0	 0x03
#define cSetD2ClrD1D0	 0x04
#define cSetD2ClrD1SetD0 0x05

#define cClrD6D5D4	     0x8f
#define cClrD6D5SetD4    0x10
#define cClrD6SetD5ClrD4 0x20
#define cClrD6SetD5D4	 0x30
#define cSetD6ClrD5D4	 0x40
#define cSetD6ClrD5SetD4 0x50
#define cSetD6D5ClrD4	 0x60
#define cSetD6D5D4		 0x70


//#define uint uint8_t
//#define uint16 uint16_t

class TFT{

#ifdef RA8875
    Adafruit_RA8875 __tft = Adafruit_RA8875(PIN_TFT_CS, PIN_TFT_RESET);
#endif
uint16_t v_XTouch, v_YTouch;

public:
    bool Init();
    //pTypeZ 0=Fraiseuse, 1=RPM, 2=Filetage, 3=Position_Broche
    void MenuMain(int32_t pX, int32_t pY, int32_t pZ, bool pRefresh, uint8_t pTypeZ, int32_t pStopY);
    void MenuValue(uint8_t pAxe);
    int8_t MenuFiletage(uint8_t pIndexMenu, bool pRefresh, int32_t pPosX, int32_t pPosY, int32_t pPosBroche);//retourne si OK
    int8_t IfTouched(uint16_t pNbrColonnes, uint8_t pNbrLignes, uint16_t pDelay, bool pWriteTXT);//retourne la position du button touché
    void print(String pS){__tft.print(pS);}
    void printTXT(char* pText, int pX, int pY, uint16_t pTextColor, uint16_t pBackColor, int pSize);
    void clr(int8_t pSize){ __tft.fillScreen(0); __tft.textEnlarge(pSize);}

private:
    void Active_Window(uint16_t XL, uint16_t XR, uint16_t YT, uint16_t YB);
    void Chk_Busy(void);
    void Clear_Active_Window(void);
    void Memory_Clear(void);
    void Text_Background_Color(uchar setR, uchar setG, uchar setB);
    void Text_Foreground_Color(uchar setR, uchar setG, uchar setB);
    void toTXT(int32_t pValue, bool pPoint);
    void Button(uint16_t pX, uint16_t pY, uint8_t pSize, uint16_t pTextColor, uint16_t pBackColor,  char* pText, uint8_t pDecalageY);
    void SetTextColor(uint16_t pColor, bool pFront);
    void Calcul_Coordonnees(double pValue);

    bool m_readTouch();
    void m_dessinerClavier();
    void m_deblocageCodePIN();
};

extern TFT _tft;
