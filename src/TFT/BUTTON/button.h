#include <Arduino.h>
#include "..\..\DRO.h"

#pragma once

#ifndef _BUTTON_
#define _BUTTON_

class cBUTTON{
public:
    static byte NbrButtons; 
    cBUTTON(){}
    cBUTTON(Point pCenter, uint16_t pLength, uint16_t pHeight, byte ptextSize, ButtonNames pName, String pTexte, uint16_t pTextColor, uint16_t pBackColor, void (*pFunction)(int32_t *pValue, String  pButtonName ));

    bool Actif;
    Point center;
    uint16_t height;
    uint16_t length;
    byte textSize;
    ButtonNames name;
    String texte;
    uint16_t textColor;
    uint16_t backColor;
    void (* FUNCTION)(int32_t *pValue, String  pButtonName );
};

#endif
