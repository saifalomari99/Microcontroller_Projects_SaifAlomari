



//------------------------------------------------------------------------------        Done
//----------------------------- Main Header File -------------------------------
//------------------------------------------------------------------------------

#define _XTAL_FREQ      8000000

#define ACK             1
#define NAK             0

#define ACCESS_CFG      0xAC
#define START_CONV      0xEE
#define READ_TEMP       0xAA
#define CONT_CONV       0x02






#define FAN_EN_LED       PORTAbits.RA4            //output LED
//------------ Fan pins
#define FAN_EN           PORTAbits.RA5            //output
#define FAN_PWM          PORTCbits.RC2            //Input to the PIC18F (The values of PWM of Fan)

#define RTC_ALARM_NOT    PORTBbits.RB2            //(Input) RTC_MATCH# of the Timer


//-------------------------- Remote Keys
#define Setup_Time_Key        	12                //0x30 (1)
#define Setup_Alarm_Key       	13                //0x18 (2)
#define Setup_Fan_Temp_Key    	14                //0x7a (3)
//-----
#define Toggle_Fan_Key      	5                 //0xc2 
#define Prev            		3                 //0x22
#define Next            		4                 //0x02
#define Minus           		6                 //0xe0 
#define Plus            		7                 //0xa8
#define Ch_Plus            		2                 //0xe2
#define Ch_Minus				0                 //0xa2
