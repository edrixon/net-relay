
//
//  Works with Velleman KA05 I/O board on Leonard ETH Arduino board
//  first four inputs on one board drive first four relays on remote
//  Grounding an input activates a relay
//  Fifth relay is an alarm relay which activates if no messages received for 30 seconds
//  Last relay is not fitted to board - LED indicates messages are being received
//  Serial port used for configuration commands at 9600bd
//  KA05 needs mofifying so that pin 6 can be used as an output to drive relay 3 instead of using pin 10
//  because pin 10 is connected to the Ethernet controller
//  Can be done by re-positioning one end of R11 to connect T3 to pin 6 instead of pin 10 - also remove R24, LD11 and D5
//  


#define __IN_MAIN

#include "types.h"
#include "globals.h"

// Calculate checksum for message
unsigned long int calcChecksum(byte *bPtr)
{
    cSum_t cSum;
    int bCount;

    bCount = sizeof(ioMsg_t) - sizeof(cSum_t);

    cSum = 0;
    while(bCount)
    {
        cSum = cSum + *bPtr;
        bCount--;
        bPtr++;
    }

    return cSum;
}

// Read inputs and send update message to remote board
void sendPinStates()
{
    int c;
    int dataSize;

    Serial.write("[");
    Serial.print(outBuffer.sequence, DEC);
    Serial.write("] Send update: ");
    for(c = 0; c < MAXPINS; c++)
    {
        if(digitalRead(inputPins[c]) == HIGH)
        {
            Serial.write(" 1 ");
            outBuffer.pinState[c] = MSG_HIGH;
        }
        else
        {
            Serial.write(" 0 ");
            outBuffer.pinState[c] = MSG_LOW;
        }
    }
    Serial.write("\r\n");

    outBuffer.sequence++;

    outBuffer.checksum = calcChecksum((byte *)&outBuffer);

    udp.beginPacket(netVars.remoteIp, PORTNUMBER);
    udp.write((byte *)&outBuffer, sizeof(ioMsg_t));
    udp.endPacket();
}

// Set relay states based on message received from remote board
void setRelayStates()
{
    int c;
    unsigned long int cSum;
    byte *bPtr;

    // Calculate message checksum
    cSum = calcChecksum((byte *)&inBuffer);

    // Handle message if checksum matches received one
    if(cSum == inBuffer.checksum)
    {
        Serial.write("[");
        Serial.print(inBuffer.sequence, DEC);
        Serial.write("] Got update: ");
        for(c = 0; c < MAXPINS; c++)
        {
            // Grounding remote input turns relay on
            if(inBuffer.pinState[c] == MSG_LOW)
            {
                Serial.write(" 0 ");
                digitalWrite(outputPins[c], HIGH);
            }
            else
            {
                Serial.write(" 1 ");
                digitalWrite(outputPins[c], LOW);
            }
        }
        Serial.write("\r\n");
    }
    else
    {
        Serial.write("Bad checksum received\r\n");
    }
}

// Initialise everything
void setup()
{
  byte dns[4] = { 8, 8, 8, 8 };
  
  Serial.begin(9600);
  initCmdBuff();

  initNetVars();

  initPins();

  msgDataSize = sizeof(ioMsg_t) - sizeof(unsigned long int);
  outBuffer.sequence = 0;

  // DNS is not used but need to put something in to be able to specify gateway and netmask
  Ethernet.begin(netVars.macAddr, netVars.ipAddr, dns, netVars.gwAddr, netVars.netmask);
  udp.begin(PORTNUMBER);

  initTimers();

  handleMessaging = true;
}

// Msin loop
void loop() {

  int inBytes;

  // See if there's a serial port command
  if (getCommand() == 0)
  {
    // Handle command
    cmdBuffIndex = 0;
    parseCommand();
  }
  else
  {
    // Handle system timers
    decrementTimers();

    // Make sure messaging is enabled
    if(netVars.dataValid == DATA_VALID && handleMessaging == true)
    {
        // Turn off 'message received' LED if timer has expired
        if(timerExpired(TIMER_RXLED) == true)
        {
            digitalWrite(LED_RX, LOW);
        }

        // Activate alarm relay if no messages have been received
        if(timerExpired(TIMER_DEAD) == true)
        {
            digitalWrite(ALARM_RELAY, HIGH);
        }

        // Send update message if timer has expired
        if(timerExpired(TIMER_MSG) == true)
        {
            sendPinStates();
            resetTimer(TIMER_MSG);
        }

        // See if there's a message from remote board
        if(udp.parsePacket())
        {
           inBytes = udp.available();
           if(inBytes)
           {
                // Turn 'message received' LED on
                digitalWrite(LED_RX, HIGH);
                resetTimer(TIMER_RXLED);

                // Reset alarm relay and timer
                digitalWrite(ALARM_RELAY, LOW);
                resetTimer(TIMER_DEAD);

                // Read all received data
                udp.read((byte *)&inBuffer, inBytes);

                // If message length is valid, handle message
                if(inBytes == sizeof(ioMsg_t))
                {
                    setRelayStates();
                }
                else
                {
                    Serial.write("Wrong message size received\r\n");
                }
           }
        }

        // Wait one system tick
        delay(TICK_TIME);
    }
  }
}
