#include <stdbool.h>
#include <stdint.h>

void UART1_Init(unsigned long baud_rate);
void UART1_Send(const char *data);
void UART1_SendFormatted(const char *format, ...);
void OSC_Init(void);
float exp(float x);
float fabs(float num);
void setup(void);
void Reset_MCLR(void);
void LEDChangeState(unsigned char State);
void pinMode(unsigned char pin, unsigned char mode);
void setupTimer(void);
unsigned long millis(void);
unsigned long micros(void);
void WaitPeriod(void);
void ConfigurationCheck(void);
void DeviceInitialization(void);
void Setting(void);
void DetectorTest(void);
void WaitReleaseSW1(void);
void ResistorAdjust(void);
void StartPrinting(void);
void FormatForPrint(void);
void DataPrinting(void);
void PrintNoData(void);
void MinMaxValReset(void);
void AbsorptionCoefficentCalc(void);
void HeartRateCalc(void);
void OxygenSPO2Calc(void);
void TimerPeriodCheck(void);
void GetTimerVal(void);

#define _XTAL_FREQ 8000000
#define MCLR_PIN PORTEbits.MCLR
#define TX_PIN PORTCbits.RC6
#define RX_PIN PORTCbits.RC7
#define INPUT 1
#define OUTPUT 0
#define INPUT_PULLUP 2
#define INPUT_PULLDOWN 3
#define ARRAY_SIZE 32
#define CRus 100000 //Time constant 
#define Buzzer  PORTBbits.RB5
#define SW1 PORTBbits.RB7
#define Detect PORTBbits.RB0
#define Charge PORTBbits.RB1
#define IRLED PORTBbits.RB2
#define REDLED PORTBbits.RB3
#define BAUD_RATE 9600
#define SerialMode 2  //0:None 1:SerialMonitor  2:SerialPloter
#define Number_of_Measurements  16384
#define IVRO 1 //0:disable  1:enable
#define MovingAverage 3
#define KillHumNoize 1
#define CommercialPowerSourceFreq 50 // change this according to your supply line frequency either 50 or 60
#define SpO2_Hysteresis_width 1
#define HR_Hysteresis_width 2
uint16_t Timer_HeartRateArray[4];
float HR_val;
unsigned char HR_Hysteresis;
float R;
float SPO2_OxyLevelArray[4];
float SpO2_value;
unsigned char SpO2_Hysteresis;
unsigned long count=0;      //Count of measurements
unsigned long Heart_Rate_Count=0;    //Count of heart beat
unsigned char PeriodEndCount=0;
unsigned char RangeCountover;
unsigned long Tmi[2];
float Timer[5][2];
float Tm_prev;
float detTimer[8];
float average_detTimer8;
unsigned char LED_Color = 0; //0:IR 1:RED
unsigned long TimerStart;
unsigned long PeakTime[2];
float Absorption_Coefficents[2]; //Transmission coefficient
float Last_beta[32][2];  //
float beta_Average[2];
float beta_MinMax[2][2]; //[Min or Max][IR or Red]
bool Period;
volatile unsigned long timerOverflowCount = 0; // Counter for timer overflows
volatile unsigned long microseconds = 0;        // Microseconds count
unsigned char mode = 0;
unsigned long Measured_Period;
unsigned long StartTime;
unsigned long Timer_val_min;
unsigned long Timer_val_max;
unsigned char Heart[] = { 0b00000, 0b01010, 0b11111, 0b11111, 0b11111, 0b01110, 0b00100, 0b00000};


