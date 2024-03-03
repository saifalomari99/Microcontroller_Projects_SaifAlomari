                                                                     //Voltage Meter

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
 
void Set_ADCON0 (char AN_pin);                                       //function to set what A/D channel to use 
void Init_ADC(void);                                                 //Function to initialize the ADC registers
unsigned int get_full_ADC(void);                                     //Function to read the Analog value
void init_UART(); 
void putch (char c) ;                                                //This function is called  anytime you call the function print f
void Init_TRIS();
void DO_DISPLAY_7SEG_Upper(char digit);
void DO_DISPLAY_7SEG_Lower(char digit);
void Delay_One_Sec();                                                //Function to delay 1 second


//============================================== main project =========================================================
void main()                                  
{ 
    Init_ADC();                                                      //initialize the ADC pins
    init_UART();
    Init_TRIS();                                                     //initialize the ports as inputs and outputs
    
    
    //----------------------- start of while loop
    while (1)
    {   
        Set_ADCON0(0);                                               //set channel AN0 so we can read the voltage from the potentiometer
        int ADC_Value = get_full_ADC();                              //start the A/D conversion and save it in number step              
        float VL0 = ((ADC_Value * 4.000) /1000.0) ;                  // Convert ADC value to voltage
             
        //------------- 7-segments display
        int voltage = (int) VL0;     
        float biggerVL0 = VL0 * 10;                                  //for the lower digit
        int secondVoltage = (int) biggerVL0;              
        char U = voltage;                                            //upper digit number
        char L = secondVoltage %10;                                  //lower digit number                              
        
        DO_DISPLAY_7SEG_Upper(U);
        DO_DISPLAY_7SEG_Lower(L);                                    
                 
        printf("VL0 = %f V \r\n\n " , VL0  );            
        
        Delay_One_Sec();
    }//-------------------- end while loop                           
    
    
    
                                                                     
}
//================================================ end ==============================================================
 
void Set_ADCON0 (char AN_pin)                                        //function to set what A/D channel to use 
{                                                                   
    ADCON0 = AN_pin * 0x04 + 1;                                      //Equation to choose the value for ADCON0
} 
void Init_ADC(void)                                                  //Function to initialize the ADC registers
{ 
    ADCON0=0x01;                                                     // select channel AN0, and turn on the ADC subsystem 
    ADCON1=0x1B ;                                                    // select pins AN0 through AN3 as analog signal, VDD-VSS as reference voltage      
    ADCON2=0xA9;                                                     // right justify the result. Set the bit conversion time (TAD) and // acquisition time 
}
unsigned int get_full_ADC(void)                                      //Function to read the Analog value
{ 
    int result;
    ADCON0bits.GO=1;                                                 //Start Conversion 
    while(ADCON0bits.DONE==1);                                       //wait for conversion to be completed 
    result = (ADRESH * 0x100) + ADRESL;                              //combine result of upper byte and lower byte into result 
    
    return result;                                                   //return the result. 
} 
void init_UART() 
{ 
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF & 
    USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &            
    USART_BRGH_HIGH, 25); 
    OSCCON = 0x60; 
} 
void putch (char c)                                                  //This function is called  anytime you call the function print f
{ 
    while (!TRMT);                                                   
    TXREG = c; 
}
void Init_TRIS()
{
    TRISA = 0x0F;                                                    //make portA input   
    TRISB = 0x00;                                                    //make PortB output
    TRISC = 0x00;                                                    //make PortC output
    TRISD = 0x00;                                                    //make PortD output
    TRISE = 0x00;                                                    //make PortE output   
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

void Delay_One_Sec()                                                 //Function to delay 1 second
{ 
 for(int I=0; I <17000; I++); 
 }
