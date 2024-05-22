
#include <stdio.h>                  // all libraries needed 
#include <stdlib.h> 
#include <xc.h> 
#include <math.h> 
#include <p18f4620.h>

#pragma config OSC = INTIO67        // device configrations
#pragma config WDT = OFF 
#pragma config LVP = OFF 
#pragma config BOREN = OFF          

//=========================== main project ===============================
void main()                         
{ 
 char in;                           // declare char in
 TRISA = 0xff;                      // set PortA to be input 
 TRISB = 0x00;                      // set PortB to be output
 ADCON1 = 0x0f ;                    // sets the ADCON1 register to 0x0F to configure the analog-to-digital converter
 
 
 
 while (1)                          // start endless while loop
{ 
in = PORTA;                         // read data from PORTA and save it 
                                
 in = in & 0x0F;                    // Mask out the unused upper four bits 
                                 
 PORTB = in;                        // Output the data to PORTC 
} 
}
//============================== end =====================================