



//------------------------------------------------------------------------------
//---------------------------- Utility Header File -----------------------------
//------------------------------------------------------------------------------

#define Clicked_LED PORTAbits.RA3                                 //LED to indicate that the button is pressed



char check_for_button_input();

void Do_Beep();
void Do_Beep_Good();
void Do_Beep_Bad();
void Wait_One_Sec();

void Activate_Buzzer();
void Activate_Buzzer_4KHz();
void Activate_Buzzer_2KHz();
void Activate_Buzzer_500Hz();
void Deactivate_Buzzer();

char bcd_2_dec (char);
int dec_2_bcd (char);
//void do_update_pwm(char);

void Set_RGB_D3_Color(char);
float read_volt();
unsigned int get_full_ADC();
void Init_ADC();

