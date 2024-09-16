#pragma once 

#include <Arduino.h>
#include "..\DRO.h"
#include "BUTTON\button.h"

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
/*
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
*/
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

    #include <Adafruit_GFX.h>
    #include "Adafruit_RA8875.h"
    #define BLACK RA8875_BLACK
    #define WHITE RA8875_WHITE
#endif
#ifdef SCREEN_ESP32
    #include <Arduino_GFX_Library.h>
    #include "TOUCH\TAMC_GT911.h"

    class Arduino_RPi_DPI_RGBPanel_extended : public Arduino_RPi_DPI_RGBPanel {
        public:
            Arduino_RPi_DPI_RGBPanel_extended(
            Arduino_ESP32RGBPanel *bus,
            int16_t w, uint16_t hsync_polarity, uint16_t hsync_front_porch, uint16_t hsync_pulse_width, uint16_t hsync_back_porch,
            int16_t h, uint16_t vsync_polarity, uint16_t vsync_front_porch, uint16_t vsync_pulse_width, uint16_t vsync_back_porch,
            uint16_t pclk_active_neg = 0, int32_t prefer_speed  = GFX_NOT_DEFINED, bool auto_flush = true);

            void touchInit();
            //retourne coordonnées du Touch ou 0,0 sinon
            void touchGet(Point *pPoint);

            //pour compatibite avec RA8875    
            void graphicsMode();
            void textMode();
            void textEnlarge(int pSize);
            void textSetCursor(uint16 pX, uint16 pY);
            ///////////////////////
        private :
            TAMC_GT911 _ts = TAMC_GT911(TOUCH_GT911_SDA, TOUCH_GT911_SCL, TOUCH_GT911_INT, TOUCH_GT911_RST, max(TOUCH_MAP_X1, TOUCH_MAP_X2), max(TOUCH_MAP_Y1, TOUCH_MAP_Y2)); 
   };
    //pour compatibite avec RA8875   
    void LCD_CmdWrite(uint8_t p);
    void LCD_DataWrite(uint8_t p);
    int LCD_DataRead();
    int LCD_StatusRead();
    #define cSetD6 0
    #define cSetD7 0
    ///////////////////////////////
#endif

class TFT{
#ifdef RA8875
    Adafruit_RA8875 __tft = Adafruit_RA8875(PIN_TFT_CS, PIN_TFT_RESET);
    public:
        void clr(int8_t pSize){ __tft.fillScreen(0); __tft.textEnlarge(pSize);}
#endif
#ifdef SCREEN_ESP32
    #define GFX_BL          DF_GFX_BL // default backlight pin, you may replace DF_GFX_BL to actual backlight pin
    #define RA8875_BLACK    BLACK
    #define RA8875_WHITE    WHITE
    #define RA8875_RED      RED
    #define RA8875_GREEN    GREEN
    #define RA8875_BLUE     BLUE
    #define TFT_BL 2
    #define uchar u_int8_t    

    #define FRAM_CS GPIO_NUM_10
    #define FRAN_MOSI GPIO_NUM_11
    #define FRAM_MISO GPIO_NUM_13
    #define FRAM_CLK GPIO_NUM_12

    Arduino_ESP32RGBPanel bus = Arduino_ESP32RGBPanel(
        GFX_NOT_DEFINED /* CS */, GFX_NOT_DEFINED /* SCK */, GFX_NOT_DEFINED /* SDA */,
        41 /* DE */, 40 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
        14 /* R0 */, 21 /* R1 */, 47 /* R2 */, 48 /* R3 */, 45 /* R4 */,
        9 /* G0 */, 46 /* G1 */, 3 /* G2 */, 8 /* G3 */, 16 /* G4 */, 1 /* G5 */,
        15 /* B0 */, 7 /* B1 */, 6 /* B2 */, 5 /* B3 */, 4 /* B4 */
    );
    Arduino_RPi_DPI_RGBPanel_extended __tft = Arduino_RPi_DPI_RGBPanel_extended(
        &bus,
        800 /* width */, 0 /* hsync_polarity */, 210 /* hsync_front_porch */, 30 /* hsync_pulse_width */, 16 /* hsync_back_porch */,
        480 /* height */, 0 /* vsync_polarity */, 22 /* vsync_front_porch */, 13 /* vsync_pulse_width */, 10 /* vsync_back_porch */,
        1 /* pclk_active_neg */, 16000000 /* prefer_speed */, true /* auto_flush */);

    public :
        void clr(int8_t pSize){ __tft.fillScreen(0); __tft.setTextSize(pSize);}

#endif

public:
    bool Init();
    //pTypeZ 0=Fraiseuse, 1=RPM, 2=Filetage, 3=Position_Broche    pModifAxe si <>99 , affiche valeur de l'axe en couleur inversée lors modif valeur
    void MenuMain(int32_t pX, int32_t pY, int32_t pZ, bool pRefresh, uint8_t pTypeZ, int32_t pStopY, byte pModifAxe);
    void MenuValue(uint8_t pAxe);
    int8_t MenuFiletage(uint8_t pIndexMenu, bool pRefresh, int32_t pPosX, int32_t pPosY, int32_t pPosBroche);//retourne si OK
    ButtonNames IfTouched(ScreenStates pScreenState, uint16_t pDelay);//(uint16_t pNbrColonnes, uint8_t pNbrLignes, uint16_t pDelay, bool pWriteTXT);//retourne le nom du button ou NULL
    void print(String pS){__tft.print(pS);}
    void printTXT(char* pText, uint16 pX, uint16 pY, uint16_t pTextColor, uint16_t pBackColor, int pSize);
    cBUTTON *Buttons[5][30];
    cBUTTON *ButtonLast;
    void Buttons_Init();
    void Buttons_Draw(ScreenStates pScreenState);
    void DessineButton(cBUTTON pButton, bool pInverted);

private:
    void Buttons_Init(ScreenStates pScreenState, byte pNbr, cBUTTON *pButton, ...);
    cBUTTON button_b1, button_b2, button_b3, button_b4, button_b5, button_b6, button_b7, button_b8, button_b9, button_b0,
            button_bmoins, button_bplus, button_bXYZ, button_bRPM, button_bFILETAGE, button_bANGLE, 
            button_bCX, button_bVX, button_bCY, button_bVY, button_bCZ, button_bVZ, button_bCANGLE,
            button_bANNULE, button_bVALIDE, button_StopY, button_bplusmoins;
    Point _Touched;

    void Active_Window(uint16_t XL, uint16_t XR, uint16_t YT, uint16_t YB);
    void Chk_Busy(void);
    void Clear_Active_Window(void);
    void Memory_Clear(void);
    void Text_Background_Color(uchar setR, uchar setG, uchar setB);
    void Text_Foreground_Color(uchar setR, uchar setG, uchar setB);
    void toTXT(int32_t pValue, bool pPoint);
    void Button(uint16_t pX, uint16_t pY, uint8_t pSize, uint16_t pTextColor, uint16_t pBackColor,  char* pText, uint8_t pDecalageY);
    void SetTextColor(uint16_t pColor, bool pFront);
    void SetTextColor32(uint16_t pFColor, uint16_t pBColor);
    void Calcul_Coordonnees(double pValue);

    bool m_readTouch();
    void m_dessinerClavier();
    void m_deblocageCodePIN();
};

extern TFT _tft;
