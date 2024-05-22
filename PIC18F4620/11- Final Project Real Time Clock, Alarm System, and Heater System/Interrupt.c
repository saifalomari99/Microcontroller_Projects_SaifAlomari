

//##############################################################################
//                           Interrupt Source File                            //
//##############################################################################






#include <xc.h>
#include <p18f4620.h>
#include "Interrupt.h"
                               
//------------------------- Function to initilize the interrupt pin and Timer
void Init_Interrupt()
{
    // Initialize INT0
    INTCON2bits.INTEDG0 = 0;                                      // Interrupt on falling edge
    INTCONbits.INT0IE = 1;                                        // Enable INT0 interrupt
    INTCONbits.INT0IF = 0;                                        // Clear INT0 interrupt flag
    
    // Initialize INT1
    INTCON3bits.INT1IF = 0;                                       //Clear external interrupt INT1IF
    INTCON2bits.INTEDG1 = 0;                                      //Edge programming for INT 1 falling edge H to L
    INTCON3bits.INT1IE = 1;                                       //Enable external interrupt INT1IE
    
    // Initialize Timer1
    TMR1H = 0;                                                    //Reset Timer1
    TMR1L = 0;                                                    //
    PIR1bits.TMR1IF = 0;                                          //Clear timer 1 interrupt flag
    PIE1bits.TMR1IE = 1;                                          //Enable Timer 1 interrupt
    
    // Enable interrupts
    INTCONbits.PEIE = 1;                                          //Enable Peripheral interrupt
    INTCONbits.GIE = 1;                                           //Enable global interrupts
}

//------------------------- Function to Enable the INT pins
void Enable_INT_Interrupt()
{
    //Enable INT0
    INTCONbits.INT0IE = 1;
    INTCON2bits.INTEDG0 = 0;                                      // Interrupt INT0 on falling edge
      
    //Enable INT1
    INTCON3bits.INT1IE = 1;          		                      //Enable external interrupt
    INTCON2bits.INTEDG1 = 0;        		                      //Edge programming for INT1 falling edge
}    

//========================= High Priority Function =============================
void interrupt high_priority chkisr()                             
{
    if (PIR1bits.TMR1IF == 1) TIMER1_isr();                       //
    if (INTCONbits.INT0IF == 1) INT0_isr();
    if (INTCON3bits.INT1IF == 1) INT1_isr();
}

//------------------------- Function to Triggered Timer1
void TIMER1_isr(void)
{   
    Nec_state = 0;                                                //Reset decoding process
    INTCON2bits.INTEDG1 = 0;                                      //Edge programming for INT 1 falling edge
    T1CONbits.TMR1ON = 0;                                         //Disable T1 Timer
    PIR1bits.TMR1IF = 0;                                          //Clear interrupt flag
}

//------------------------- Function to Triggered INT0
void INT0_isr()                                                   //Function to set INT0 flag
{ 
    INTCONbits.INT0IF=0;                                          //Clear the interrupt flag 
    INT0_flag = 1;                                                //set software INT0_flag  
} 




//------------------------- Function to reset the nec_state
void Reset_Nec_State()
{
    Nec_state=0;
    T1CONbits.TMR1ON = 0;                                         //TIMER1 ON
}

//------------------------- Function to Triggered INT1            
void INT1_isr(void)
{
    INTCON3bits.INT1IF = 0;                                       //Clear external interrupt INT1IF   
    //Clicked_LED = 1;       
    if (Nec_state != 0)
    {
        Time_Elapsed = (TMR1H << 8) | TMR1L;                      //Store Timer1 value in Time_Elapsed        
        TMR1H = 0;                                                //Reset Timer1
        TMR1L = 0;              
    }                                                             
    
    switch(Nec_state)                                             
    {
        case 0 ://-------------------------------------------- Case 0
        {
            TMR1H = 0;                                            //clear
            TMR1L = 0;                                            
            
            PIR1bits.TMR1IF = 0;                                  
            T1CON= 0x90;                                          //Program Timer1 mode with count = 1usec using System clock running at 8Mhz
            T1CONbits.TMR1ON = 1;                                 //Enable Timer 1
            
            bit_count = 0;                                        //Force bit count (bit_count) to 0
            Nec_code = 0;                                         //Set Nec_code = 0
            
            Nec_state = 1;                                        //Set Nec_State to state 1
            
            INTCON2bits.INTEDG1 = 1;                              //Change Edge interrupt of INT 1 to Low to High            
            return;
        }
        case 1 ://--------------------------------------------- Case 1
        {
            if((Time_Elapsed > 8500) && (Time_Elapsed < 9500))    //A leading burst of pulses that lasts up 9 msec.               
            {
                Nec_state = 2;                                                  
            }
            else Reset_Nec_State();                                     
            
            INTCON2bits.INTEDG1 = 0;                              //High to Low                                         
            return;
        }     
        case 2 ://---------------------------------------------- Case 2
        { 
            if((Time_Elapsed > 4000) && (Time_Elapsed < 5000))    //A space period that lasts 4.5 msec.                 
            {                                                                  
                Nec_state = 3;                 
            }
            else Reset_Nec_State();                                          
            
            INTCON2bits.INTEDG1 = 1;                              //Low to High                                     
            return; 
        }       
        case 3 ://----------------------------------------------- Case 3
        {
            if((Time_Elapsed > 400) && (Time_Elapsed < 700))      //A logic 0                 
            {
                Nec_state = 4;
                
            }
            else Reset_Nec_State();                                           
            
            INTCON2bits.INTEDG1 = 0;                              //High to Low                                         
            return;
        }        
        case 4 ://----------------------------------------------- Case 4
        {
            if((Time_Elapsed >= 400) && (Time_Elapsed <= 1800))   //A logic 1                
            {
                Nec_code = Nec_code << 1;                                       
                if(Time_Elapsed >= 1000)
                {
                    Nec_code+=1;                                                                            
                }
                bit_count++;                                      
                               
                if(bit_count > 31)                                //if a 32-bit data is read                                                                   
                {
                    Nec_OK = 1;                                   //flag to indicate that the whole 32-bit is reached                                              
                    INTCON3bits.INT1IE = 0; 
                    Nec_Button = Nec_code >> 8;                   //save the data input in Nec_Button variable     
                    Nec_state = 0;                                              
                }
                else
                {
                    Nec_state = 3;                                //go to state 3, and keep checking all the 32-bits (0's and 1's)
                }                                                                     
            }                                                     
            else
            {
                Reset_Nec_State();                                                                       
            }
            INTCON2bits.INTEDG1 = 1;                              //Low to High                           
            return;
        }
    }//end switch
}//end function
