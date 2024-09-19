#pragma once

#ifndef _DRO_H_
#define _DRO_H_

#include <Arduino.h>
#include <SPI.h>
//

//#define SPI_HALF_SPEED 1

#define DEBUG
//ici on definit le type d'ecran
//#define RA8875
//#define SCREEN_ESP32
//dans platformio.ini

#define int32 int32_t
#define uint16 uint16_t
#define int8 int8_t
#define abs(x)(x>0? x: -x)
#define TOGGLE(x) digitalWrite(x, !digitalRead(x))

//#define dans RA8875.h
//SPI	MOSI	MISO	SCLK	CS
//VSPI	GPIO 23	GPIO 19	GPIO 18	GPIO 5
//#define PIN_RA8875_INT GPIO_NUM_4
//#define PIN_TFT_CS GPIO_NUM_0
//#define PIN_RA8875_RESET GPIO_NUM_2
#ifdef READ
    #define PIN_LED GPIO_NUM_26
    #define PIN_RA8875_CS GPIO_NUM_5
    #define PIN_RA8875_RESET GPIO_NUM_17
    #define PIN_CS_X GPIO_NUM_4
    #define PIN_CS_Y GPIO_NUM_16
    #define PIN_CS_Z GPIO_NUM_13
    #define PIN_CS_BROCHE GPIO_NUM_2
    #define PIN_SERVO GPIO_NUM_15
#endif

#define axe_X 0
#define axe_Y 1
#define axe_Z 2
#define axe_BROCHE 3
#define axe_STOP_Y 4

#define MODE_TOUR 0//1

#define PAS_VIS_MERE 6000

//Menu Filetage
#define PAS_DEMMARE -1
#define OK 0
#define ANNULER 1
#define EN_COURS 2
#define CENTRE_X 650
#define CENTRE_Y 240
#define RAYON 129


/// @brief screen_XYZ pour MODE_TOUR == 0 ou sinon Z -> RPM pour Tour
enum ScreenStates {screen_XYZ, screen_RPM, screen_Filetage, screen_Position_Broche, screen_value_Axe};
enum ButtonNames {bnX, bnY, bnZ, bnCX, bnCY, bnCZ, bnCAngle, bnXYZ, bnRPM, bnFILETAGE, bnANGLE,
                 bnANNULE, bnVALIDE, bn0, bn1, bn2, bn3, bn4, bn5, bn6, bn7, bn8, bn9, bnp, bnm, bnpm, bnStopY, bnNULL };
/// @brief variables pour calcul Filetage
// position absolue Y depart , position absolue broche quand Y commence à bouger, pas calculé
/// @return
extern int32_t _posY_Depart, _posBrocheMovY, _pasFiletage;

void m_AfficheXYZ(bool pRefresh, ScreenStates pTypeZ);
void m_Filetage(int16_t pTempo);
void m_videSerial();
/// @brief 
/// @param pAfficheXYZ 
/// @return 
bool m_Get_Serial(bool pAfficheXYZ);

class Point{
    public:
        uint16_t x;
        uint16_t y;
        Point(){}
        Point(uint16_t pX, uint16_t pY){ x = pX; y = pY;}
        bool InRange(Point pPoint, uint16_t pLength, uint16_t pHeight){
            return ((x >= pPoint.x - (pLength / 2)) & (x <= pPoint.x + (pLength / 2)) &
                (y >= pPoint.y - (pHeight / 2) & (y <= pPoint.y + (pHeight / 2))));
        }
};

#ifdef SCREEN_ESP32
    #define FRAM_CS GPIO_NUM_10
    #define FRAM_MOSI GPIO_NUM_11
    #define FRAM_MISO GPIO_NUM_13
    #define FRAM_CLK GPIO_NUM_12

    #include "TFT/BUTTON/button.h"

    void m_ModifValue(int32_t *pValue, String  pButtonName);
#endif

class AXE{
public:
    static uint32_t TimerDRO, TimerDRO_LAST; //Pour calcul RPM

    void init(uint8_t pName, uint8_t pPas){
        Name = 88 + pName;//char(88) = 'X'
        Absolute =  0; //valeur de LS7366
        Correction = 0;//valeur ajoutée pour modifier affichage
        Modif = 0;//pour gestion saisie modification valeur
        Pas = pPas;
        TimerDRO_LAST = 0;
    }
    /// @brief MAJ Correction
    void reset(){
        if (Correction == Absolute){
            Correction = Modif;
        }
        else
        {   
            Modif = Correction;     
            Correction = Absolute;
        }
    }
    void set(int32_t pValue){
        if (pValue != Last){
            Absolute = pValue;
            Last = Absolute;
           // fram.write8(1,1);
        }
    }
    void modif(int32_t pValue){
        Modif = pValue;
    }
    void corrige(int32_t pValue){
        //Serial.println("Corrige");
        Correction = Absolute - (pValue / Pas);
    }
    int32_t get(){
        return (Absolute - Correction)  * Pas;
    }
    uint32_t get_modif(){
        return Modif;
    }
    uint32_t get_RPM(){
        uint32_t result = abs( 1000 * (Absolute - Last)) ;
        result /= (TimerDRO - TimerDRO_LAST);
        TimerDRO_LAST = TimerDRO;
        return result;
    }
    uint32_t get_ANG(){
        uint32_t result = abs((Absolute - Correction) % 3600) * 100;
        return result;
    }

private:
    int32_t FRAM_decalage; //A l'allumage, lit la 
    int32_t Absolute; //Valeur envoyée par LS7366
    int32_t Correction; //pour compenser affichage
    int32_t Modif; //pour afficher en mode modif
    int32_t Last; // pour mémoriser précédente valeur
    int8_t  Pas;
    int8_t  Name;
};

static int16_t m_GCD(int pA, int pB) {
	int v_reste;
	while (pB > 0)
	{
		v_reste = pA % pB;
		pA = pB;
		pB = v_reste;
	}
	return pA;
}

#endif