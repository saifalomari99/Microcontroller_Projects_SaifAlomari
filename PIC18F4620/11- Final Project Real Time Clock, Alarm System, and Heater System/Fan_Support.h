

//------------------------------------------------------------------------------
//---------------------------- Fan Support Header File -------------------------
//------------------------------------------------------------------------------


#define FAN_EN			PORTAbits.RA5    //output
#define FAN_EN_LED		PORTAbits.RA4    //FAN LED
#define FAN_PWM			PORTCbits.RC2    //


int get_RPM();

void Toggle_Fan();
void Turn_Off_Fan();
void Turn_On_Fan();
void Increase_Speed();
void Decrease_Speed();

void do_update_pwm(char);

void Set_DC_RGB(int);                       //set D1
void Set_RPM_RGB(int);                      //set D2




