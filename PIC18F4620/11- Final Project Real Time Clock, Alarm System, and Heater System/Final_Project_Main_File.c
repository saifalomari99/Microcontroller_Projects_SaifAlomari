

          ////////////////////////////////////////////////////////////////////////////////
          //                                                                            //
          //                       ECE3301 Lab Session 1                                //
          //        Final Project (Real Time Clock, Alarm System, Heater System)        //           
          //                    Computer Engineer: SAIF ALOMARI                         //
          //                                                                            //
          ////////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>
#include <string.h>

#include "Fan_Support.h"                                 // (1)
#include "I2C.h"                                         // (2)        
#include "I2C_Support.h"                                 // (3)
#include "Interrupt.h"                                   // (4)
#include "Main.h"                                        // (5)
#include "Main_Screen.h"                                 // (6)
#include "ST7735_TFT.h"                                  // (7)
#include "utils.h"                                       // (8)

#include "Setup_Time.h"                                  // (9)
#include "Setup_Alarm_Time.h"                            // (10)
#include "Setup_Fan_Temp.h"                              // (11)


#pragma config OSC = INTIO67
#pragma config BOREN =OFF
#pragma config WDT=OFF
#pragma config LVP=OFF

#pragma config CCP2MX = PORTC                            //CPP2 Register to send the right frequency to Buzzer



//-------- Time declarations
char second = 0x00;
char minute = 0x00;
char hour = 0x00;
char dow = 0x00;
char day = 0x00;
char month = 0x00;
char year = 0x00;

//-------- Setup new times declarations
char found;
char tempSecond = 0xff; 
signed int DS1621_tempC, DS1621_tempF;
char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;
char alarm_second, alarm_minute, alarm_hour, alarm_date;
char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;

//--------- declarations that are used across the files
unsigned char fan_set_temp = 75;
float volt;
char INT1_flag, INT2_flag;
char INT0_flag;
short Nec_OK ;
char  Nec_Button;

//-------- integers that are being changes and checked in the main file
char FAN;
char duty_cycle;
int rps;
int rpm;
int ALARMEN;
int alarm_mode, MATCHED, color;
int RGB_D3_Counter = 0;

//-------- Screen initialization strings
char buffer[31]     = " ECE3301L Sp'23 Final\0";
char *nbr;
char *txt;
char tempC[]        = "+25";
char tempF[]        = "+77";
char time[]         = "00:00:00";
char date[]         = "00/00/00";
char alarm_time[]   = "00:00:00";
char Alarm_SW_Txt[] = "OFF";
char Fan_Set_Temp_Txt[] = "075F";
char Fan_SW_Txt[]   = "OFF";                                        //text storage for Heater Mode

//---------- the 21 Buttons data
char array1[21]={0xa2,0x62,0xe2,
                 0x22,0x02,0xc2,
                 0xe0,0xa8,0x90,
                 0x68,0x98,0xb0,
                 0x30,0x18,0x7a,
                 0x10,0x38,0x5a,
                 0x42,0x4a,0x52};

//---------- Updated values and strings on the LCD
char DC_Txt[]       = "000";                                       //text storage for Duty Cycle value
char Volt_Txt[]     = "0.00V";
char RTC_ALARM_Txt[]= "0";                                         
char RPM_Txt[]      = "0000";                                      //text storage for RPM

char setup_time[]       = "00:00:00";
char setup_date[]       = "01/01/00";
char setup_alarm_time[] = "00:00:00"; 
char setup_fan_set_text[]   = "075F";


//-------- Functions that are used in the main file
void test_alarm();
void Monitor_Fan();
int get_duty_cycle(signed int temp, int set_temp);
void putch (char c);
void init_UART();


//======================= Initilize all ports Function 
void Do_Init()                                                   // Initialize the ports 
{  
    init_UART();                                                 // Initialize the uart
    Init_ADC();
    OSCCON=0x70;                                                 // Set oscillator to 8 MHz 
    
    //------- Ports initialization:
    TRISA = 0x01;                                                //PORTA = 0000 0001.
                                                                 //                   RA0 = (Input) PHOTO_RESISTOR (Analog)                   
    TRISB = 0x07;                                                //PORTB = 0000 0111. First 3 bits are inputs
                                                                 //                   RB0 = INT0
                                                                 //                   RB1 = IR Receiver INT1
                                                                 //                   RB2 = RTC_MATCH# of the Timer
    TRISC = 0x01;                                                //PORTC = Output, bit0 is input (TACH_PULSE)
    TRISD = 0x00;                                                //PORTD = Output
    TRISE = 0x00;                                                //PORTE = Output (D3 RGB)
    
    
    
    //------- Initial Values
    PORTE = 0x00;                                                //clear D3
    FAN = 0; 
    RBPU=0;
    TMR3L = 0x00;                   
    T3CON = 0x03;
    I2C_Init(100000); 

    DS1621_Init();
    Init_Interrupt();                                            //Initialize the Interrupt
    Turn_Off_Fan();
    fan_set_temp = 75;
    rpm = 0;
}



//================================== Main ============================================================
void main() 
{
    Do_Init();                                                  // Initialization     
    DS3231_Turn_Off_Alarm();                                    
    DS3231_Read_Alarm_Time();                                   // Read alarm time for the first time
    Initialize_Screen();
    T3CON = 0x03;                                               //Initilize Timer3 to be used for the RPM of the fan
    TMR3L = 0x00;
    
    tempSecond = 0xff;
    //-------------------- start of While loop
    while (1)
    {
        DS3231_Read_Time();
        Clicked_LED = 0;

        if(tempSecond != second)                               //check every second
        {
            tempSecond = second;
            
            //------- Update the values every second
            rpm = get_RPM();                                   //1) Measure the fan?s rpm
            volt = read_volt();                                //2) Measure the voltage of the light sensor
            DS1621_tempC = DS1621_Read_Temp();                 //3) Measure the ambient temperature
            DS1621_tempF = (DS1621_tempC * 9 / 5) + 32;
            
            //------- update the D1 and D2 RGBs
            Set_DC_RGB(duty_cycle);
            Set_RPM_RGB(rpm);
            
            //------- Tera Term 
            printf ("%02x:%02x:%02x %02x/%02x/%02x",hour,minute,second,month,day,year);
            printf (" Temp = %d C = %d F ", DS1621_tempC, DS1621_tempF);
            //printf ("alarm = %d match = %d", RTC_ALARM_NOT, MATCHED);
            printf ("RPM = %d  duty_cycle = %d\r\n", rpm, duty_cycle);
            
            
            Monitor_Fan();                                    //Check the status of the temperature and monitor the heater
            test_alarm();                                     //Check the status of the Alarm time every second
            
            Update_Screen();                                  //Update the screen every second with the new values
        }
        
        if (check_for_button_input() == 1)                    //check if a button is pressed (utils.c)      
        {        
            switch (found)
            {
                case Setup_Time_Key:        
                    Do_Beep_Good();
                    Do_Setup_Time();
                    break;
                
                case Setup_Alarm_Key:           
                    Do_Beep_Good();
                    Do_Setup_Alarm_Time();
                    break;
                    
                case Setup_Fan_Temp_Key:
                    Do_Beep_Good();
                    Setup_Temp_Fan();            
                    break;
                    
                case Toggle_Fan_Key:
                    Do_Beep_Good();
                    Toggle_Fan();
                    break;           
        
                default:
                    Do_Beep_Bad();
                    break;
            }
        }   
        
        if (INT0_flag == 1)                                   //If the push button interrupt flag is 1, then Enable the Alarm
        {
            INT0_flag = 0;
            if (ALARMEN == 0) 
                ALARMEN = 1;
            else 
                ALARMEN = 0;
        }
        
        
        
        
    }//end while
}
//============================== end main ===========================================================




//======================= Main Functions =======================================
void putch (char c)
{   
    while (!TRMT);       
    TXREG = c;
}
void init_UART()
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 25);
    OSCCON = 0x70;
}


//============================== Test the Alarm Function =======================
void test_alarm()
{
    // add code here
    if (alarm_mode == 0 && ALARMEN == 1)                         //If the alarm is enabled, go to the next state 
    {
        // activate the alarm
        DS3231_Turn_On_Alarm();
        alarm_mode = 1;
    }
    else if (alarm_mode == 1 && ALARMEN == 0)                    //Exiting the alarm mode state
    {
        // disable the alarm
        DS3231_Turn_Off_Alarm();
        alarm_mode = 0;
        MATCHED = 0;
        Deactivate_Buzzer();
        PORTE = 0x00;                                            //clear D3
        
    }
    else if (alarm_mode == 1 && ALARMEN == 1)                    //If the alarm is enabled and and mode is on, stay in this state until the actual time equals the alarm time
    {               
        // check if the alarm time matches with the actual time
        if (RTC_ALARM_NOT == 0 )                                 //if the time matches, this signal is coming from the Real Time Clock Chip
        {
            MATCHED = 1;
        }
        else
        {
            MATCHED = 0;
        }
    }  
    
    
    
    //--------- if MATCHED flag is triggered  
    if (MATCHED == 1)
    {
        if (volt >= 3.0 || INT0_flag == 1)                       //if you cover the photo sensor or press the push button
        {
            MATCHED = 0;
            Deactivate_Buzzer();
            PORTE = 0x00;                                        //clear D3
            
            if (volt >= 3.0)                                     //Snooze
                DS3231_Turn_Off_Alarm();                         //This resets the RTC_ALARM_NOT signal         
        }
        else                                                     //The alarm will go off
        {
            Activate_Buzzer();
            Set_RGB_D3_Color(RGB_D3_Counter);                    //Update the D3 RGB color with different color every second
            
            if (RGB_D3_Counter < 7)
            {
                RGB_D3_Counter++;         
            }
            else
            {
                RGB_D3_Counter = 0;
            }       
                
        }
        
    }//end if statement    

               
}//end function


//==================== Monitor the fan and check temperature ===================
void Monitor_Fan()
{
    duty_cycle = get_duty_cycle(DS1621_tempF,  fan_set_temp);          //check the temperatures
    do_update_pwm (duty_cycle);                                        //update the speed of the fan
    
    //-------- if the temperature goes under the set temperature of the heater, (turn on the Heater)
    if (DS1621_tempF <= fan_set_temp)                                   
    { 
        FAN = 1;
    }
    
    if (FAN == 0) Turn_Off_Fan();                                       
    else if (FAN == 1)Turn_On_Fan();           
}




//=========================== Control the Fan Speed ============================
int get_duty_cycle(signed int temp, int set_temp)                    //Change the speed of the fan regarding the difference of between the set temp and the actual temp
{  
    int duty_cycle;
    
    if (temp >= set_temp)
    {
        duty_cycle = 0;
    }
    else                                                            //(temp < set_temp) Turn on the heater
    {
        float diff_temp = set_temp - temp;
         
        if (diff_temp > 50) duty_cycle = 100;
        else if (diff_temp >= 35 && diff_temp < 50) duty_cycle = diff_temp * 2;
        else if (diff_temp >= 25 && diff_temp < 35) duty_cycle = diff_temp * 3 / 2;
        else if (diff_temp >= 10 && diff_temp < 25) duty_cycle = diff_temp * 5 / 4;
        else if (diff_temp >= 0 && diff_temp < 10)  duty_cycle = diff_temp;              
    }
        
    return duty_cycle;
}

