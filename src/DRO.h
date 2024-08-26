#pragma once

#include <Arduino.h>
#include <SPI.h>

//#define SPI_HALF_SPEED 1

#define DEBUG
#define RA8875

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

#define PIN_LED GPIO_NUM_26
#define PIN_RA8875_CS GPIO_NUM_5
#define PIN_RA8875_RESET GPIO_NUM_17
#define PIN_CS_X GPIO_NUM_4
#define PIN_CS_Y GPIO_NUM_16
#define PIN_CS_Z GPIO_NUM_13
#define PIN_CS_BROCHE GPIO_NUM_2
#define PIN_SERVO GPIO_NUM_15

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

class AXE{
public:
    static uint32_t TimerDRO, TimerDRO_LAST; //Pour calcul RPM

    void init(uint8_t pName, uint8_t pPas){
        Name = 88 + pName;//char(88) = 'X'
        Absolute =  0;
        Correction = 0;
        Modif = 0;
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
            Last = Absolute;
        }
        Absolute = pValue;
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
    int32_t Absolute;
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