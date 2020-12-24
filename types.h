#include <SPI.h>
#include <Ethernet2.h>
#include <EEPROM.h>

// ASCII characters
#define BS  0x08
#define CR  0x0d
#define LF  0x0a
#define DEL 0x7f

// Number of inputs handled
#define MAXPINS 4

// EEPROM verification value
#define DATA_VALID 0x55aa

// TImer initial values
#define TICKS_LED 2            // Persist 'message sent' and 'message received' LED
#define TICKS_MSG 10           // Time between sending update messages
#define TICKS_DEAD 300         // Time before raising alarm when no messages received
#define TICK_TIME 100          // milliseconds between ticks

#define MAXTIMERS 3            // Number of timers

// Timer ID numbers
#define TIMER_RXLED 0
#define TIMER_DEAD 1
#define TIMER_MSG   2

// Size of serial port command line
#define CMDLEN 80

// Values for 'high' and 'low' in messages - not 0 and 1 so checksum is useful
#define MSG_HIGH 0xaa
#define MSG_LOW  0x55

// UDP port number to use - send and receive use the same port
#define PORTNUMBER 1000

// Send and receive LED pins
#define LED_RX      13

// Pin with alarm relay on it
#define ALARM_RELAY 12

// Serial port command handler function prototypes
void addrCmd(char *params);
void gwCmd(char *params);
void netmaskCmd(char *params);
void remoteCmd(char *params);
void macCmd(char *params);
void saveCmd(char *params);
void showCmd(char *params);
void stopCmd(char *params);
void goCmd(char *params);
void clearCmd(char *params);
void helpCmd(char *params);

// Data stored in EEPROM
typedef struct
{
    byte macAddr[6];                     // MAC address
    byte ipAddr[4];                      // This board's IP address
    byte netmask[4];                     // netmask
    byte gwAddr[4];                      // Gateway address
    byte remoteIp[4];                    // Remote board's IP address
    unsigned short int dataValid;        // Validation of EEPROM data
} netVars_t;

// Checksum in messages
typedef unsigned short int cSum_t;

// Structure of update messages
typedef struct
{
    unsigned short int sequence;          // A sequence number, incremented with each message
    byte pinState[MAXPINS];               // State of each input pin
    cSum_t checksum;                      // Message checksum
} ioMsg_t;

// Timer data
typedef struct
{
    unsigned short int count;             // Current value  
    unsigned short int initValue;         // Reset value
} timer_t;

// Serial port command table entry
typedef struct
{
    char *cmdName;                        // ASCII command
    void (*cmdFn)(char *);                // Handler function
} cmd_t;


void readNetVars(void);
void saveNetVars(void);
void dumpEeprom(void);
void resetTimer(int timer);
void initCmdBuff(void);
void initNetVars(void);
void initPins(void);
void initTimers(void);
boolean getCommand(void);
void parseCommand(void);
void decrementTimers(void);
boolean timerExpired(int timer);

