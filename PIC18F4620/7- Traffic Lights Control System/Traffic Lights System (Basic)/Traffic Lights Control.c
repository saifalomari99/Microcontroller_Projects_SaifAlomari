                                                                     //the full design of a Traffic Light

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

//-------------- main functions to initialize and set and the pins
void Init_TRIS();                                                    //Function to initialize the ports to inputs and outputs
void init_UART();                                                    //Function to initialize the Universal Asynchronous Receiver/Transmitter (UART) module in a microcontroller.

//--------------- delay functions and timers:
#define SEC_LED PORTCbits.RC7                                        //Define the Seconds light (Blue Light)
void Wait_One_Second();
void Wait_Half_Second();
void Wait_N_Seconds (char seconds);

//--------------- North South Traffic Lights
#define NS_RED PORTEbits.RE0                                         //----> NS
#define NS_GREEN PORTEbits.RE1
#define NSLT_RED PORTEbits.RE2                                       //----> NS left turn
#define NSLT_GREEN PORTCbits.RC0
//--------------- East West Traffic Lights
#define EW_RED PORTCbits.RC2                                         //----> EW
#define EW_GREEN PORTCbits.RC3
#define EWLT_RED PORTCbits.RC4                                       //----> EW left turn
#define EWLT_GREEN PORTCbits.RC5

//--------------- functions to set the traffic lights colors
#define OFF 0 
#define RED 1
#define GREEN 2                                                      
#define YELLOW 3
void Set_NS(char color); 
void Set_NSLT(char color);                                           
void Set_EW(char color);
void Set_EWLT(char color);                                           

//----------------- Pedestrian Control functions
void DO_DISPLAY_7SEG_Upper(char digit);
void DO_DISPLAY_7SEG_Lower(char digit);
void PED_Control( char Direction, char Num_Sec);
void Wait_One_Second_With_Beep();
void Activate_Buzzer();
void Deactivate_Buzzer();

//---------------- Photo sensor functions
unsigned int get_full_ADC(void);                                     //Function to read the Analog value

//---------------- mode functions
#define MODE_LED PORTCbits.RC6
void Day_Mode();
void Night_Mode();

#define EWLT_SW PORTAbits.RA1                                        //left turn switches
#define NSLT_SW PORTAbits.RA2 
#define EWPED_SW PORTAbits.RA3                                       //pedestrians switches
#define NSPED_SW PORTAbits.RA4                                                 





//============================================== main project =========================================================
void main()                                                                                      
{ 
    Init_TRIS();                                                     
    ADCON0 = 0x01;                                                   //set channel AN0 so we can read the voltage from the photo sensor
    ADCON1 = 0x0E;                                                   //set all the A inputs to digital inputs (0's and 1's), but keep AN0 analog
    ADCON2=0xA9;                                                     // right justify the result. Set the bit conversion time (TAD) and // acquisition time 
     
    TRISB = 0xFF;                                                  
    TRISD = 0xFF;
    while (1) 
    {                
        int ADC_Value = get_full_ADC();                              //start the A/D conversion and save it in number step         
        float voltage = ((ADC_Value * 4.000) /1000.0) ;              //the equation to get the Voltage value in volts
        
        if (voltage < 2.5000)                                        //-----> below 2.5 volts = day mode
        {
            MODE_LED = 1;
            Day_Mode();          
        }
        else                                                         //-----> above 2.5 volts = night mode
        {
            MODE_LED = 0; 
            Night_Mode();
        }                 
    }       
   
}                                                                    
//================================================ end main ==============================================================


//--------------------------------- main functions to initialize and set and the pins
void Init_TRIS()                                                     //Function to initialize the ports to inputs and outputs
{
    TRISA = 0xFF;                                                    //make portA input
    
    TRISB = 0x00;                                                    //make PortB output
    TRISC = 0x00;                                                    //make PortC output
    TRISD = 0x00;                                                    //make PortD output
    TRISE = 0x00;                                                    //make PortE output   
}
void init_UART()                                                     //Function to initialize the Universal Asynchronous Receiver/Transmitter (UART) module in a microcontroller.
{ 
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF & 
    USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & 
    USART_BRGH_HIGH, 25); 
    OSCCON = 0x60;                                                   
} 


//------------------------------------- delay functions and timers:  
void Wait_One_Second() 
{ 
    SEC_LED = 1;                                                     //First, turn on the SEC LED 
    Wait_Half_Second();                                              //Wait for half second (or 500 msec) 
    SEC_LED = 0;                                                     //then turn off the SEC LED 
    Wait_Half_Second();                                              //Wait for half second (or 500 msec) 
} 
void Wait_Half_Second() 
{ 
    T0CON = 0x02;                                                    // Timer 0, 16-bit mode, prescaler 1:8 
    TMR0L = 0xB3;                                                    // set the lower byte of TMR 
    TMR0H = 0xB0;                                                    // set the upper byte of TMR 
     
    INTCONbits.TMR0IF = 0;                                           // clear the Timer 0 flag 
    T0CONbits.TMR0ON = 1;                                            // Turn on the Timer 0 
    while (INTCONbits.TMR0IF == 0);                                  // wait for the Timer Flag to be 1 for done 
    T0CONbits.TMR0ON = 0;                                            // turn off the Timer 0 
}
void Wait_N_Seconds (char seconds)                                   
{ 
    char I; 
    for (I = 0; I< seconds; I++) 
    { 
        Wait_One_Second();                                           
    } 
} 

//------------------------------------ functions to set the traffic lights colors
void Set_NS(char color) 
{ 
    switch (color)                                                   
    { 
        case OFF:    NS_RED =0;NS_GREEN=0;break;                     //Turns off the NS LED 
        case RED:    NS_RED =1;NS_GREEN=0;break;                     //Sets NS LED RED 
        case GREEN:  NS_RED =0;NS_GREEN=1;break;                     //sets NS LED GREEN 
        case YELLOW: NS_RED =1;NS_GREEN=1;break;                     //sets NS LED YELLOW 
    } 
}
void Set_NSLT(char color) 
{ 
    switch (color) 
    { 
        case OFF:    NSLT_RED =0;NSLT_GREEN=0;break;                 //Turns off the NS left LED 
        case RED:    NSLT_RED =1;NSLT_GREEN=0;break;                 //Sets NS left LED RED 
        case GREEN:  NSLT_RED =0;NSLT_GREEN=1;break;                 //sets NS left LED GREEN 
        case YELLOW: NSLT_RED =1;NSLT_GREEN=1;break;                 //sets NS left LED YELLOW 
    } 
}
void Set_EW(char color) 
{ 
    switch (color) 
    { 
        case OFF:    EW_RED =0;EW_GREEN=0;break;                     //Turns off the EW LED 
        case RED:    EW_RED =1;EW_GREEN=0;break;                     //Sets EW LED RED 
        case GREEN:  EW_RED =0;EW_GREEN=1;break;                     //sets EW LED GREEN 
        case YELLOW: EW_RED =1;EW_GREEN=1;break;                     //sets EW LED YELLOW 
    } 
} 
void Set_EWLT(char color) 
{ 
    switch (color) 
    { 
        case OFF:    EWLT_RED =0;EWLT_GREEN=0;break;                 //Turns off the EW left LED 
        case RED:    EWLT_RED =1;EWLT_GREEN=0;break;                 //Sets EW left LED RED 
        case GREEN:  EWLT_RED =0;EWLT_GREEN=1;break;                 //sets EW left LED GREEN 
        case YELLOW: EWLT_RED =1;EWLT_GREEN=1;break;                 //sets EW left LED YELLOW 
    } 
} 




//------------------------------------- Pedestrian Control functions 
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

void PED_Control( char Direction, char Num_Sec)
{
    TRISD = 0x00;                                                  //start values
    TRISB = 0x00;
    
    if (Direction == 0)                                            //------> North-South Direction 
    {
        TRISB = 0xFF;                                              //Turn off the lower digit       
        for (int i = Num_Sec-1  ; i > 0 ; i--)
        {
            DO_DISPLAY_7SEG_Upper(i);                              //count down on the upper 7-seg
            Wait_One_Second_With_Beep();
        }
        TRISB = 0xFF;                                              //Turn off both 7 segments
        TRISD = 0xFF;                      
    }
    else                                                           //------> East-West Direction
    {
        TRISD = 0xFF;                                              //Turn off the upper digit 
        
        for (int i = Num_Sec-1  ; i > 0 ; i--)
        {
            DO_DISPLAY_7SEG_Lower(i);                              //count down on the lower 7-seg  
            Wait_One_Second_With_Beep();
        }
        TRISB = 0xFF;                                              //Turn off both 7 segments
        TRISD = 0xFF;      
    }
    
    TRISB = 0xFF;                                                  //in the end
    TRISD = 0xFF;
    Wait_One_Second_With_Beep();

    
}
void Wait_One_Second_With_Beep() 
{  
    SEC_LED = 1;                                                   //First, turn on the SEC LED 
    Activate_Buzzer();                                             //Activate the buzzer
    Wait_Half_Second();                                            //Wait for half second (or 500 msec)   
    SEC_LED = 0;                                                   //then turn off the SEC LED 
    Deactivate_Buzzer();                                           //Deactivate the buzzer 
    Wait_Half_Second();                                            //Wait for half second (or 500 msec) 
}
void Activate_Buzzer() 
{ 
    PR2 = 0b11111001 ; 
    T2CON = 0b00000101 ; 
    CCPR2L = 0b01001010 ; 
    CCP2CON = 0b00111100 ; 
//    PORTCbits.RC1 = 1;
} 
void Deactivate_Buzzer() 
{ 
    CCP2CON = 0x0; 
    PORTCbits.RC1 = 0; 
} 



unsigned int get_full_ADC(void)                                    //Function to read the Analog value
{ 
    int result;
    ADCON0bits.GO=1;                                               //Start Conversion 
    while(ADCON0bits.DONE==1);                                     //wait for conversion to be completed 
    result = (ADRESH * 0x100) + ADRESL;                            //combine result of upper byte and lower byte into result     
    return result;                                                 //return the result. 
}




void Day_Mode()
{
    //---------------- > start with:  East - West 
    //--- step 1) 
    Set_NS(1);                                                    //red
    Set_NSLT(1);                                                  //red     
    Set_EWLT(1);                                                  //red
    Set_EW(2);                                                    //green
    
    //--- step 2) 
    if (EWPED_SW == 1)                                            //if a PED wants to cross the street
    { 
       PED_Control(1, 7);                                         
    }
    
    //---- step 3 4, 5)                                           
    Wait_N_Seconds (6);       
    Set_EW(3);                                                    //yellow
    Wait_N_Seconds (2);
    Set_EW(1);                                                    //red  
    
    
    //---------------- > second:  North - South 
    //---- step 6)
    if (NSLT_SW == 1)
    {
        //---- step 7 and 8 and 9)
        Set_NSLT(2);                                              //left turn = green
        Wait_N_Seconds (6);
        Set_NSLT(3);
        Wait_N_Seconds (2);
        Set_NSLT(1); 
    }           
    //---- step 10)
    Set_NS(2);                                                    //GREEN NORTH SOUTH     
    if (NSPED_SW == 1)
    {
        PED_Control(0, 8);     
    }   
    //---- step 11 and 12 and 13)
    Wait_N_Seconds (8);
    Set_NS(3);
    Wait_N_Seconds (2);
    Set_NS(1);                                                    //red North South
    
    
    
    //---- step 14)
    if (EWLT_SW == 1)
    {
        //---- step 15 and 16 and 17)
        Set_EWLT(2);
        Wait_N_Seconds (8);
        Set_EWLT(3);
        Wait_N_Seconds (2);
        Set_EWLT(1);     
    }          
    
}




void Night_Mode()
{
    //---- step 1 and 2)
    Set_NS(1);                                                    //red
    Set_NSLT(1);                                                  //red     
    Set_EWLT(1);                                                  //red
    Set_EW(2);                                                    //green
    Wait_N_Seconds (6);
    
    //---- step 3 and 4)
    Set_EW(3);
    Wait_N_Seconds (2);
    Set_EW(1);
    
    //---- step 5, 6, 7 ,8)
    if (NSLT_SW == 1)
    {
        Set_NSLT(2);
        Wait_N_Seconds (5);
        Set_NSLT(3);
        Wait_N_Seconds (2); 
        Set_NSLT(1);
    }
    
    //---- STEP 9, 10, 11)
    Set_NS(2);
    Wait_N_Seconds (7); 
    Set_NS(3);
    Wait_N_Seconds (2);
    Set_NS(1);
    
    //---- step 12)
    if (EWLT_SW == 1)
    {
        //---- step 15 and 16 and 17)
        Set_EWLT(2);                                             //Green
        Wait_N_Seconds (7);
        Set_EWLT(3);                                             //yellow
        Wait_N_Seconds (2);
        Set_EWLT(1);                                             //red  
    }  
    
    
}
