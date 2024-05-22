
          ////////////////////////////////////////////////////////////////////////////////
          //                                                                            //
          //                       ECE3301 Lab Session 1                                //
          //        Fan Speed measurement with Speed control through PWM                //           
          //                   Computer Engineer: SAIF ALOMARI                          //
          //                                                                            //
          ////////////////////////////////////////////////////////////////////////////////
                                                                                             
#include <stdio.h>                                                //all libaraies needed
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>                                             
#include <usart.h>
#include <string.h>
 
#include "ST7735_TFT.h"                                           //include header files
#include "Interrupt.h"

#include "I2C.h"                                                  //Header files for the I2C Bus
#include "I2C_Support.h" 

#include "Fan_Support.h"                                          //Header files for the fan control
#include "utils.h" 

#pragma config OSC = INTIO67                                      //Initilize the PIC18F4620                              
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF

char tempSecond = 0xff; 
char second = 0x00; 
char minute = 0x00; 
char hour = 0x00; 
char dow = 0x00; 
char day = 0x00; 
char month = 0x00; 
char year = 0x00; 
char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year; 
char alarm_second, alarm_minute, alarm_hour, alarm_date; 
char setup_alarm_second, setup_alarm_minute, setup_alarm_hour; 

char duty_cycle;                                                  //duty cycle of the fan speed

extern unsigned char Nec_State = 0;                               //Variables for the remote control Infra red info
short Nec_OK ;
char  Nec_Button; 

#define _XTAL_FREQ  8000000                                       //Set operation for 8 Mhz

//------------ main functions                             
void putch (char c);                   
void init_UART();                                                      

//------------ LCD Screen
void Initialize_Screen();                                                                                     

//-------------------- define sizes and text in the screen 
#define Circle_Size     20                                        //Size of Circle for Light
#define Circle_X        60                                        //Location of Circle
#define Circle_Y        80                                        //Location of Circle
#define Text_X          52
#define Text_Y          77
#define TS_1            1                                         //Size of Normal Text
#define TS_2            2                                         //Size of Big Text
char buffer[31];                                                  //general buffer for display purpose
char *nbr;                                                        //general pointer used for buffer
char *txt;

char array1[21]={0xa2, 0x62, 0xe2,                                //array of the data inputs of the buttons on the remote control                                                           
                 0x22, 0x02, 0xc2,                                             
                 0xe0, 0xa8, 0x90,
                 0x68, 0x98, 0xb0,
                 0x30, 0x18, 0x7a,
                 0x10, 0x38, 0x5a,
                 0x42, 0x4a, 0x52 
};				                             
char txt1[21][4] ={"CH-\0", "CH \0", "CH+\0",                     //array of the text on the buttons                                
                   "|<<\0", ">>|\0", ">||\0",                                   
                   " - \0", " + \0", "EQ \0",                                   
                   " 0 \0", "100\0", "200\0",                                   
                   " 1 \0", " 2 \0", " 3 \0", 
                   " 4 \0", " 5 \0", " 6 \0", 
                   " 7 \0", " 8 \0", " 9 \0"                            
};									           


//-------------------- LCD colors                                 //colors for the LCD
#define RD               ST7735_RED
#define BU               ST7735_BLUE
#define GR               ST7735_GREEN
#define MA               ST7735_MAGENTA
#define BK               ST7735_BLACK
#define CN               ST7735_CYAN
//-------------------- arrays of colors                  
int color[21]={RD, RD, RD,                                                                        
               CN, CN, GR,                                                     
               BU, BU, MA,
               BK, BK, BK,                                                      
               BK, BK, BK,
               BK, BK, BK,                                        
               BK, BK, BK
};


//-------------------- define colors values                       //colors values for the RGBs
#define D1Red           0x08                                                   
#define D1Green         0x10                                                                                                                          
#define D1Cyan          0x30                     

#define D2Blue          0x08                                                    
#define D2Magenta       0x0a                                                             
#define D2Black         0x00                                                                         
#define D2White         0x0e
                                                 
#define D3White         0x07                                   
			                          		             
int colorD1[21]={D1Red , D1Red , D1Red,                           //D1 RGB                                                
                 D1Cyan,D1Cyan,D1Green,                                                       
                  0x00 , 0x00 , 0x00,
                  0x00 , 0x00 , 0x00,
                  0x00 , 0x00 , 0x00,
                  0x00 , 0x00 , 0x00,
                  0x00 , 0x00 , 0x00
};
int colorD2[21]={0x00   , 0x00  , 0x00,                           //D2 RGB                                                    
                 0x00   , 0x00  , 0x00,                                                      
                 D2Blue , D2Blue, D2Magenta,
                 D2White,D2White, D2White,
                 0x00   , 0x00  , 0x00, 
                 0x00   , 0x00  , 0x00, 
                 0x00   , 0x00  , 0x00 
};
int colorD3[21]={0x00   , 0x00  , 0x00,                           //D3 RGB                                                    
                 0x00   , 0x00  , 0x00,                                                      
                 0x00   , 0x00  , 0x00,
                 0x00   , 0x00  , 0x00,
                 D3White   , D3White  , D3White, 
                 D3White   , D3White  , D3White, 
                 D3White   , D3White  , D3White                   
};

//================================ main code ===================================
void main()                                                       
{
    init_UART(); 
    					             
    OSCCON = 0x70;                                                //8 Mhz
    nRBPU = 0;                                                    //Enable PORTB internal pull up resistor 
	TRISB = 0x07;							                      //PORTB as output and input
    TRISC = 0x01;                                                 //PORTC as output    
    TRISD = 0x00;                                                 //PORTD as output   
    TRISE = 0x00;                                                 //PORTE as output
    TRISA = 0x00;
    ADCON1 = 0x0F;                                                //Digital inputs for portB and portA
  
    Initialize_Screen();                                          //Initialize the LCD screen
    
    TMR3L = 0;                                                    //Initialize Timer3 to count the pulses per rev
    T3CON = 0x03;                                                 //0000 0111
    
    I2C_Init(100000);                                             //Initialize the I2C Bus
    DS1621_Init();                                                //Initialize the Digital Temp Sensor
        
    Init_INTERRUPT();                                             //initilize the interrupt pins
    
    TMR1H = 0;                                                    //Reset Timer1
    TMR1L = 0;                                                    //
    PIR1bits.TMR1IF = 0;                                          //Clear timer 1 interrupt flag
    PIE1bits.TMR1IE = 1;                                          //Enable Timer 1 interrupt
    INTCONbits.PEIE = 1;                                          //Enable Peripheral interrupt
    INTCONbits.GIE = 1;                                           //Enable global interrupts
    
    Nec_OK = 0;                                                   //Clear flag
    Nec_code = 0x0;                                               //Clear code  
    
    
    FAN_EN = 0;                                                   //Set the fan off when you turn on the PIC18F4620 
    FAN_PWM = 1; 
    FAN_EN_LED = 0;
    duty_cycle = 50; 
    do_update_pwm(duty_cycle) ;   
    int rpm = 0;
    
    //------------------------------- Start while loop -------------------------
    while(1)
    {  
        Clicked_LED = 0;
        DS3231_Read_Time(); 
        if(tempSecond != second) 
        { 
            tempSecond = second; 
            char tempC = DS1621_Read_Temp(); 
            char tempF = (tempC * 9 / 5) + 32; 
            rpm = get_RPM();
            
            Set_DC_RGB(duty_cycle);
            Set_RPM_RGB(rpm);
            
            printf ("\nduty_cycle = %d \r\n" , duty_cycle );
            printf ("rpm = %d \r\n\n" , rpm );
            printf ("%02x:%02x:%02x %02x/%02x/%02x",hour,minute,second,month,day,year); 
            printf (" Temperature = %d degreesC = %d degreesF\r\n", tempC, tempF); 
        } 

        
        
             
        
        //-------------------------- if you detect a click of a button           
        if (Nec_OK == 1)                                          //if you detect a press of a button with all the states of the sequnce
        {
            Nec_OK = 0;
            Enable_INT_Interrupt();                               //Enable INT1 pin
            Clicked_LED = 1; 
            printf ("Nec_button = %x \r\n", Nec_Button);    
  
            //--------------- for loop to get the number of the button being pressed (total of 21 buttons)
            char found = 0xff;                                      
            for(int i = 0; i < 21; i++)                           //loop to identify location of remote button using previously defined arrays
            {
                if(Nec_Button == array1[i])                       //if Nec_code data bits match the code in the array that indexes the Nec_code data byte hex value  
                {                                                 //with the location of the remote buttons
                    found = i;                                    //assign variable found with that index (location) 
                }                   
            }        
            printf ("Found = %d \r\n\n", found);                          
            
            if (found == 0)             //set up the starting time
            { 
                DS3231_Setup_Time();
            }
                    
            //---------------------- if a button is detected
            if (found != 0xff)                                                          
            {
                //---- display on the LCD
                fillCircle(Circle_X, Circle_Y, Circle_Size, color[found]); 
                drawCircle(Circle_X, Circle_Y, Circle_Size, ST7735_WHITE);  
                drawtext(Text_X, Text_Y, txt1[found], ST7735_WHITE, ST7735_BLACK,TS_1);
                
                //---- display on the RGB
                PORTB = colorD1[found];
                PORTD = (PORTD & 0x70) | colorD2[found];          //0111 0000 --- logic OR the values of D1 and D3 arrays to display button color on correct LED without conflict
                PORTE = colorD3[found]; 
                             			
                //Do_Beep();                                      //Activate the buzzer for 1 second
                Activate_Buzzer();
                Wait_One_Sec();
                Deactivate_Buzzer();
                Clicked_LED = 0;
                
                //------------------ Control the Fan
                if (found == 5)                                   //play/pause
                {
                    Toggle_Fan();
                }
                if (found == 6)                                   // (-)
                {
                    Decrease_Speed();
                }
                if (found == 7)                                   // (+)
                {
                    Increase_Speed();
                }
                
                                                               
            }                                         
        }
     
        Wait_One_Sec();
    }//end while
    
    
}
//==================================== end main ================================

void putch (char c)                                               //This function is called  anytime you call the function print f
{ 
    while (!TRMT); 
    TXREG = c;                                                       
}
void init_UART() 
{ 
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &                 
    USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & 
    USART_BRGH_HIGH, 25); 
    OSCCON = 0x60; 
}                                                                 

//============================= Initialize the LCD =============================
void Initialize_Screen()
{
    LCD_Reset();
    TFT_GreenTab_Initialize();
    fillScreen(ST7735_BLACK);
  
    /* TOP HEADER FIELD */
    txt = buffer;
    strcpy(txt, "ECE3301L Sp23-S1/T13");  
    drawtext(2, 2, txt, ST7735_WHITE, ST7735_BLACK, TS_1);

    strcpy(txt,  "lab 12");  
    drawtext(50, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
}