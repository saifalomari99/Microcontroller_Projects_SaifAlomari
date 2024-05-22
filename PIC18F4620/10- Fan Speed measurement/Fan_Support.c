
#include <p18f4620.h>
#include "stdio.h"
#include "Fan_Support.h"
#include "utils.h"

char FAN;
extern char duty_cycle;

//==================== Get RPM Value ===========================================
int get_RPM()
{
    int RPS = TMR3L / 2;                         //read the count. Since there are 2 pulses per rev then RPS = count /2                                                                           
    TMR3L = 0;                                   //clear out the count               
    return (RPS * 60);                           //return RPM = 60 * RPS                          
}

//==================== Toggle Fan ==============================================
void Toggle_Fan()
{
    if (FAN == 0)                             
        Turn_On_Fan();
    else if (FAN == 1)
        Turn_Off_Fan(); 
}

//==================== Turn On Fan =============================================
void Turn_On_Fan()
{
    FAN_EN=1;
    do_update_pwm(duty_cycle);
    FAN_EN_LED=1;
    FAN=1;  
}
//==================== Turn Off Fan ============================================
void Turn_Off_Fan()
{
    FAN_EN=0;
    FAN_EN_LED=0;
    FAN=0;
}


//==================== Increase Speed ==========================================
void Increase_Speed()
{
    if (duty_cycle == 100)
    {
        Do_Beep();
        Do_Beep();
        do_update_pwm(100);
    }
    else
    {
        duty_cycle = duty_cycle + 5; 
        do_update_pwm(duty_cycle);
    }
  
}

//==================== Decrease Speed ==========================================
void Decrease_Speed()
{
    if (duty_cycle == 0)
    {
        Do_Beep();
        Do_Beep();
        do_update_pwm(0);
    }
    else
    {
        duty_cycle=duty_cycle - 5;
        do_update_pwm(duty_cycle);
    }  
 
}
//==================== Update PWM ==============================================
void do_update_pwm(char duty_cycle) 
{ 
    float dc_f; 
    int dc_I; 
    
    PR2 = 0b00000100 ;                             // set the frequency for 25 Khz 
    T2CON = 0b00000111 ;                            // 
    dc_f = ( 4.0 * duty_cycle / 20.0) ;            // calculate factor of duty cycle versus a 25 Khz signal
                                         
    dc_I = (int) dc_f;                                      // get the integer part 
    if (dc_I > duty_cycle) dc_I++;                         // round up function 

    CCP1CON = ((dc_I & 0x03) << 4) | 0b00001100; 
    CCPR1L = (dc_I) >> 2; 

}







//--------------------- values for the RGB Lights D1 and D2
#define  D1OFF 0x00
#define  D1R   0x08       
#define  D1G   0x10
#define  D1B   0x20
#define  D1Y   0x18
#define  D1C   0x30
#define  D1P   0x28
#define  D1W   0x38

#define  D2OFF 0x00
#define  D2R   0x02
#define  D2G   0x04
#define  D2Y   0x06
#define  D2B   0x08
#define  D2C   0x0c
#define  D2P   0x0a
#define  D2W   0x0e

char D1RGB[8] = { D1OFF,D1R,D1G,D1Y,D1B,D1P,D1C,D1W };
char D2RGB[8] = { D2OFF,D2R,D2G,D2Y,D2B,D2P,D2C,D2W };

//========================== set D1 ============================================
void Set_DC_RGB(int duty_cycle)
{
    char Duty_cycle= duty_cycle/10;
    if(Duty_cycle >= 7)  
        PORTB = D1W;
    else
        PORTB=D1RGB[Duty_cycle] ;
    
}

//============================ set D2 ==========================================
void Set_RPM_RGB(int rpm)
{
    int RPM = rpm/500 + 1;
    
    if(rpm == 0)
    {
        PORTD = (PORTD & 0x70) | D2RGB[RPM];
    }        
    else 
    {    
        PORTD = (PORTD & 0x70) | D2RGB[RPM];          //0111 0000 --- logic OR the values of D1 and D3 arrays to display button color on correct LED without conflict     
    }  
}