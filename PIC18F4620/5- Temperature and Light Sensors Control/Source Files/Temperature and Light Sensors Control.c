                                                                        // Temperature and Light sensors

#include <stdio.h>                                                      //all libraries needed 
#include <stdlib.h> 
#include <xc.h> 
#include <math.h> 
#include <p18f4620.h>                                                  
#include <usart.h>

#pragma config OSC = INTIO67                                           //device configrations
#pragma config WDT = OFF 
#pragma config LVP = OFF 
#pragma config BOREN = OFF                                             

//---------- main functions to initialize the pins                     
void Set_ADCON0 (char AN_pin);                                         //Function to set what A/D channel to be used
void Init_ADC(void);                                                   //Function to initialize the ADC registers 
unsigned int get_full_ADC(void); 
void init_UART(); 
void putch (char c);                                                   //This function is called  anytime you call the function print f
void Init_TRIS();
void Delay_One_Sec();                                                  //Function to delay 1 second

//-------------- functions to control the RGB lights
#define D1_RED PORTEbits.RE0                                           //D1 RGB LIGHT
#define D1_GREEN PORTEbits.RE1
#define D1_BLUE PORTEbits.RE2
#define D2_RED PORTCbits.RC0                                           //D2 RGB LIGHT
#define D2_GREEN PORTCbits.RC1
#define D2_BLUE PORTCbits.RC2
#define D3_GREEN PORTCbits.RC4                                         //D3 RGB LIGHT
#define D3_BLUE PORTCbits.RC5
SET_D1_WHITE();
SET_D1_CYAN();
SET_D1_BLUE();                                                         
SET_D1_YELLOW();
SET_D1_GREEN();
SET_D1_RED();
SET_D1_OFF(); 
void Display_D1(int temperature);                                      
SET_D2_WHITE();
DO_DISPLAY_D2(int temp);
SET_D3_OFF();
SET_D3_GREEN();
SET_D3_BLUE();                                                         
SET_D3_CYAN();
DO_DISPLAY_D3(float volt);

//------------------ functions to control the 7 segments display       
void DO_DISPLAY_7SEG_Upper(char digit);
void DO_DISPLAY_7SEG_Lower(char digit);


//======================================================== main project ===============================================================
void main()                                  
{ 
    Init_ADC();                                                       //initialize the ADC pins
    init_UART();
    Init_TRIS();                                                      //initialize the ports as inputs and outputs
    
    
    //----------------------- start of while loop
    while (1)
    {
        Set_ADCON0(0);                                                //set channel AN0 so we can read the temperature from the LMT84
        int num_step = get_full_ADC();                                //start the A/D conversion and save it in number step
        
        float voltage_mv = num_step * 4.0;                            //get the voltage reading
        
        float temperature_C = (1035.0 - voltage_mv) / 5.50;
        float temperature_F = 1.80 * temperature_C + 32.0;
        
        int tempF = (int) temperature_F;
        
        char L = tempF/10;
        char U = tempF%10;
        DO_DISPLAY_7SEG_Upper(U);
        DO_DISPLAY_7SEG_Lower(L);
        
        printf("Voltage = %f mV \r\n\n " ,voltage_mv  );
        printf("Temperature = %d F \r\n\n " , tempF  );
        
        Display_D1(tempF);
        DO_DISPLAY_D2(tempF);
        
        //Set_ADCON0(1);                                              //now read the light resistor value
        ADCON0 = 0x09;
        float voltage_mv1 = (float)get_full_ADC();                    //start the A/D conversion and save it in number step
        voltage_mv1 = (voltage_mv1 / 1023.0) * 5.0;                   //get the voltage reading
        printf("Light Volt = %f \r\n\n\n " , voltage_mv1  );
               
        DO_DISPLAY_D3(voltage_mv1);
       
        Delay_One_Sec() ;                                             
        
    }//-------------------- end while loop    

}
//============================================================= end ===================================================================


void Set_ADCON0 (char AN_pin)                                        //function to set what A/D channel to use 
{                                                     
    ADCON0 = AN_pin * 0x04 + 1;                                      //Equation to choose the value for ADCON0
} 
void Init_ADC(void)                          
{ 
    ADCON0=0x01;                                                     //select channel AN0, and turn on the ADC subsystem 
    ADCON1=0x1B ;                                                    //select pins AN0 through AN3 as analog signal, VDD-VSS as reference voltage      
    ADCON2=0xA9;                                                     //right justify the result. Set the bit conversion time (TAD) and // acquisition time 
}
unsigned int get_full_ADC(void) 
{ 
    int result;
    ADCON0bits.GO=1;                                                 //Start Conversion 
    while(ADCON0bits.DONE==1);                                       //wait for conversion to be completed 
    result = (ADRESH * 0x100) + ADRESL;                              //combine result of upper byte and lower byte into result   
    return result;                                                   // return the result. 
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
void Delay_One_Sec()                                                 //Function to delay 1 second
{ 
 for(int I=0; I <17000; I++); 
 }

void DO_DISPLAY_7SEG_Upper(char digit)
{
    char array[10] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10};
    PORTB = array[digit];
}
void DO_DISPLAY_7SEG_Lower(char digit)
{
    char array[10] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10};
    PORTD = array[digit];    
}





//=============================== D1 Code ============================================
SET_D1_WHITE()
{
    D1_RED = 1;
    D1_GREEN = 1;
    D1_BLUE = 1;
}
SET_D1_CYAN()
{
    D1_RED = 0;
    D1_GREEN = 1;
    D1_BLUE = 1;      
    
}
SET_D1_BLUE()
{
    D1_RED = 0;
    D1_GREEN = 0;
    D1_BLUE = 1;
}
SET_D1_YELLOW()
{
    D1_RED = 1;     
    D1_GREEN = 1;
    D1_BLUE = 0;
}
SET_D1_GREEN()
{
    D1_RED = 0;
    D1_GREEN = 1;
    D1_BLUE = 0;
    
}
SET_D1_RED()
{
    D1_RED = 1;
    D1_GREEN = 0;
    D1_BLUE = 0;
    
}
SET_D1_OFF()
{
    D1_RED = 0;
    D1_GREEN = 0;
    D1_BLUE = 0; 
}

void Display_D1(int temperature) 
{
    if(temperature < 40) 
        SET_D1_OFF();
    else if(temperature >=40 && temperature <=49) 
        SET_D1_RED();     
    else if(temperature >=50 && temperature <=59) 
        SET_D1_GREEN();    
    else if(temperature >=60 && temperature <=69) 
        SET_D1_YELLOW();    
    else if(temperature >=70 && temperature <=71) 
        SET_D1_BLUE();     
    else if(temperature >=72 && temperature <=73) 
        SET_D1_CYAN(); 
    else   
        SET_D1_WHITE();
}


//============================== D2 Code =======================================
SET_D2_WHITE()
{
    D2_RED = 1;
    D2_GREEN = 1;
    D2_BLUE = 1;
}

DO_DISPLAY_D2( int temp)
{
    if (temp > 70)
        SET_D2_WHITE();
    else
        PORTC = temp/10;  //else, set to a number 0-7 to LED 2 and display as color   
}

//================================= D3 Code =====================================
SET_D3_OFF()
{
    //D3_RED = 0;
    D3_GREEN = 0;
    D3_BLUE = 0;   
}
SET_D3_GREEN()
{
    //D3_RED = 0;
    D3_GREEN = 1;
    D3_BLUE = 0;  
}
SET_D3_BLUE()
{
    //D3_RED = 0;
    D3_GREEN = 0;
    D3_BLUE = 1;   
}
SET_D3_CYAN()
{
    //D3_RED = 0;
    D3_GREEN = 1;
    D3_BLUE = 1;   
}

DO_DISPLAY_D3(float volt)
{
    if (volt < 2.6)
        SET_D3_OFF();
    else if (volt >= 2.6 && volt < 3.0)
        SET_D3_GREEN();
    else if (volt >= 3.0 && volt < 3.3)
        SET_D3_BLUE();
    else
        SET_D3_CYAN();  
}



