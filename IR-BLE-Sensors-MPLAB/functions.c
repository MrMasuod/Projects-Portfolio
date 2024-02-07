#include <xc.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include "definitions.h"

float exp(float x) {
    float result = 1.0f;
    float term = 1.0f;

    for (int i = 1; i < 10; ++i) {
        term *= x / i;
        result += term;
    }

    return result;
}

void OSC_Init(void) {
    OSCCON = 0b01110000;   // Configure internal oscillator for 8MHz
}

void UART1_Init(unsigned long baud_rate) {
    TRISC6 = 0; // TX pin as output
    TRISC7 = 1; // RX pin as input

    SPBRG1 = (unsigned char)((_XTAL_FREQ / (baud_rate * 16UL)) - 1);

    TXSTA1bits.BRGH = 0; // Low speed mode
    TXSTA1bits.TXEN = 1; // Enable transmission
    TXSTA1bits.SYNC = 0; // Asynchronous mode
    RCSTA1bits.SPEN = 1; // Enable serial port
}

void UART1_Send(const char *data) {
    while (*data != '\0') {
        while (!TXSTA1bits.TRMT);
        TXREG1 = *data;
        ++data;
    }
}

void UART1_SendFormatted(const char *format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    UART1_Send(buffer);
}
float fabs(float num) {
    if (num < 0)
        return -num;
    else
        return num;
}
void setup(void) {
    UART1_Init(BAUD_RATE);
    DeviceInitialization();
    MinMaxValReset();
    StartPrinting();
    ConfigurationCheck();
    FormatForPrint();  
}
void Reset_MCLR(void) {
    MCLR_PIN = 0;
    __delay_ms(10);
    MCLR_PIN = 1;
}
void LEDChangeState(unsigned char State){
  switch(State){
    case 0: // IR On and charge
      IRLED = 0;   // Turn on IRLED
      REDLED = 1;  // Turn off REDLED
      Charge = 0;  // Charge
      break;
    case 1: // RED On and charge
      IRLED = 1;   // Turn off IRLED
      REDLED = 0;  // Turn on REDLED
      Charge = 0;  // Charge
      break;
    case 2: // LED Off and discharge
      IRLED = 1;   // Turn off IRLED
      REDLED = 1;  // Turn off REDLED
      Charge = 1;  // Discharge
      break;
    default:
      // Handle invalid State
      break;
  }
}
void pinMode(unsigned char pin, unsigned char mode) {
    unsigned char mask = (unsigned char)(1 << pin); // Calculate mask for the specified pin

    if (mode == OUTPUT) {
        TRISB &= ~mask; // Set the corresponding bit to 0 for OUTPUT mode
    } else if (mode == INPUT) {
        TRISB |= mask;  // Set the corresponding bit to 1 for INPUT mode
    } else if (mode == INPUT_PULLUP) {
        TRISB |= mask;  // Set the corresponding bit to 1 for INPUT mode
        LATB |= mask;   // Set the pin HIGH for pull-up
    } else if (mode == INPUT_PULLDOWN) {
        TRISB |= mask;  // Set the corresponding bit to 1 for INPUT mode
        LATB &= ~mask;  // Set the pin LOW for pull-down
    }
}
void setupTimer(void) {
    T1CON = 0x80; // Timer1 enabled, 1:1 prescaler
    TMR1H = 0x00; // Initialize timer register
    TMR1L = 0x00;
    T1CONbits.TMR1ON = 1; // Start Timer1
    TMR1IF = 0; // Clear Timer1 interrupt flag
    TMR1IE = 1; // Enable Timer1 overflow interrupt
    PEIE = 1;   // Enable peripheral interrupts
    GIE = 1;    // Enable global interrupts
}

void __interrupt() isr() {
    if (TMR1IF) {
        TMR1IF = 0; // Clear Timer1 interrupt flag
        TMR1H = 0x00; // Reinitialize timer register
        TMR1L = 0x00;
        timerOverflowCount++; // Increment overflow count
    }
}
unsigned long millis(void) {
    unsigned long milliseconds;
    unsigned int tmr1Val;

    do {
        // Read Timer1 value
        tmr1Val = ((unsigned int)TMR1H << 8) | TMR1L;
        // Calculate milliseconds, considering timer overflow
        milliseconds = (timerOverflowCount << 16) + (tmr1Val >> 6);
    } while (TMR1IF); // Ensure no interrupt occurred during the reading

    return milliseconds;
}
unsigned long micros(void) {
    unsigned int timerValue;

    // Calculate the elapsed microseconds
    // (timer overflow count * timer period) + current timer value
    timerValue = (unsigned int)(TMR1L | (TMR1H << 8));
    microseconds = (timerOverflowCount << 16) + (timerValue * 4); // Adjust based on your timer configuration

    return microseconds;
}

void GetTimerVal(void) {
    Tm_prev = Timer[4][0];
    for (LED_Color = 0; LED_Color < 2; ++LED_Color) {
        WaitPeriod();
        LEDChangeState(LED_Color);
        TimerStart = micros(); // You need to implement micros() for PIC18F4620

        for (Tmi[LED_Color] = 0; Tmi[LED_Color] < Number_of_Measurements; ++Tmi[LED_Color]) {
            if (Detect == 1) {
                Timer[count % MovingAverage][LED_Color] = micros() - TimerStart;  
                break;  
            }   
        }

        LEDChangeState(2); // Discharge LEDs
        Timer[4][LED_Color] = 0;
        for (int i = 0; i < MovingAverage; ++i) {
            Timer[4][LED_Color] += Timer[i][LED_Color];
        }
        Timer[4][LED_Color] /= MovingAverage;    
    }
}
void Setting(void) {
    mode = 0;
    WaitReleaseSW1();
    UART1_SendFormatted(" Opt Device Test\r\n");
    UART1_SendFormatted(" Adjust Resistor\r\n");
    
    while (1) {
        if (!SW1) {
            StartTime = millis();
            __delay_ms(200);
            while (!SW1) {
                if (millis() - StartTime > 3000) {
                    break;
                }
            }
            if (millis() - StartTime > 3000) {
                break;
            }
            ++mode;
            mode = mode & 1;
        }
    }
    WaitReleaseSW1();
    if (mode == 0) {
        DetectorTest();
    } else if (mode == 1) {
        ResistorAdjust();
    }
    Reset_MCLR();
}

void WaitPeriod(void) {
    if (KillHumNoize == 1) {
        if (LED_Color == 0) {
            while (1) {
                if (micros() % Measured_Period < 100) {
                    break;
                }
            }
        } else if (LED_Color == 1) {
            while (1) {
                if (micros() % Measured_Period > Measured_Period * 0.3) {
                    break;
                }
            }
        }
    } else {
        __delay_ms(1);
    }
}
//millis wala function
void ConfigurationCheck(void) {
    if (!SW1) { 
        unsigned long startTime = millis(); 
        while (!SW1) {
            if (millis() - startTime > 3000) {
                Setting();
                break;
            }
        }
    }
}
void DeviceInitialization(void){
  // Initialize Serial communication
  UART1_SendFormatted("Initializing Serial Communication...\n");
  UART1_SendFormatted("Baud rate: %d\n", BAUD_RATE);
  UART1_SendFormatted("Setting pinMode for Buzzer, IRLED, REDLED, Charge, SW1, SW2, and Detect...\n");
  pinMode(Buzzer, OUTPUT);
  UART1_SendFormatted("Initializing Buzzer...\n");
  Buzzer = 0;
  pinMode( REDLED, OUTPUT );
  UART1_SendFormatted("Initializing Detect...\n");
  pinMode( Charge, OUTPUT );
  UART1_SendFormatted("Initializing Charge...\n");
  pinMode( SW1, INPUT_PULLUP);
  UART1_SendFormatted("Initializing SW1...\n");  
  pinMode( Detect, INPUT); 
  UART1_SendFormatted("Initializing Detect...\n");  
  UART1_SendFormatted("Setting LED state to 2...\n");
  LEDChangeState(2);
}

void DetectorTest(void) {
  UART1_SendFormatted("Opt Device Test\n");
  UART1_SendFormatted("Push SW1\n");
  while (SW1 == 1) {
  }
  WaitReleaseSW1();
  
  UART1_SendFormatted("Flashing Red LED\n");
  UART1_SendFormatted("OK: Push SW1\n");
  count = 0;
  
  while (SW1 == 1) {
    if (count % 2 == 0) {
      LEDChangeState(1);
      __delay_ms(10);
    } else {
      LEDChangeState(2);
      __delay_ms(100);
    }
    ++count;
  }
  LEDChangeState(2);
  
  UART1_SendFormatted("Checking...\n");
  count = 0;
  GetTimerVal();
  __delay_ms(1000);
  
  if (Detect == 1) {
    UART1_SendFormatted("PhotoTr Error1\n");//PTr output High When All LED OFF
  } else if (Tmi[1] > Number_of_Measurements / 2) {
    UART1_SendFormatted("PhotoTr Error2\n");//PTr output Low When Red LED On
  } else if (Tmi[0] > Number_of_Measurements / 2) {
    UART1_SendFormatted("IR LED Error\n");//PTr output Low When IR LED On
  } else {
    UART1_SendFormatted("OptDevices OK\n");
  }
  
  while (SW1) {
  }
}
void WaitReleaseSW1(void) {
  UART1_SendFormatted("Release SW1\n");
  while (SW1 == 0) {
  }
}
void ResistorAdjust(void) {
  UART1_SendFormatted("Adjust Resistor\n");
  __delay_ms(1000);
  UART1_SendFormatted("\033[H\033[J"); // Clear the terminal screen
  
  while (SW1) {
    GetTimerVal();
    UART1_SendFormatted("IR : %3lu%%\n", 100 * Tmi[0] / Number_of_Measurements);
    UART1_SendFormatted("Red: %3lu%%\n", 100 * Tmi[1] / Number_of_Measurements);
    __delay_ms(100);
  }
}

void AbsorptionCoefficentCalc(void) {
  for (LED_Color = 0; LED_Color < 2; ++LED_Color) {
    Absorption_Coefficents[LED_Color] = 1 / (1 - exp(-Timer[4][LED_Color] / CRus)); // Calculate beta
    if (Absorption_Coefficents[LED_Color] > beta_MinMax[1][LED_Color]) {
      beta_MinMax[1][LED_Color] = Absorption_Coefficents[LED_Color];
    }
    if (Absorption_Coefficents[LED_Color] < beta_MinMax[0][LED_Color]) {
      beta_MinMax[0][LED_Color] = Absorption_Coefficents[LED_Color];
    }
  }
  if (count % 2 == 0) {
    for (int j = 0; j < 2; ++j) {
      Last_beta[count / 2 % ARRAY_SIZE][j] = Absorption_Coefficents[j];
      beta_Average[j] = 0;
      for (int i = 0; i < ARRAY_SIZE; ++i) {
        beta_Average[j] += Last_beta[i][j];
      }
      beta_Average[j] /= ARRAY_SIZE;
    }
  }
}

void StartPrinting(void) {
  UART1_SendFormatted("PulseOximeter\n");
  __delay_ms(3000); // Delay using built-in delay function for PIC
}

void FormatForPrint(void) {
  UART1_SendFormatted("SpO2:    %%\n");
  UART1_SendFormatted("HR:      BPM\n");
  
  if (SerialMode == 1) {
    UART1_SendFormatted("Time,IR,RED,SpO2,SpO2_hysteresis,HR,HR_hysteresis,Period\n");
  } else if (SerialMode == 2) {
    UART1_SendFormatted("IR,RED,SpO2,HR,,Period\n");
  }
}

void DataPrinting(void) {
  if (Period == 1) {
    UART1_SendFormatted("%c", 0); // Assuming the character write is available in your PIC printf
  } else if (PeriodEndCount == 2) {
    UART1_SendFormatted(" ");
  }

  if (Heart_Rate_Count > 4) {
    if (PeriodEndCount == 8) {
      UART1_SendFormatted("%3d", (unsigned char)SpO2_Hysteresis);
      UART1_SendFormatted("%3d", (unsigned char)HR_Hysteresis);
    }
    
    if (SerialMode != 0) {
      if (SerialMode == 1) {
        UART1_SendFormatted("%lu,%f,%f,%f,%d,%f,%d,%d\n", millis(), Absorption_Coefficents[0], Absorption_Coefficents[1], SpO2_value, SpO2_Hysteresis, HR_val, HR_Hysteresis, Period);
      } else {
        UART1_SendFormatted("%f,%f,%d,%d,%d,Time:%lums\n", (1.05 - Absorption_Coefficents[0] / beta_Average[0]) * 250 + 10, (1.05 - Absorption_Coefficents[1] / beta_Average[1]) * 250 + 30, SpO2_Hysteresis, HR_Hysteresis, Period * 10, millis());
      }
    }
  }
}

void PrintNoData(void) {
  UART1_SendFormatted(" - \n");
  UART1_SendFormatted(" - \n");
}

void MinMaxValReset(void) {
  for (int i = 0; i < 2; ++i) {
    beta_MinMax[0][i] = 1e6;
    beta_MinMax[1][i] = -1e6;
  }
}
void OxygenSPO2Calc(void){
  R=(beta_MinMax[1][1]/beta_MinMax[0][1]-1)/(beta_MinMax[1][0]/beta_MinMax[0][0]-1);
  SPO2_OxyLevelArray[Heart_Rate_Count % 4] = 0.3557f * R * R * R - 5.1864f * R * R - 18.342f * R + 108.381f;
  SpO2_value=0;
  for(int i=0;i<4;++i){
    SpO2_value+=SPO2_OxyLevelArray[i]; 
  }
  SpO2_value/=4;
  if(fabs(SpO2_Hysteresis-SpO2_value)>SpO2_Hysteresis_width){
    SpO2_Hysteresis=(unsigned char)(SpO2_value+0.5);
  }
}

void HeartRateCalc(void){
  Timer_HeartRateArray[Heart_Rate_Count%4]=(PeakTime[0]-PeakTime[1]) & 0xFFFF;
  HR_val=0;
  for(int i=0;i<4;++i){
    HR_val+=Timer_HeartRateArray[i]; 
  }
  HR_val=60000*4/HR_val;
  if(fabs(HR_Hysteresis-HR_val)>HR_Hysteresis_width){
    HR_Hysteresis = (unsigned char)(HR_val+0.5);
  }
}


void TimerPeriodCheck(void){
  Period=0;
  detTimer[count%8]=Tm_prev-Timer[4][0];
  average_detTimer8=0;
  for(int i=0;i<8;++i){
    average_detTimer8+=detTimer[i];
  }
  average_detTimer8=average_detTimer8/8;
  if(average_detTimer8<0 && PeriodEndCount>5){//period
    PeakTime[1]=PeakTime[0];
    PeakTime[0]=millis();
    Period=1;
    PeriodEndCount=0;
  }
  else if(average_detTimer8>0){
    ++PeriodEndCount;
  }
}
