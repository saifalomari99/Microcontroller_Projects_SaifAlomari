
#include <stdio.h>                           // all libraries needed 
#include <stdlib.h>                          
#include <xc.h> 
#include <math.h>                            
#include <p18f4620.h>

#pragma config OSC = INTIO67                 // device configrations
#pragma config WDT = OFF 
#pragma config LVP = OFF 
#pragma config BOREN = OFF                   


void Delay_One_Sec()                         // Function to delay 1 second
{ 
 for(int I=0; I <17000; I++); 
 }                                           

//=========================== main project ===============================
void main()                                  
{ 
char in;                                     // declare char in  
 TRISA = 0xff;                               // set PortA to be input 
 TRISC = 0x00;                               // set PortC to be output (The RGB Light in this case) 
 ADCON1 = 0x0f ;                             // sets the ADCON1 register to 0x0F to configure the analog-to-digital converter
 
 while (1)                                   // start endless while loop
{ 
     for (char i=0; i<8 ; i++){              // start of for loop to run through all 7 possible colors in the RGB
         
         in = i;                             // in equals to i in this part, so a new color lights up every time       
         in = in & 0x0F;      
         PORTC = in;
         
         Delay_One_Sec();                    // delay 1 second every loop cycle, so we can observe the lights
         
     }
    
} 
}
//============================ end ========================================