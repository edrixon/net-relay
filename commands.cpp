#include "types.h"
#include "globals.h"

// Display an IP address on serial port
void dispIp(char *msg, byte *ipAddr)
{
  int c;

  Serial.write(msg);
  for(c = 0; c < 4; c++)
  {
    Serial.print(*ipAddr);
    if(c == 3)
    {
      Serial.write("\r\n");
    }
    else
    {
      Serial.write(".");
    }
    ipAddr++;
  }
}

// Get an IP address from character string
int getIp(char *buffPtr, byte *ipAddr)
{
    int c;
    char *cPtr;
    char *nPtr;
    int rtn;

    rtn = -1;
    c = 0;

    nPtr = strtok_r(buffPtr, ".", &cPtr);
    if(nPtr)
    {
        *ipAddr = atoi(nPtr);
        ipAddr++;
      
        while(c < 3 && *cPtr != '\0')
        {
            nPtr = strtok_r(NULL, ".", &cPtr);
            if(nPtr)
            {
                *ipAddr = atoi(nPtr);
                ipAddr++;
                
                c++;
            }
        }
        
        if(c == 3)
        {
            rtn = 0;
        }       
    }

    return rtn;
}

// Get MAC address from character string
int getMac(char *buffPtr, byte *macAddr)
{
    int c;
    char *cPtr;
    char *nPtr;
    char *endPtr;
    int rtn;

    rtn = -1;
    c = 0;

    nPtr = strtok_r(buffPtr, ":", &cPtr);
    if(nPtr)
    {
        *macAddr = (byte)strtol(nPtr, &endPtr, 16);
        macAddr++;
      
        while(c < 5 && *cPtr != '\0')
        {
            nPtr = strtok_r(NULL, ":", &cPtr);
            if(nPtr)
            {
                *macAddr = (byte)strtol(nPtr, &endPtr, 16);
                macAddr++;
                
                c++;
            }
        }
        
        if(c == 5)
        {
            rtn = 0;
        }       
    }

    return rtn;
}

// Save netVars in EEPROM
void saveCmd(char *params)
{
    netVars.dataValid = DATA_VALID;
    saveNetVars();
    Serial.write("Saved parameters\r\n");
}

// Set or display local IP address
void addrCmd(char *params)
{
  int c;

  if (*params == '\0')
  {
    dispIp((char *)"Local IP address: ", netVars.ipAddr);
  }
  else
  {
    if(getIp(params, &netVars.ipAddr[0]))
    {
        Serial.write("Failed to set IP address\r\n");
    }
  }
}

// Set or display MAC address
void macCmd(char *params)
{
  int c;
  char strBuff[8];
  
  if(*params == '\0')
  {
    Serial.write("MAC address: ");
    for(c = 0; c < 6; c++)
    {
      sprintf(strBuff, "%02x", netVars.macAddr[c]);
      Serial.write(strBuff);
      if(c == 5)
      {
        Serial.write("\r\n");
      }
      else
      {
        Serial.write(":");
      }
    }
  }
  else
  {
      if(getMac(params, &netVars.macAddr[0]))
    {
        Serial.write("Failed to set MAC address\r\n");
    }
  }
}

// Set or display gateway address
void gwCmd(char *params)
{
  if(*params == '\0')
  {
    dispIp((char *)"Gateway address: ", netVars.gwAddr);
  }
  else
  {
    if(getIp(params, &netVars.gwAddr[0]))
    {
        Serial.write("Failed to set gateway address\r\n");
    }
  }
}

// Set or display netmask
void netmaskCmd(char *params)
{
  if(*params == '\0')
  {
    dispIp((char *)"Netmask: ", netVars.netmask);
  }
  else
  {
    if(getIp(params, &netVars.netmask[0]))
    {
        Serial.write("Failed to set netmask\r\n");
    }
  }
}

// Set or display remote IP address
void remoteCmd(char *params)
{
  if(*params == '\0')
  {
    dispIp((char *)"Remote address: ", netVars.remoteIp);
  }
  else
  {
    if(getIp(params, &netVars.remoteIp[0]))
    {
        Serial.write("Failed to set remote address\r\n");
    }
  }
}

// Clear EEPROM contents
void clearCmd(char *params)
{
    Serial.write("Clear EEPROM data\r\n");
    memset(&netVars, 0xff, sizeof(netVars));
    saveNetVars(); 
}

// Display raw EEPROM data
void showCmd(char *params)
{
    dumpEeprom();
}

// List all commands
void helpCmd(char *params)
{
  int c;

  c = 0;
  while(cmdTable[c].cmdName[0] != '\0')
  {
    Serial.write(cmdTable[c].cmdName);
    Serial.write("\r\n");
    c++;
  }
}

// Enable message handling
void goCmd(char *params)
{
    handleMessaging = true;
    Serial.write("Messaging started\r\n");
}

// Disable message handling
void stopCmd(char *params)
{
    handleMessaging = false;
    Serial.write("Messaging stopped\r\n");
}
// Read serial port characters until CR is pressed
boolean getCommand()
{
   int rtn;

  if(Serial.available() == 0)
  {
      return -1;
  }

  rtn = -1;
  while (Serial.available() > 0 && rtn == -1)
  {
    cmdBuff[cmdBuffIndex] = Serial.read();

    switch (cmdBuff[cmdBuffIndex])
    {
      case BS:;
      case DEL:
        if (cmdBuffIndex)
        {
          Serial.write(eraseStr);
          cmdBuffIndex--;
        }
        break;

      case LF:;
        break;

      case CR:
        cmdBuff[cmdBuffIndex] = '\0';
        Serial.write("\r\n");
        rtn = 0;
        break;

      default:
        Serial.write(cmdBuff[cmdBuffIndex]);
        cmdBuffIndex++;
        if (cmdBuffIndex == CMDLEN)
        {
          cmdBuff[cmdBuffIndex - 1] = '\0';
          rtn = 0;
        }
    }
  }

  return rtn;
}

// Validate serial port command and execute handler function
void parseCommand()
{
  int c;
  boolean gotCmd;
  char *params;

  cmdBuffIndex = 0;

  c = 0;
  gotCmd = false;
  while(gotCmd == false && cmdTable[c].cmdName[0] != '\0')
  {
      if(strncmp(cmdTable[c].cmdName, cmdBuff, strlen(cmdTable[c].cmdName)) == 0)
      {
          gotCmd = true;
      }
      else
      {
        c++;
      }
  }

  if(gotCmd == true)
  {
    params = cmdBuff;
    while(*params != '\0' && *params != '=')
    {
      params++;
    }
    if(*params == '=')
    {
      params++;
    }
    
    cmdTable[c].cmdFn(params);
  }
  else
  {
    Serial.write("Error\r\n");
  }
}

