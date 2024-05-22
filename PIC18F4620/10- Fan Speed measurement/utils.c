
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <p18f4620.h>
#include "utils.h"





void Do_Beep()
{
    Activate_Buzzer();                                                             
    Wait_One_Sec();                                                          
    Deactivate_Buzzer(); 
    //do_update_pwm(duty_cycle);
}


//================================ Buzzer ======================================
void Activate_Buzzer()                                            //Function to activate the Buzzer
{
    PR2 = 0b11111001 ;
    T2CON = 0b00000101 ;                                             
    CCPR2L = 0b01001010 ;                                            
    CCP2CON = 0b00111100 ;
    //PORTCbits.RC1 = 1;
}
void Deactivate_Buzzer()                                          //Function to deactivate the buzzer
{
    CCP2CON = 0x0;
	PORTCbits.RC1 = 0;
}


void Wait_One_Sec()
{
    for (int k=0;k<30000;k++);
}

char bcd_2_dec (char bcd)
{
    char dec;                                       //changed this from int to char -------------------------------------------
    dec = ((bcd>> 4) * 10) + (bcd & 0x0f);
    return dec;
}

int dec_2_bcd (char dec)
{
    int bcd;
    bcd = ((dec / 10) << 4) + (dec % 10);
    return bcd;
}








//void Wait_Half_Second()
//{
//    T0CON = 0x03;                                                 //Timer 0, 16-bit mode, prescaler 1:16
//    TMR0L = 0xDB;                                                 //set the lower byte of TMR
//    TMR0H = 0x0B;                                                 //set the upper byte of TMR
//    INTCONbits.TMR0IF = 0;                                        //clear the Timer 0 flag
//    T0CONbits.TMR0ON = 1;                                         //Turn on the Timer 0
//    while (INTCONbits.TMR0IF == 0);                               //wait for the Timer Flag to be 1 for done
//    T0CONbits.TMR0ON = 0;                                         //turn off the Timer 0
//}              
//void Wait_One_Second_With_Beep()                                  //creates one second delay as well as sound buzzer
//{                                                                                      
//    Activate_Buzzer();                                                             
//    Wait_Half_Second();                                              
//            
//    Deactivate_Buzzer ();                                                          
//    Wait_Half_Second();                                                            
//}