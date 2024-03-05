
                        ////////////////////////////////////////////////////////////////////////////////
                        //                                                                            //
                        //                Traffic Lights Control System with LCD                      //
                        //                Spring 2023                                                 //
                        //                Computer Engineer: SAIF ALOMARI                             //                                                                                                                                                           
                        //                                                                            //
                        ////////////////////////////////////////////////////////////////////////////////



#include <stdio.h>                                                     //all libraries needed
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <string.h>

#include "ST7735_TFT.h"                                                //Connect the other project files to this main file
#include "main.h"

#pragma config OSC = INTIO67                                           //device configrations
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF


//-------------- main functions to initialize and set and the pins     
unsigned int get_full_ADC(void);                                       //Function to read the Analog value
void Init_ADC(void);                                                   //Function to initialize the ADC registers
void Init_TRIS(void);                                                  //Function to initialize the ports to inputs and outputs
void init_UART(void);                                                  //Function to initialize the UART 
void putch (char);                                                     //Function to help with reading the strings on TeraTerm
void Set_ADCON0(char ch);                                              //Function to set the ADCON0 Register

//------------- Functions to set the colors of the traffic lights      
void Set_EW(char color);
void Set_EW_LT(char color);
void Set_NS(char color);                                               
void Set_NS_LT(char color);
                                                                       
//------------ Wait functions
void Wait_One_Second();
void Wait_Half_Second();                                               
void Wait_N_Seconds (char);
void Wait_One_Second_With_Beep();

//------------ pedestrians control function                            
void PED_Control( char Direction, char Num_Sec);

//------------ Day and Night mode cycles                               
void Day_Mode();
void Night_Mode();

//------------ LCD Functions
void update_LCD_color(char , char );                                   //Function to update and fill the circles with colors
void update_LCD_PED_Count(char direction, char count);                 //Function to update the pedestrians counter
void Initialize_Screen();                                              //Function to initialize the LCD with the right text and drawings
void update_LCD_misc();                                                //Function to update the switches LCD values
void update_LCD_count(char, char);                                     //Function to update the lights counter
void Get_Light_Readout();


#define _XTAL_FREQ  8000000                                            // Set operation for 8 Mhz
#define TMR_CLOCK   _XTAL_FREQ/4                                       // Timer Clock 2 Mhz
//colors
#define OFF             0                                              // Defines OFF as decimal value 0
#define RED             1                                              // Defines RED as decimal value 1
#define GREEN           2                                              // Defines GREEN as decimal value 2
#define YELLOW          3                                              // Defines YELLOW as decimal value 3

#define NS              0                                              // Number definition of North/South
#define NSLT            1                                              // Number definition of North/South Left Turn
#define EW              2                                              // Number definition of East/West
#define EWLT            3                                              // Number definition of East/West Left Turn
//--------------- LCD Sizes and inputs
#define Circle_Size     7                                              // Size of Circle for Light
#define Circle_Offset   15                                             // Location of Circle
#define TS_1            1                                              // Size of Normal Text
#define TS_2            2                                              // Size of PED Text
#define Count_Offset    10                                             // Location of Count

#define XTXT            30                                             // X location of Title Text 
#define XRED            40                                             // X location of Red Circle
#define XYEL            60                                             // X location of Yellow Circle
#define XGRN            80                                             // X location of Green Circle
#define XCNT            100                                            // X location of Sec Count

#define Color_Off       0                                              // Number definition of Off Color
#define Color_Red       1                                              // Number definition of Red Color
#define Color_Green     2                                              // Number definition of Green Color
#define Color_Yellow    3                                              // Number definition of Yellow Color

#define NS_Txt_Y        20                                             //North-South theme color
#define NS_Cir_Y        NS_Txt_Y + Circle_Offset
#define NS_Count_Y      NS_Txt_Y + Count_Offset                        
#define NS_Color        ST7735_BLUE 

#define NSLT_Txt_Y      50                                             //North-South left turn theme color
#define NSLT_Cir_Y      NSLT_Txt_Y + Circle_Offset                     
#define NSLT_Count_Y    NSLT_Txt_Y + Count_Offset
#define NSLT_Color      ST7735_MAGENTA

#define EW_Txt_Y        80                                             //East-West theme color
#define EW_Cir_Y        EW_Txt_Y + Circle_Offset                       
#define EW_Count_Y      EW_Txt_Y + Count_Offset
#define EW_Color        ST7735_CYAN

#define EWLT_Txt_Y      110                                            //East-West left turn theme color
#define EWLT_Cir_Y      EWLT_Txt_Y + Circle_Offset                     
#define EWLT_Count_Y    EWLT_Txt_Y + Count_Offset
#define EWLT_Color      ST7735_WHITE

#define PED_NS_Count_Y  30                                             
#define PED_EW_Count_Y  90
#define PED_Count_X     2
#define Switch_Txt_Y    140                                            

#define PED_Count_NS    8
#define PED_Count_EW    9                                              


char buffer[31];                                                       // general buffer for display purpose
char *nbr;                                                             // general pointer used for buffer
char *txt;

char NS_Count[]     = "00";                                            // text storage for NS Count
char NSLT_Count[]   = "00";                                            // text storage for NS Left Turn Count
char EW_Count[]     = "00";                                            // text storage for EW Count
char EWLT_Count[]   = "00";                                            // text storage for EW Left Turn Count

char PED_NS_Count[] = "00";                                            // text storage for NS Pedestrian Count
char PED_EW_Count[] = "00";                                            // text storage for EW Pedestrian Count

char SW_NSPED_Txt[] = "0";                                             // text storage for NS Pedestrian Switch
char SW_NSLT_Txt[]  = "0";                                             // text storage for NS Left Turn Switch
char SW_EWPED_Txt[] = "0";                                             // text storage for EW Pedestrian Switch
char SW_EWLT_Txt[]  = "0";                                             // text storage for EW Left Turn Switch
char SW_MODE_Txt[]  = "D";                                             // text storage for Mode Light Sensor
	
char Act_Mode_Txt[]  = "D";                                            // text storage for Actual Mode
char FlashingS_Txt[] = "0";                                            // text storage for Flashing Status
char FlashingR_Txt[] = "0";                                            // text storage for Flashing Request

char dir;
char Count;                                                            // RAM variable for Second Count
char PED_Count;                                                        // RAM variable for Second Pedestrian Count

char SW_NSPED;                                                         // RAM variable for NS Pedestrian Switch
char SW_NSLT;                                                          // RAM variable for NS Left Turn Switch
char SW_EWPED;                                                         // RAM variable for EW Pedestrian Switch
char SW_EWLT;                                                          // RAM variable for EW Left Turn Switch
char SW_MODE;                                                          // RAM variable for Mode Light Sensor
int MODE;
char direction;
float volt;
int nStep;


//============================================ main ===============================================================
void main(void)
{
    Init_ADC();
    init_UART();
    Init_TRIS();  
    OSCCON = 0x70;                                                    // set the system clock to be 1MHz 1/4 of the 4MHz
    RBPU = 0;
    
    Initialize_Screen();                                              // Initialize the TFT screen
  
    ADCON0 = 0x01;         
    Get_Light_Readout();
    
    
    
    while (1) 
    {                
        int ADC_Value = get_full_ADC();                              //start the A/D conversion and save it in number step         
        float voltage = ((ADC_Value * 4.000) /1000.0) ;              //the equation to get the Voltage value in volts
        printf ("Day Mode \r\n");
        
        
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
    }                                                                //
 
}
//========================================== end main ===================================================================


void Init_ADC()                                                      //Function to initialize the ADC registers
{
    ADCON0 = 0x01;
    ADCON1= 0x0E; 
    ADCON2= 0xA9;
}
void Init_TRIS()
{
    TRISA = 0x1F;                                                    //inputs RA0/AN0-RA4 inputs,RA5 output  
    TRISB = 0x00;                                                    //outputs
    TRISC = 0x00;                                                    //set PORTC as outputs
    TRISD = 0x00;                                                    //set PORTD as outputs
    TRISE = 0x00;                                                    //set PORTE as outputs
}
unsigned int get_full_ADC()
{
unsigned int result;
   ADCON0bits.GO=1;                                                  //Start Conversion
   while(ADCON0bits.DONE==1);                                        //wait for conversion to be completed
   result = (ADRESH * 0x100) + ADRESL;                               //combine result of upper byte and
                                                                     //lower byte into result
   return result;                                                    //return the result.
}

void init_UART()
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &
    USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &
    USART_BRGH_HIGH, 25);
    OSCCON = 0x60;
}
void putch (char c)
{
    while (!TRMT);
    TXREG = c;
}
void Set_ADCON0 (char ch)
{
    ADCON0 = ch * 4 + 1;
}




//================================ Set the Traffic Lights ======================
void Set_NS(char color)
{
    direction = NS;
    update_LCD_color(direction, color);
    
    switch (color)
    {
        case OFF: NS_RED =0;NS_GREEN=0;break;                        // Turns off the NS LED
        case RED: NS_RED =1;NS_GREEN=0;break;                        // Sets NS LED RED
        case GREEN: NS_RED =0;NS_GREEN=1;break;                      // sets NS LED GREEN
        case YELLOW: NS_RED =1;NS_GREEN=1;break;                     // sets NS LED YELLOW
    }
}
void Set_NS_LT(char color)                                           
{
    direction = NSLT;
    update_LCD_color(direction, color);
    switch (color)
    {
        case OFF: NSLT_RED =0;NSLT_GREEN=0;break;                    // Turns off NS_LT LED
        case RED: NSLT_RED =1;NSLT_GREEN=0;break;                    // Sets NS_LT LED RED
        case GREEN: NSLT_RED =0;NSLT_GREEN=1;break;                  // Sets NS_LT LED GREEN
        case YELLOW: NSLT_RED =1;NSLT_GREEN=1;break;                 // Sets NS_LT LED YELLOW
    }
}
void Set_EW(char color)
{
    direction = EW;
    update_LCD_color(direction, color);
    
    switch (color)
    {
        case OFF: EW_RED =0;EW_GREEN=0;break;                        // Turns off EW LED
        case RED: EW_RED =1;EW_GREEN=0;break;                        // Sets EW LED RED
        case GREEN: EW_RED =0;EW_GREEN=1;break;                      // Sets EW LED GREEN
        case YELLOW: EW_RED =1;EW_GREEN=1;break;                     // Sets EW LED YELLOW
    }
}

void Set_EW_LT(char color)                                           
{
    direction = EWLT;
    update_LCD_color(direction, color);
    
    switch (color)
    {
        case OFF: EWLT_RED =0;EWLT_GREEN=0;break;                    // Turns off EW_LT LED
        case RED: EWLT_RED =1;EWLT_GREEN=0;break;                    // Sets EW_LT LED RED
        case GREEN: EWLT_RED =0;EWLT_GREEN=1;break;                  // Sets EW_LT LED GREEN
        case YELLOW: EWLT_RED =1;EWLT_GREEN=1;break;                 // Sets EW_LT LED YELLOW
    }
}


//================================ Buzzer ======================================
void Activate_Buzzer()                                               //Function to activate the Buzzer
{
    PR2 = 0b11111001 ;
    T2CON = 0b00000101 ;                                             
    CCPR2L = 0b01001010 ;                                            
    CCP2CON = 0b00111100 ;
    //PORTCbits.RC1 = 1;
}
void Deactivate_Buzzer()                                             //Function to deactivate the buzzer
{
    CCP2CON = 0x0;
	PORTCbits.RC1 = 0;
}

//============================= Pedestrian Function ============================
void PED_Control( char direction, char Num_Sec)                      
{ 
    for(char Sec = Num_Sec-1;Sec>0; Sec--)
    {
        update_LCD_PED_Count(direction, Sec);                        //start the pedestrian counter
        Wait_One_Second_With_Beep();                                 //hold the number on 7-Segment for 1 second
    }
    
    update_LCD_PED_Count(direction, 0);         
    Wait_One_Second_With_Beep();                                     //leaves the 7-Segment off for 1 second
}




//============================= Day Mode =======================================
void Day_Mode()
{
    printf ("Day Mode1 \r\n");
    MODE = 1;                                                        //turns on the MODE_LED
    //MODE_LED = 1;
    Act_Mode_Txt[0] = 'D';
    
    //---------------- > start with:  East - West                    
    //--- step 1) 
    Set_NS(1);                                                       //red
    Set_NS_LT(1);                                                    //red     
    Set_EW_LT(1);                                                    //red
    Set_EW(2);                                                       //green  East-West
    //--- step 2) 
    if (EWPED_SW == 1)                                               //if a PED wants to cross the street
    { 
       PED_Control(EW, 7);                                         
    }    
    //---- step 3 4, 5)                                           
    Wait_N_Seconds (6);       
    Set_EW(3);                                                       //yellow
    Wait_N_Seconds (2);
    Set_EW(1);                                                       //red East-West is done     
    
    //---------------- > second:  North - South                      
    //---- step 6)
    if (NSLT_SW == 1)                                                //check if someone in on the left turn of North-South
    {
        //---- step 7 and 8 and 9)
        Set_NS_LT(2);                                                //left turn = green
        Wait_N_Seconds (6);
        Set_NS_LT(3);
        Wait_N_Seconds (2);                                          
        Set_NS_LT(1); 
    }           
    //---- step 10)
    Set_NS(2);                                                       //GREEN NORTH SOUTH     
    if (NSPED_SW == 1)
    {
        PED_Control(0, 8);                                           
    }   
    //---- step 11 and 12 and 13)
    Wait_N_Seconds (8);                                              
    Set_NS(3);
    Wait_N_Seconds (2);
    Set_NS(1);                                                       //red North South
   
  
    //---- step 14)
    if (EWLT_SW == 1)
    {
        //---- step 15 and 16 and 17)                                
        Set_EW_LT(2);                                                //Green
        Wait_N_Seconds (8);
        Set_EW_LT(3);                                                //Yellow
        Wait_N_Seconds (2);
        Set_EW_LT(1);                                                //Red
    }          
}

//=================================== Night Mode ===============================
void Night_Mode()
{ 
    printf ("Night Mode \r\n");                                  
    MODE = 0;                                                        //Mode inductator                     
    //MODE_LED = 0;   
    Act_Mode_Txt[0] = 'N';                                           //Show the N on the LCD
    
    //---- step 1 and 2 and step 3 and 4)
    Set_NS(1);                                                       //red
    Set_NS_LT(1);                                                    //red     
    Set_EW_LT(1);                                                    //red
    Set_EW(2);                                                       //green East-West
    Wait_N_Seconds (6);
    Set_EW(3);                                                       //Yellow East-West
    Wait_N_Seconds (2);
    Set_EW(1);                                                       //Red East-West END
    
    //---- step 5, 6, 7 ,8)
    if (NSLT_SW == 1)                                                //if there is a car on the left turn of North-South
    {
        Set_NS_LT(2);
        Wait_N_Seconds (5);
        Set_NS_LT(3);
        Wait_N_Seconds (2); 
        Set_NS_LT(1);
    }
    
    //---- STEP 9, 10, 11)
    Set_NS(2);                                                       //Green North-South
    Wait_N_Seconds (7); 
    Set_NS(3);                                                       //Yellow North-South
    Wait_N_Seconds (2);
    Set_NS(1);                                                       //Red North-South
    
    //---- step 12)
    if (EWLT_SW == 1)                                                //if there is a car on the left turn of East-West
    {
        //---- step 15 and 16 and 17)
        Set_EW_LT(2);                                                //Green
        Wait_N_Seconds (7);
        Set_EW_LT(3);                                                //yellow
        Wait_N_Seconds (2);
        Set_EW_LT(1);                                                //red  
    }   
}

//============================= Wait Functions =================================
void Wait_One_Second()				                                 //creates one second delay and blinking asterisk
{
    SEC_LED = 1;
    strcpy(txt,"*");                                                 //A star to indicates the seconds passing
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);             //Location of the text
    Wait_Half_Second();                                              //Wait for half second (or 500 msec)

    SEC_LED = 0;
    strcpy(txt," ");
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);             //Location of the text
    Wait_Half_Second();                                              //Wait for half second (or 500 msec)
    update_LCD_misc();
}

void Wait_One_Second_With_Beep()                                     //creates one second delay as well as sound buzzer
{
    SEC_LED = 1;
    strcpy(txt,"*");                                                 //A star to indicates the seconds passing
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);             //Location of the text
    Activate_Buzzer();
    Wait_Half_Second();                                              //Wait for half second (or 500 msec)

    SEC_LED = 0;
    strcpy(txt," ");
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);             //Location of the text
    Deactivate_Buzzer();
    Wait_Half_Second();                                              //Wait for half second (or 500 msec)
    update_LCD_misc();
}
    
void Wait_Half_Second()                                              
{
    T0CON = 0x03;                                                    //Timer 0, 16-bit mode, prescaler 1:16
    TMR0L = 0xDB;                                                    //set the lower byte of TMR
    TMR0H = 0x0B;                                                    //set the upper byte of TMR
    INTCONbits.TMR0IF = 0;                                           //clear the Timer 0 flag
    T0CONbits.TMR0ON = 1;                                            //Turn on the Timer 0
    while (INTCONbits.TMR0IF == 0);                                  //wait for the Timer Flag to be 1 for done
    T0CONbits.TMR0ON = 0;                                            //turn off the Timer 0
}

void Wait_N_Seconds (char seconds)                                   //Wait N seconds function
{
    char I;
    for (I = seconds; I> 0; I--)
    {
        update_LCD_count(direction, I);                              //LCD counters for the lights
        Wait_One_Second();                                           //calls Wait_One_Second for x number of times
      
    }
    update_LCD_count(direction, 0);                                  //set it to 0
}


//================================ LCD Functions ===============================

void Initialize_Screen()                                             //Function to initialize the LCD with the locations and the drawings
{
  LCD_Reset();
  TFT_GreenTab_Initialize();
  fillScreen(ST7735_BLACK);
  
  /* TOP HEADER FIELD */
  txt = buffer;
  strcpy(txt, "ECE3301L Spring 23-S1");                              //Header Text
  drawtext(2, 2, txt, ST7735_WHITE, ST7735_BLACK, TS_1);             //the location of the text
  
  /* MODE FIELD */
  strcpy(txt, "Mode:");                                              //Mode Text
  drawtext(2, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);            //location of the text
  drawtext(35,10, Act_Mode_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);

  /* EMERGENCY REQUEST FIELD */
  strcpy(txt, "FR:");
  drawtext(50, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(70, 10, FlashingR_Txt, ST7735_WHITE, ST7735_BLACK, TS_1); 
  
  /* EMERGENCY STATUS FIELD */
  strcpy(txt, "FS:");
  drawtext(80, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(100, 10, FlashingS_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  
  /* SECOND UPDATE FIELD */
  strcpy(txt, "*");                                                  //Seconds indicator light
  drawtext(120, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);          //location of the text
      
  /* NORTH/SOUTH UPDATE FIELD */
  strcpy(txt, "NORTH/SOUTH");                                        //North-South Light
  drawtext  (XTXT, NS_Txt_Y  , txt, NS_Color, ST7735_BLACK, TS_1);   //location of the text
  drawRect  (XTXT, NS_Cir_Y-8, 60, 18, NS_Color);                    //Outer rectangle
  drawCircle(XRED, NS_Cir_Y  , Circle_Size, ST7735_RED);             //red circle
  drawCircle(XYEL, NS_Cir_Y  , Circle_Size, ST7735_YELLOW);          //yellow circle
  fillCircle(XGRN, NS_Cir_Y  , Circle_Size, ST7735_GREEN);           //green circle
  drawtext  (XCNT, NS_Count_Y, NS_Count, NS_Color, ST7735_BLACK, TS_2);//The counter of the North-South Light
    
  /* NORTH/SOUTH LEFT TURN UPDATE FIELD */
  strcpy(txt, "N/S LT");                                             //North-South left turn
  drawtext  (XTXT, NSLT_Txt_Y, txt, NSLT_Color, ST7735_BLACK, TS_1);
  drawRect  (XTXT, NSLT_Cir_Y-8, 60, 18, NSLT_Color);
  fillCircle(XRED, NSLT_Cir_Y, Circle_Size, ST7735_RED);
  drawCircle(XYEL, NSLT_Cir_Y, Circle_Size, ST7735_YELLOW);
  drawCircle(XGRN, NSLT_Cir_Y, Circle_Size, ST7735_GREEN);  
  drawtext  (XCNT, NSLT_Count_Y, NSLT_Count, NSLT_Color, ST7735_BLACK, TS_2);
  
  /* EAST/WEST UPDATE FIELD */
  strcpy(txt, "EAST/WEST");                                         //East-West Light
  drawtext  (XTXT, EW_Txt_Y, txt, EW_Color, ST7735_BLACK, TS_1);
  drawRect  (XTXT, EW_Cir_Y-8, 60, 18, EW_Color);
  fillCircle(XRED, EW_Cir_Y, Circle_Size, ST7735_RED);
  drawCircle(XYEL, EW_Cir_Y, Circle_Size, ST7735_YELLOW);
  drawCircle(XGRN, EW_Cir_Y, Circle_Size, ST7735_GREEN);  
  drawtext  (XCNT, EW_Count_Y, EW_Count, EW_Color, ST7735_BLACK, TS_2);

  /* EAST/WEST LEFT TURN UPDATE FIELD */
  strcpy(txt, "E/W LT");                                            //East-West left turn
  drawtext  (XTXT, EWLT_Txt_Y, txt, EWLT_Color, ST7735_BLACK, TS_1);
  drawRect  (XTXT, EWLT_Cir_Y-8, 60, 18, EWLT_Color);  
  fillCircle(XRED, EWLT_Cir_Y, Circle_Size, ST7735_RED);
  drawCircle(XYEL, EWLT_Cir_Y, Circle_Size, ST7735_YELLOW);
  drawCircle(XGRN, EWLT_Cir_Y, Circle_Size, ST7735_GREEN);   
  drawtext  (XCNT, EWLT_Count_Y, EWLT_Count, EWLT_Color, ST7735_BLACK, TS_2);

  /* NORTH/SOUTH PEDESTRIAM UPDATE FIELD */
  strcpy(txt, "PNS");  
  drawtext(3, NS_Txt_Y, txt, NS_Color, ST7735_BLACK, TS_1);
  drawtext(2, PED_NS_Count_Y, PED_NS_Count, NS_Color, ST7735_BLACK, TS_2);
  
  /* EAST/WEST PEDESTRIAM UPDATE FIELD */  
  drawtext(2, PED_EW_Count_Y, PED_EW_Count, EW_Color, ST7735_BLACK, TS_2);
  strcpy(txt, "PEW");  
  drawtext(3, EW_Txt_Y, txt, EW_Color, ST7735_BLACK, TS_1);
  
  
  
  /* MISCELLANEOUS UPDATE FIELD */  
  strcpy(txt, "NSP NSLT EWP EWLT MR");                               //Switches Sensors
  drawtext(1,  Switch_Txt_Y, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(6,  Switch_Txt_Y+9, SW_NSPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(32, Switch_Txt_Y+9, SW_NSLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(58, Switch_Txt_Y+9, SW_EWPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(87, Switch_Txt_Y+9, SW_EWLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);  
  drawtext(112,Switch_Txt_Y+9, SW_MODE_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
}

void update_LCD_color(char direction, char color)                    //Update the colors and filling the circles
{
    char Circle_Y;
    Circle_Y = NS_Cir_Y + direction * 30;    
    
    if (color == Color_Off)                                          //if Color off make all circles black but leave outline
    {
            fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);
            fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);
            fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK); 
            drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
            drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
            drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);                       
    }    
    
    if (color == Color_Red)                                          //if the color is red only fill the red circle with red
    {
            fillCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);
            fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);
            fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK); 
            drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
            drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
            drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);  
    }
          
    if (color == Color_Yellow)                                       //if the color is yellow only fill the yellow circle with yellow
    {
            fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);
            fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
            fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK); 
            drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
            drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
            drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);                
    }
          
    if (color == Color_Green)                                        //if the color is green only fill the green circle with green
    {
            fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);
            fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);
            fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN); 
            drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
            drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
            drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);                      
    }
         
}

void update_LCD_count(char direction, char count)
{
   switch (direction)                                                //update traffic light no ped time
   {
      case NS:       
        NS_Count[0] = count/10  + '0';
        NS_Count[1] = count%10  + '0';
        drawtext(XCNT, NS_Count_Y, NS_Count, NS_Color, ST7735_BLACK, TS_2); 
        break;
      
      case NSLT:      
        NSLT_Count[0] = count/10  + '0';
        NSLT_Count[1] = count%10  + '0';
        drawtext(XCNT, NSLT_Count_Y, NSLT_Count, NSLT_Color, ST7735_BLACK, TS_2);          
        break;
      
      case EW:        
        EW_Count[0] = count/10  + '0';
        EW_Count[1] = count%10  + '0';
        drawtext(XCNT, EW_Count_Y, EW_Count, EW_Color, ST7735_BLACK, TS_2);                
        break;
            
      case EWLT:       
        EWLT_Count[0] = count/10  + '0';
        EWLT_Count[1] = count%10  + '0';
        drawtext(XCNT, EWLT_Count_Y, EWLT_Count, EWLT_Color, ST7735_BLACK, TS_2);        
        break;
    }  
}

void update_LCD_PED_Count(char direction, char count)
{

   switch (direction)
   {
      case NS:       
        PED_NS_Count[0] = count/10  + '0';                                                   //PED count upper digit
        PED_NS_Count[1] = count%10  + '0';                                                   //PED Lower
        drawtext(PED_Count_X, PED_NS_Count_Y, PED_NS_Count, NS_Color, ST7735_BLACK, TS_2);
                                                                                             //Put counter digit on screen
        break;
        
      case EW:       
        PED_EW_Count[0] = count/10  + '0';                                                   //PED count upper digit
        PED_EW_Count[1] = count%10  + '0';                                                   //PED Lower
        drawtext(PED_Count_X, PED_EW_Count_Y, PED_EW_Count, EW_Color, ST7735_BLACK, TS_2);        
        break;
   }  
}
void Get_Light_Readout()
{
    nStep = get_full_ADC();                                                                  //calculates the # of steps for analog conversion
    volt = nStep * 5 /1024.0;                                                                //gets the voltage in Volts, using 5V as reference s instead of 4, also divide by 1024 
}

void update_LCD_misc()                                                                       //Function to update the switches LCD values
{
    Get_Light_Readout();
    SW_MODE = volt < 3.5 ? 1:0;                                                              //Mode = 1, Day_mode, Mode = 0 Night_mode  
    SW_NSPED = NSPED_SW;
    SW_NSLT = NSLT_SW;
    SW_EWPED = EWPED_SW;
    SW_EWLT = EWLT_SW;
      
    if (SW_NSPED == 0) SW_NSPED_Txt[0] = '0'; else SW_NSPED_Txt[0] = '1';                    //Set Text at bottom of screen to switch states
    if (SW_NSLT == 0) SW_NSLT_Txt[0] = '0'; else SW_NSLT_Txt[0] = '1';
    if (SW_EWPED == 0) SW_EWPED_Txt[0] = '0'; else SW_EWPED_Txt[0] = '1';
    if (SW_EWLT == 0) SW_EWLT_Txt[0] = '0'; else SW_EWLT_Txt[0] = '1';
    if (SW_MODE == 0) SW_MODE_Txt[0] = 'N'; else SW_MODE_Txt[0] = 'D';    
   
    drawtext(35,10, Act_Mode_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);    
    drawtext(6,  Switch_Txt_Y+9, SW_NSPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);            //Show switch and sensor states at bottom of the screen
    drawtext(32,  Switch_Txt_Y+9, SW_NSLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
    drawtext(58,  Switch_Txt_Y+9, SW_EWPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
    drawtext(87,  Switch_Txt_Y+9, SW_EWLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
    drawtext(112,  Switch_Txt_Y+9, SW_MODE_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);            
}


