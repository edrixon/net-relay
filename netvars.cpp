#include "types.h"
#include "globals.h"

void readNetVars()
{  
  int eepromAddr; 
  byte *bPtr;

  eepromAddr = 0;
  bPtr = (byte *)&netVars;

  while (eepromAddr < sizeof(netVars_t))
  {
    *bPtr = EEPROM.read(eepromAddr);

    bPtr++;
    eepromAddr++;
  }
}

// Save network variables in EEPROM
void saveNetVars()
{
  int eepromAddr;
  byte *bPtr;

  eepromAddr = 0;
  bPtr = (byte *)&netVars;

  while (eepromAddr < sizeof(netVars_t))
  {
    EEPROM.write(eepromAddr, *bPtr);

    bPtr++;
    eepromAddr++;
  }  
}

void dumpEeprom()
{
    int eepromAddr;
    int c;
    char strBuff[8];

    eepromAddr = 0;
    while(eepromAddr < 256)
    {
        sprintf(strBuff, "%04x  ", eepromAddr);
        Serial.write(strBuff);
        for(c = 0; c < 8; c++)
        {
            sprintf(strBuff, "%02x ", EEPROM.read(eepromAddr + c));
            Serial.write(strBuff);
        }
        Serial.write("- ");
        for(c = 8; c < 16; c++)
        {
            sprintf(strBuff, "%02x ", EEPROM.read(eepromAddr + c));
            Serial.write(strBuff);
        }
        Serial.write("\r\n");
        eepromAddr = eepromAddr + 16;
    }
}
