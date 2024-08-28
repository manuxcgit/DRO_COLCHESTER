#pragma once 

#include <Arduino.h>
#include "..\DRO.h"

#ifdef RA8875
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

    #include <Adafruit_GFX.h>
    #include "Adafruit_RA8875.h"
    #define BLACK RA8875_BLACK
    #define WHITE RA8875_WHITE
#endif
#ifdef SCREEN_ESP32
    #include <Arduino_GFX_Library.h>
    typedef struct Point {
        int32_t x;
        int32_t y;
    } tsPoint_t;
#endif

class TFT{

#ifdef RA8875
    Adafruit_RA8875 __tft = Adafruit_RA8875(PIN_TFT_CS, PIN_TFT_RESET);
    public:
        void clr(int8_t pSize){ __tft.fillScreen(0); __tft.textEnlarge(pSize);}
#endif

#ifdef SCREEN_ESP32
    #define GFX_BL DF_GFX_BL // default backlight pin, you may replace DF_GFX_BL to actual backlight pin
    #define RA8875_BLACK BLACK
    #define RA8875_WHITE WHITE
    #define RA8875_RED RED
    #define RA8875_GREEN GREEN
    #define RA8875_BLUE BLUE
    #define TFT_BL 2
    //#define __tft.textEnlarge __tft.setTextSize
    #define uchar u_int8_t

    Arduino_ESP32RGBPanel bus = Arduino_ESP32RGBPanel(
        GFX_NOT_DEFINED /* CS */, GFX_NOT_DEFINED /* SCK */, GFX_NOT_DEFINED /* SDA */,
        41 /* DE */, 40 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
        14 /* R0 */, 21 /* R1 */, 47 /* R2 */, 48 /* R3 */, 45 /* R4 */,
        9 /* G0 */, 46 /* G1 */, 3 /* G2 */, 8 /* G3 */, 16 /* G4 */, 1 /* G5 */,
        15 /* B0 */, 7 /* B1 */, 6 /* B2 */, 5 /* B3 */, 4 /* B4 */
    );

    Arduino_RPi_DPI_RGBPanel __tft = Arduino_RPi_DPI_RGBPanel(
    &bus,
    //  800 /* width */, 0 /* hsync_polarity */, 8/* hsync_front_porch */, 2 /* hsync_pulse_width */, 43/* hsync_back_porch */,
    //  480 /* height */, 0 /* vsync_polarity */, 8 /* vsync_front_porch */, 2/* vsync_pulse_width */, 12 /* vsync_back_porch */,
    //  1 /* pclk_active_neg */, 16000000 /* prefer_speed */, true /* auto_flush */);

        800 /* width */, 0 /* hsync_polarity */, 210 /* hsync_front_porch */, 30 /* hsync_pulse_width */, 16 /* hsync_back_porch */,
        480 /* height */, 0 /* vsync_polarity */, 22 /* vsync_front_porch */, 13 /* vsync_pulse_width */, 10 /* vsync_back_porch */,
        1 /* pclk_active_neg */, 16000000 /* prefer_speed */, true /* auto_flush */);

    #include "TOUCH\touch.h"
    public :
        void clr(int8_t pSize){ __tft.fillScreen(0); __tft.setTextSize(pSize);}

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
    //void clr(int8_t pSize){ __tft.fillScreen(0); __tft.textEnlarge(pSize);}

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
