
                                                                  
#define Clicked_LED PORTCbits.RC0                                 //LED to indicate that the button is pressed

unsigned char bit_count;
unsigned int Time_Elapsed;                                        //variable to store the values of the timer

unsigned long long Nec_code;
unsigned char Nec_state = 0;

extern char Nec_Button;                                           //variable to save the data coming from the remote control as an infra red light
extern short Nec_OK;                                              //flag to check if a 32-bit data is received


//============================ Interrupt Functions: ============================
void Init_INTERRUPT();
void Enable_INT_Interrupt();
void interrupt  high_priority chkisr(void) ;
void INT1_isr(void);
void TIMER1_isr(void);
void Reset_Nec_State();
