

#include <stdio.h>                            // all libraries needed 
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
                                             
//=========================== main project =============================================
void main()                                  
{ 
char in;                                     // declare char in  
char array[8] = {6,2,7,3,4,5,0,1};           // a specific sequence from the lab manual to be used in the for loop

 TRISC = 0x00;                               // set PortC to be output (The RGB Light in this case) 
 TRISD = 0x00;                               // set PortD to be output (The RGB Light in this case) 
 
 ADCON1 = 0x0f ;                             // sets the ADCON1 register to 0x0F to configure the analog-to-digital converter
 
 
 while (1)                                   // start endless while loop
{ 
     for (char i=0; i<8 ; i++){              // start of for loop to run through all 7 possible colors in the RGB
         in = i;
         
         in = in & 0x0F;
         PORTC = in;                         // PortC RGB is going to light up in order from 1 to 7
         PORTD = array[i];                   // PortD RGB is going to light up in the sequence declared earlier
         
         Delay_One_Sec();                    // delay 1 second every loop cycle, so we can observe the lights
         
     }
    
} 
 }
//============================ end ======================================================