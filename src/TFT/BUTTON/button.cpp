#include "button.h"

cBUTTON::cBUTTON(Point pCenter, uint16_t pLength, uint16_t pHeight, byte pTextSize, ButtonNames pName, String pTexte, uint16_t pTextColor, uint16_t pBackColor, void (*pFunction)(int32_t *pValue, String pButtonName ))
{
    center = pCenter;
    height = pHeight;
    length = pLength;
    textSize = pTextSize;
    name = pName;
    texte = pTexte;
    textColor = pTextColor;
    backColor = pBackColor;
    FUNCTION = pFunction;
}
