

//##############################################################################
//                             Utility Source File                            //
//##############################################################################

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

#include <p18f4620.h>
#include "utils.h"
#include "Fan_Support.h"

extern char found;
//extern char Nec_code1;
extern char Nec_Button;
extern short Nec_OK;
extern char array1[21];
extern char duty_cycle;


//========================= Function to check button input =====================
char check_for_button_input()
{       
    if (Nec_OK == 1)
    {
            Clicked_LED = 1;
            Nec_OK = 0;

             //printf ("NEC_Code = %x\r\n", Nec_code1);

            INTCON3bits.INT1IE = 1;          		                      //Enable external interrupt
            INTCON2bits.INTEDG1 = 0;        		                      //Edge programming for INT1 falling edge

            found = 0xff;
            for (int j=0; j< 21; j++)
            {
                if (Nec_Button == array1[j]) 
                {
                    found = j;
                    j = 21;
                }
            }
            printf ("Found = %d \r\n\n", found);
            
            //Activate_Buzzer();
            Wait_One_Sec();
            //Deactivate_Buzzer();
            Clicked_LED = 0;
            
            if (found == 0xff) 
            {
                printf ("Cannot find button \r\n");
                return (0);
            }
            else
            {
                return (1);
            }
            
            
    }
}






//========================== Do_Beep Functions =================================
void Do_Beep()
{
    Activate_Buzzer();
    Wait_One_Sec();
    Deactivate_Buzzer();
    Wait_One_Sec();
    do_update_pwm(duty_cycle);
}

void Do_Beep_Good()                           
{
    //add code here using Activate_Buzzer_2KHz()
    Activate_Buzzer_2KHz();
    Wait_One_Sec();
    Deactivate_Buzzer();
    Wait_One_Sec();
    do_update_pwm(duty_cycle);
}

void Do_Beep_Bad()                                
{
    //add code here using Activate_Buzzer_500Hz()
    Activate_Buzzer_500Hz();
    Wait_One_Sec();
    Deactivate_Buzzer();
    Wait_One_Sec();
    do_update_pwm(duty_cycle);
}

void Wait_One_Sec()
{
    for (int k=0;k<0xffff;k++);
    //for (int k=0;k<30000;k++);
}


//================================= Buzzer Functions ===========================
void Activate_Buzzer()
{
    PR2 = 0b11111001 ;           //0xF9
    T2CON = 0b00000101 ;         //0x05
    CCPR2L = 0b01001010 ;        //0x4A
    CCP2CON = 0b00111100 ;       //0x3C
}

void Activate_Buzzer_500Hz()                            ////////////////////////////////////
{
    // add code here
    PR2 = 0xf9;
    T2CON = 0x07;
    CCPR2L = 0x7C;
    CCP2CON = 0x3C;
}

void Activate_Buzzer_2KHz()                               //////////////////////////////////
{
    // add code here
    //Fosc = 8000000 Hz
    //Fpwm = 2000.00 Hz (Requested : 2000 Hz)
    //Duty Cycle = 50 %
    //PR2 = 0b11111001 ;
    //T2CON = 0b00000101 ;
    //CCPR1L = 0b01111100 ;
    //CCP1CON = 0b00111100 ;
    PR2 = 0x3E;
    T2CON = 0x04;
    CCPR2L = 0x1F;
    CCP2CON = 0x3C;
}

void Activate_Buzzer_4KHz()                                 //////////////////////////////
{
    // add code here
    //Fosc = 8000000 Hz
    //Fpwm = 4000.00 Hz (Requested : 2000 Hz)
    //Duty Cycle = 50 %
    PR2 = 0b01111100 ;
    T2CON = 0b00000101 ;
    CCPR1L = 0b00111110 ;
    CCP1CON = 0b00011100 ;
}

void Deactivate_Buzzer()                                    /////////////////////////////////
{
    CCP2CON = 0x0;
	PORTCbits.RC1 = 0;
}





//========================== Update the Fan Speed ==============================
char bcd_2_dec (char bcd)              //function to covert BCD to decimal
{
    int dec;
    dec = ((bcd>> 4) * 10) + (bcd & 0x0f);
    return dec;
}
int dec_2_bcd (char dec)               //function to covert decimal to BCD
{
    int bcd;
    bcd = ((dec / 10) << 4) + (dec % 10);
    return bcd;
}


////============================== Update the PWM of the FAN =====================
//void do_update_pwm(char duty_cycle) 
//{ 
//	float dc_f;
//	int dc_I;
//    
//	PR2 = 0b00000100 ;                      // Set the frequency for 25 Khz 
//	T2CON = 0b00000111 ;                    // As given in website
//	dc_f = ( 4.0 * duty_cycle / 20.0) ;     // calculate factor of duty cycle versus a 25 Khz signal
//	dc_I = (int) dc_f;                      // Truncate integer
//	if (dc_I > duty_cycle) dc_I++;          // Round up function
//	CCP1CON = ((dc_I & 0x03) << 4) | 0b00001100;
//	CCPR1L = (dc_I) >> 2;
//}





void Set_RGB_D3_Color(char color)
{
    if ((color > 7)  || (color <= 0))
    {
        PORTE = 0x00;
    }
    else 
    {
        PORTE = color;
    }
    
}




//======================== Analog to Digital Functions =========================
unsigned int get_full_ADC()
{
    // add code here
    int result;
    ADCON0bits.GO=1;                                               //Start Conversion 
    while(ADCON0bits.DONE==1);                                     //wait for conversion to be completed 
    result = (ADRESH * 0x100) + ADRESL;                            //combine result of upper byte and lower byte into result     
    return result;                                                 //return the result.
}
void Init_ADC()                                                 ////////////////////////////////////////////////
{
    // add code here
    ADCON0= 0x01;                                                     //select channel AN0, and turn on the ADC subsystem 
    ADCON1= 0x0E;                                                     //set all the A inputs to digital inputs (0's and 1's), but keep AN0 analog      
    ADCON2= 0xA9;                                                     //right justify the result. Set the bit conversion time (TAD) and // acquisition time  
}
float read_volt()                                             //////////////////////////////////////////////
{
    // add code here
    int ADC_Value = get_full_ADC();                              //start the A/D conversion and save it in number step         
    float voltage = ((ADC_Value * 4.000) /1000.0) ;              //the equation to get the Voltage value in volts
    return voltage;
}


