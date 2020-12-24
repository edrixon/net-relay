#include "types.h"
#include "globals.h"

// Configure IO pins
void initPins()
{
  int c;

  for(c = 0; c < MAXPINS; c++)
  {
    pinMode(inputPins[c], INPUT_PULLUP);
    pinMode(outputPins[c] , OUTPUT);
  }

  // Message received LED
  pinMode(LED_RX, OUTPUT);
  digitalWrite(LED_RX, LOW);

  // Alarm relay
  pinMode(ALARM_RELAY, OUTPUT);
  digitalWrite(ALARM_RELAY, LOW);
}

// Initialise serial port command buffer
void initCmdBuff()
{
  cmdBuffIndex = 0;
}

// Initialise network variables
void initNetVars()
{
  readNetVars();
}

// Initialise all system timers
void initTimers()
{
    int c;

    for(c = 0; c < MAXTIMERS; c++)
    {
        resetTimer(c);
    }
}

