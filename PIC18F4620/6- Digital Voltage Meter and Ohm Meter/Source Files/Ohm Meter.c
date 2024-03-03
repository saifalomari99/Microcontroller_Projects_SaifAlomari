                                                                     //Ohm Meter with 1K ohm resistor

#include <stdio.h>                                                   //all libraries needed 
#include <stdlib.h> 
#include <xc.h> 
#include <math.h> 
#include <p18f4620.h>
#include <usart.h>

#pragma config OSC = INTIO67                                         //device configrations
#pragma config WDT = OFF 
#pragma config LVP = OFF 
#pragma config BOREN = OFF      

//---------- main functions to initialize and set and the pins
void Set_ADCON0 (char AN_pin);                                       //Function to set what A/D channel to be used
void Init_ADC(void);                                                 //Function to initialize the ADC registers 
unsigned int get_full_ADC(void);                                     //Function to read the Analog value
void init_UART();                                                    //Function to initialize the Universal Asynchronous Receiver/Transmitter (UART) module in a microcontroller.
void putch (char c);                                                 //Function to send a single character over the UART module of a microcontroller. for (Tera Term)
Init_TRIS();                                                         //Function to initialize the ports to inputs and outputs
void Delay_One_Sec();                                                //Function to delay 1 second

//--------- 7 segments display functions
void DO_DISPLAY_7SEG_Upper(char digit);
void DO_DISPLAY_7SEG_Lower(char digit);
#define DP_Of_Upper PORTDbits.RD7                                    //Define the decimal point of the upper digit

//--------- Lights control Functions
#define D1_RED PORTEbits.RE0                                         //Define the port pins to the RGB light
#define D1_GREEN PORTEbits.RE1
#define D1_BLUE PORTEbits.RE2
SET_D1_RED();                                                        //Functions to set the colors of the RGB
SET_D1_PURPLE();
SET_D1_OFF();
void Display_D1(int resistance);                                     //Function to display D1 RGB Light
#define D2_RED PORTCbits.RC3                                         //Define the port pins to the RGB light
#define D2_GREEN PORTCbits.RC4
#define D2_BLUE PORTCbits.RC5
SET_D2_WHITE();                                                      //Functions to set the colors of the RGB
SET_D2_CYAN();
SET_D2_BLUE();                                                       
SET_D2_YELLOW();
SET_D2_GREEN();
SET_D2_RED();                                                        
SET_D2_OFF(); 
void Display_D2(int resistance);                                     //Function to display D2 RGB Light

//----------- Buzzer Control functions
void Activate_Buzzer();                                              //Function to Activate the Buzzer
void Deactivate_Buzzer();                                            //Function to Deactivate Buzzer
//void Control_Buzzer(int resistance);

//============================================== main project =========================================================
void main()                                                                                      
{ 
    Init_ADC();                                                      //initialize the ADC pins
    init_UART();
    Init_TRIS();                                                     //initialize the ports as inputs and outputs
    
    //----------------------- start of while loop
    while(1)
    {   
        ADCON0 = 0x09;                                               //set channel AN2 so we can read the VL1
        int ADC_Value = get_full_ADC();                              //start the A/D conversion and save it in number step         
        float VL1 = ((ADC_Value * 4.000) /1000.0) ;                  //the equation to get the Voltage value in volts
        
        float Rref = 1.000;                                          //Rref = 1K ohm
        float Vref = 4.096;                                          //Vin = 4.096 volts       
        float RL = (Rref * VL1)/(Vref - VL1);                        //The equation to get the Resistor value in Kohms    
        
        //------------- 7-segments display      
        float biggerRL = RL * 10;                                    //for the lower digit
        int BiggerRL0 = (int) biggerRL;                              //switch the value to int                    
        if (RL < 10.0)                                               //if RL is less than 10K, then display with decimal point
        {
            char U = BiggerRL0 /10;    
            char L = BiggerRL0 % 10;
            DO_DISPLAY_7SEG_Upper(U);
            DO_DISPLAY_7SEG_Lower(L);           
        } 
        else                                                         //if RL is more than 10K, then turn off the decimal point
        {
            BiggerRL0 = BiggerRL0 /10;                               //bring it back to the original number
            char U = BiggerRL0 /10;    
            char L = BiggerRL0 % 10;
            DO_DISPLAY_7SEG_Upper(U);
            DO_DISPLAY_7SEG_Lower(L);
            DP_Of_Upper = 1;                                         //Turn off the Dp
        }       
        
        //------------------- Tera Term Display
        printf("VL1 = %f V \r\n " , VL1  );                          //The voltage value
        printf("Resistor value = %f Kohm \r\n\n\n " , RL  );         //The resistor value
        
        //----------------- RGB Lights Display
        Display_D2(RL);                  
        Display_D1(biggerRL);
        
        //---------------- Buzzer Sound Display     
        if (RL < 0.07)                                               //if RL is less than 70 ohms, turn on the Buzzer
        {
            Activate_Buzzer();
            Delay_One_Sec();           
        }
        if (RL > 0.07)                                               //if RL is bigger than 70 ohms, turn off the Buzzer
        {
            Deactivate_Buzzer();                                                        
        } 
        
        Delay_One_Sec();
    }//-------------------- end while loop                           
    
 
}                                                                    
//================================================ end main ==============================================================


void Set_ADCON0 (char AN_pin)                                        //function to set what A/D channel to use 
{                                                                    
    ADCON0 = AN_pin * 0x04 + 1;                                      //Equation to choose the value for ADCON0
} 
void Init_ADC(void)                                                  //Function to initialize the ADC registers
{ 
    ADCON0=0x01;                                                     //select channel AN0, and turn on the ADC subsystem 
    ADCON1=0x1B ;                                                    //select pins AN0 through AN3 as analog signal, VDD-VSS as reference voltage      
    ADCON2=0xA9;                                                     //right justify the result. Set the bit conversion time (TAD) and // acquisition time 
}
unsigned int get_full_ADC(void)                                      //Function to read the Analog value
{ 
    int result;
    ADCON0bits.GO=1;                                                 //Start Conversion 
    while(ADCON0bits.DONE==1);                                       //wait for conversion to be completed 
    result = (ADRESH * 0x100) + ADRESL;                              //combine result of upper byte and lower byte into result     
    return result;                                                   //return the result. 
}
void init_UART()                                                     //Function to initialize the Universal Asynchronous Receiver/Transmitter (UART) module in a microcontroller.
{ 
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF & 
    USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & 
    USART_BRGH_HIGH, 25); 
    OSCCON = 0x60; 
} 
void putch (char c)                                                  //Function to send a single character over the UART module of a microcontroller. for (Tera Term)
{ 
    while (!TRMT); 
    TXREG = c; 
}
Init_TRIS()                                                          //Function to initialize the ports to inputs and outputs
{
    TRISA = 0x0F;                                                    //make portA input 
     
    TRISB = 0x00;                                                    //make PortB output
    TRISC = 0x00;                                                    //make PortC output
    TRISD = 0x00;                                                    //make PortD output
    TRISE = 0x00;                                                    //make PortE output   
}
void Delay_One_Sec()                                                 //Function to delay 1 second
{
    for(int I=0; I <17000; I++); 
}

void DO_DISPLAY_7SEG_Upper(char digit)
{
    char array[10] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10};
    PORTD = array[digit];
}
void DO_DISPLAY_7SEG_Lower(char digit)
{
    char array[10] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10};
    PORTB = array[digit];   
}

//================================ D1 LED Code =====================================
SET_D1_RED()
{
    D1_RED = 1;
    D1_GREEN = 0;
    D1_BLUE = 0;    
}
SET_D1_PURPLE()
{
    D1_RED = 1;
    D1_GREEN = 0;
    D1_BLUE = 1;  
}
SET_D1_OFF()
{
    D1_RED = 0;
    D1_GREEN = 0;
    D1_BLUE = 0; 
}

void Display_D1(int resistance) 
{
    if (resistance > 0.100 )
        SET_D1_OFF();
    else if (resistance >= 0.07 && resistance <= 0.100)
        SET_D1_RED(); 
    else
        SET_D1_PURPLE();   
}

//================================= D2 LED Code ====================================
SET_D2_WHITE()
{
    D2_RED = 1;
    D2_GREEN = 1;
    D2_BLUE = 1;
}
SET_D2_CYAN()
{
    D2_RED = 0;
    D2_GREEN = 1;
    D2_BLUE = 1;      
    
}
SET_D2_BLUE()
{
    D2_RED = 0;
    D2_GREEN = 0;
    D2_BLUE = 1;
}
SET_D2_YELLOW()
{
    D2_RED = 1;     
    D2_GREEN = 1;
    D2_BLUE = 0;
}
SET_D2_GREEN()
{
    D2_RED = 0;
    D2_GREEN = 1;
    D2_BLUE = 0;
    
}
SET_D2_RED()
{
    D2_RED = 1;
    D2_GREEN = 0;
    D2_BLUE = 0;
    
}
SET_D2_PURPLE()
{
    D2_RED = 1;
    D2_GREEN = 0;
    D2_BLUE = 1;
    
}
SET_D2_OFF()
{
    D2_RED = 0;
    D2_GREEN = 0;
    D2_BLUE = 0; 
}

void Display_D2(int resistance) 
{
    if(resistance < 10) 
        SET_D2_OFF();
    else if(resistance >=10 && resistance <=19) 
        SET_D2_RED();     
    else if(resistance >=20 && resistance <= 29) 
        SET_D2_GREEN();    
    else if(resistance >=30 && resistance <=39) 
        SET_D2_YELLOW();    
    else if(resistance >=40 && resistance <=49) 
        SET_D2_BLUE(); 
    else if(resistance >=50 && resistance <=59) 
        SET_D2_PURPLE();    
    else if(resistance >=60 && resistance <=69) 
        SET_D2_CYAN(); 
    else   
        SET_D2_WHITE();
}


//================================= Buzzer =====================================
void Activate_Buzzer() 
{ 
    PR2 = 0b11111001 ; 
    T2CON = 0b00000101 ; 
    CCPR2L = 0b01001010 ; 
    CCP2CON = 0b00111100 ; 
    PORTCbits.RC1 = 1;
} 

void Deactivate_Buzzer() 
{ 
    CCP2CON = 0x0; 
    PORTCbits.RC1 = 0; 
} 
