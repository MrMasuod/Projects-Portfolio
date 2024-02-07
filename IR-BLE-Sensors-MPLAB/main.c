#include "config.h"
#include <xc.h>
#include <stdint.h>
#include "definitions.h"

void main() {
    OSC_Init();
    setupTimer();
    setup();
    Timer_val_min = (uint16_t)(Number_of_Measurements * 0.05);
    Timer_val_max = (uint16_t)(Number_of_Measurements * 0.95);
    Measured_Period = (unsigned long)(1e6/CommercialPowerSourceFreq);//us
    while(1) {
        GetTimerVal();
        bool is_overflow_timer = (Tmi[0]< Timer_val_min || Tmi[1]< Timer_val_min || Tmi[0] > Timer_val_max || Tmi[1] > Timer_val_max);        
        if(is_overflow_timer){
            ++RangeCountover;
            if(RangeCountover == 10){
                RangeCountover=100;
                PrintNoData();
                if(IVRO == 1){//reset counter
                    count=0;
                    Heart_Rate_Count=0;
                }
            }
            else if(RangeCountover>99){ //Wait 1s
                RangeCountover=100;
                LEDChangeState(1);  //flash Red LED
                __delay_ms(10);
                LEDChangeState(2);
                __delay_ms(1000);
            }
        }
        else{
            RangeCountover=0;
            AbsorptionCoefficentCalc();
            TimerPeriodCheck();
            if(Period==1){
                HeartRateCalc();
                OxygenSPO2Calc();
                ++Heart_Rate_Count;
                MinMaxValReset();
                if(Heart_Rate_Count > 4){
                    if(SpO2_value < 90){
                        Buzzer  = 1;
                    }
                    else{
                        Buzzer = 0;
                    }           
                }
            }
            DataPrinting();
            ++count;
        }
    }
}

