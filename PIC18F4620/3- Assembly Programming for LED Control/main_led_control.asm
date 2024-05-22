
#include <P18F4620.inc>                                              ;the header file for the P18F4620 microcontroller
    
 config OSC = INTIO67                                                ;configuration settings for the microcontroller:
 config WDT = OFF 
 config LVP = OFF 
 config BOREN = OFF 

Delay1 equ 0xFF                                                      ;Constant declarations:
Delay2 equ 0XFF 
Counter_L equ 0x20                                                   
Counter_H equ 0x21 
Color_Value equ 0x28 
Loop_Count equ 0x29
 
D1_Start_Value equ 0x50                                              ;a constant equal to 0x50, This constant is used as a memory location to store the starting address of an array later in the code
D2_Start_Value equ 0x60                                              ;a constant equal to 0x60, This constant is used as a memory location to store the starting address of an array later in the code
 

 ORG 0x0000                                                          ;set the program counter to the beginning of memory 

;============================================ the start of the code ========================================
START:  
 
 MOVLW 0x0F                                                         ;sets the ADCON1 register to 0x0F to configure the analog-to-digital converter
 MOVWF ADCON1 
 
 MOVLW 0x00                                                         ;set port C and port D to be outputs
 MOVWF TRISC  
 MOVWF TRISD  
 
 MOVLW 0x00                                                         ;set the initial values for an array D1_Start_Value
 MOVWF D1_Start_Value
 MOVLW 0x01
 MOVWF D1_Start_Value + 1                                           
 MOVLW 0x02
 MOVWF D1_Start_Value + 2
 MOVLW 0x03                                                         
 MOVWF D1_Start_Value + 3
 MOVLW 0x04
 MOVWF D1_Start_Value + 4
 MOVLW 0x05                                                         
 MOVWF D1_Start_Value + 5
 MOVLW 0x06
 MOVWF D1_Start_Value + 6
 MOVLW 0x07
 MOVWF D1_Start_Value + 7                                           
 
 
 MOVLW 0x06                                                         ;set the initial values for an array D1_Start_Value
 MOVWF D2_Start_Value 
 MOVLW 0x02
 MOVWF D2_Start_Value + 1                                           
 MOVLW 0x07
 MOVWF D2_Start_Value + 2
 MOVLW 0x03
 MOVWF D2_Start_Value + 3                                           
 MOVLW 0x04
 MOVWF D2_Start_Value + 4
 MOVLW 0x05
 MOVWF D2_Start_Value + 5
 MOVLW 0x00
 MOVWF D2_Start_Value + 6                                           
 MOVLW 0x01
 MOVWF D2_Start_Value + 7
 
                                                                    
 
 
;-------------- start of while loop                                 
WHILE_LOOP: 
    
LFSR 0, D1_Start_Value                                              ;set the "local file select register 0" to the address of the D1_Start_Value array
LFSR 1, D2_Start_Value                                              ;set the "local file select register 1" to the address of the D2_Start_Value array
     
 MOVLW 0x00                                                         ;moving the value in the working register into the memory location associated with the constant Color_Value so it starts from 0.
 MOVWF Color_Value
 
 MOVLW 0x08                                                         ;moving the value in the working register (0x08) into the memory location associated with the constant Loop_Count so it starts from 8.
 MOVWF Loop_Count 
 
;------ for loop
FOR_LOOP: 
    
 MOVF INDF0 ,W                                                      ;reads a value from the array D1_Start_Value using the "local file select register 0" (INDF0) and sends it to the PORTC output
 MOVWF PORTC 
 
 MOVF INDF1 , W                                                     ;reads a value from the array D2_Start_Value using the "local file select register 1" (INDF1) and sends it to the PORTD output
 MOVWF PORTD
 
 CALL DELAY_ONE_SEC                                                 ;delay one second
 
 INCF FSR0L                                                         ;increments the "local file select register 0" to point to the next elements of the array
 INCF FSR1L                                                         ;increments the "local file select register 1" to point to the next elements of the array
 
 INCF Color_Value, F                                                ;increments the Color_Value by 1 to the next number 
 DECF Loop_Count,F                                                  ;decrement Loop_Count by 1 and checks if its zero
 
 BNZ FOR_LOOP ; 
;------ end for loop
 
 GOTO WHILE_LOOP 
;------------ end of while loop
 
 
 
 
 
 
 
 
//============= delay one sec function
 DELAY_ONE_SEC: 
    MOVLW Delay1 ; 
    MOVWF Counter_H ; 
 LOOP_OUTER: 
    NOP ; 
    MOVLW Delay2 ; 
    MOVWF Counter_L ; 
 LOOP_INNER: 
    NOP ; 
    DECF Counter_L,F ; 
    BNZ LOOP_INNER ; 
    DECF Counter_H,F ; 
    BNZ LOOP_OUTER ; 
    RETURN 
//============= end of the function
 
 
 
    
 END 
;=============================================== end ===============================================
