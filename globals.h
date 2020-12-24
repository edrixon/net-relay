#ifdef __IN_MAIN

// System timers
timer_t timers[MAXTIMERS] = 
{
  { 0, TICKS_LED },
  { 0, TICKS_DEAD },
  { 0, TICKS_MSG }
};

// Digital input pins to be read
byte inputPins[MAXPINS] = { 2, 3, 4, 5};

// Output pins to be written - each drives a relay
// pin 10 is used by Ethernet chip, so can't use that...
byte outputPins[MAXPINS] = { 8, 9, 6, 11 };

// String to send when BS or DEL key is pressed
char eraseStr[4] = { BS, ' ', BS, '\0' };

// Serial port command definitions
cmd_t cmdTable[] =
{
    { (char *)"addr", addrCmd },
    { (char *)"clear", clearCmd },
    { (char *)"go", goCmd },
    { (char *)"gw", gwCmd },
    { (char *)"help", helpCmd },
    { (char *)"mac", macCmd },
    { (char *)"netmask", netmaskCmd },
    { (char *)"remote", remoteCmd },
    { (char *)"save", saveCmd },
    { (char *)"show", showCmd },
    { (char *)"stop", stopCmd },
    { (char *)"?", helpCmd },
    { (char *)"", 0 },
};

ioMsg_t inBuffer;             
ioMsg_t outBuffer;                  
boolean handleMessaging;             
int msgDataSize;                 
int cmdBuffIndex;                      
char cmdBuff[CMDLEN];
EthernetUDP udp;
netVars_t netVars;

#else

extern timer_t timers[]; 
extern byte inputPins[];
extern byte outputPins[];
extern char eraseStr[];
extern cmd_t cmdTable[];
extern ioMsg_t inBuffer;             
extern ioMsg_t outBuffer;                  
extern boolean handleMessaging;             
extern int msgDataSize;                 
extern int cmdBuffIndex;                      
extern char cmdBuff[];
extern EthernetUDP udp;
extern netVars_t netVars;

#endif


