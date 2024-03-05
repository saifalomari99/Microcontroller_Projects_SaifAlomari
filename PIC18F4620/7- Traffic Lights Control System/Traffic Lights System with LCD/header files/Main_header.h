


//============================= Setting up all the port inputs and outputs =======================================

//#define NS_LT_SW    PORT?bits.R??       // Defines NS_LT as 
//#define NS_PED_SW   PORT?bits.R??       // Defines EW_LT as 
//#define EW_LT_SW    PORT?bits.R??       // Defines EW_PED as 

#define EWLT_SW PORTAbits.RA1                                        //left turn switches
#define NSLT_SW PORTAbits.RA2 
#define EWPED_SW PORTAbits.RA3                                       //pedestrians switches
#define NSPED_SW PORTAbits.RA4                                                 


#define NS_RED      PORTEbits.RE0       // Defines NS_RED as 
#define NS_GREEN    PORTEbits.RE1       // Defines NS_GREEN as 
#define NSLT_RED    PORTEbits.RE2       // Defines NS_LT RED as 
#define NSLT_GREEN  PORTCbits.RC0       // Defines NS_LT GREEN as 

#define EW_RED      PORTBbits.RB3       // Defines EW_RED as 
#define EW_GREEN    PORTBbits.RB4       // Defines EW_GREEN as 

#define EWLT_RED    PORTBbits.RB0       // Defines EWLT_RED as 
#define EWLT_GREEN  PORTCbits.RC2       // Defines EWLT_GREEN as 
#define MODE_LED    PORTDbits.RD0       // Defines MODE_LED as 
#define SEC_LED     PORTDbits.RD1       // Defines SEC_LED as

