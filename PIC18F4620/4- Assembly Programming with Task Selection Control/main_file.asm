                                                              ;Part G: Combine all parts and use a control center to choose between them

#include<P18F4620.inc>                                        ;the header file for the P18F4620 microcontroller
    
config OSC = INTIO67                                          ;configuration settings for the microcontroller:  
config WDT = OFF      
config LVP = OFF      
config BOREN = OFF                                            
                                              
InA     equ     0x20                                          ;Define InA to be located at memory address 0x20
InB     equ     0x21                                          ;Define InB to be located at memory address 0x21
Result  equ     0x22                                          ;Define Result to be located at memory address 0x22    
                                                    
;============================================ the start of the code ========================================
START:                                                        
 ORG 0x0000                                                   ;set the program counter to the beginning of memory
 CLRF PORTE                                                   ;clear port E
 MOVLW 0x0F                                                   ;sets the ADCON1 register to 0x0F to configure the analog-to-digital converter
 MOVWF ADCON1 
 MOVLW 0xFF                                                   ;set port A to be input
 MOVWF TRISA 
 MOVLW 0xFF                                                   ;set port B to be input
 MOVWF TRISB 
 MOVLW 0x07                                                   ;set port D to be 0111, make sure PORT D is mixture of inputs/outputs 
 MOVWF TRISD   
 MOVLW 0x00                                                   ;set port C to be output
 MOVWF TRISC  
 MOVLW 0x00                                                   ;set port E to be output
 MOVWF TRISE  
 ;------------------------ starts main loop
 MAIN_LOOP:
    
     BTFSC PORTD, 2                                           ;check bit 2 of D. if D2 = 1, the next instruction will be executed. if D2 = 0, the next istruction will be skipped
     GOTO PORTD2EQ1                                           ;if D2 = 1
     GOTO PORTD2EQ0                                           ;if D2 = 0
     
    PORTD2EQ1: 
     GOTO TASK_BCD                                            ;if D2 = 1, always do BCD
     
    PORTD2EQ0: 
     BTFSC PORTD, 1                                           ;check bit 1 of PORT D 
     GOTO PORTD21EQ01                                         ;if D21 = 1   
     GOTO PORTD21EQ00                                         ;if D21 = 0
     
    PORTD21EQ01:
     BTFSC PORTD, 0                                           ;check bit 0 of PORT D 
     GOTO TASK_OR                                             ;if D0 = 1    011
     GOTO TASK_XOR                                            ;IF D0 = 0    010
     
    PORTD21EQ00:
     BTFSC PORTD, 0                                           ;check bit 0 of PORT D
     GOTO TASK_ADD                                            ;if D0 = 1    001
     GOTO TASK_COMP                                           ;if D0 = 0    000
    
    ;--------------- All Tasks:
    TASK_COMP: 
     BCF PORTD, 5                                             ;This is to clear the Blue LED of the RGB 
     BCF PORTD, 4                                             ;This is to clear the Green LED of the RGB 
     BCF PORTD, 3                                             ;This is to clear the LED LED of the RGB 
     CALL SUB_COMP                                            ;Call the function to do the Complement of the number
     GOTO MAIN_LOOP
    TASK_ADD: 
     BCF PORTD, 5                                             ;clear Blue
     BCF PORTD, 4                                             ;clear Green
     BSF PORTD, 3                                             ;set the red color
     CALL SUB_ADD                                             ;Call the function to do the addition of two numbers
     GOTO MAIN_LOOP 
    TASK_XOR: 
     BCF PORTD, 5                                             ;clear Blue
     BSF PORTD, 4                                             ;set the Green color
     BCF PORTD, 3                                             ;clear Red
     CALL SUB_XOR                                             ;Call the function to do the XOR of two numbers
     GOTO MAIN_LOOP 
    TASK_OR: 
     BCF PORTD, 5                                             ;clear Blue
     BSF PORTD, 4                                             ;set the green color  Green + Red = yellow
     BSF PORTD, 3                                             ;set the red color
     CALL SUB_OR                                              ;Call the function to do the OR of two numbers
     GOTO MAIN_LOOP 
    TASK_BCD: 
     BSF PORTD, 5                                             ;set the blue color
     BCF PORTD, 4                                             ;clear Green
     BCF PORTD, 3                                             ;clear Red
     CALL SUB_BCD                                             ;Call the function to do the BCD of the number
     GOTO MAIN_LOOP    
 GOTO MAIN_LOOP ; 
 ;----------------------- end main loop                       
   
 SUB_COMP:                                                    ;One's complement
     MOVF PORTA, W                                            ;move the value from Port A into the working register using MOVF                                              
     ANDLW 0x0F                                               ;perform a bitwise AND with the value 0x0F using ANDLW to isolate the least significant 4 bits                                               
     MOVWF InA                                                ;store the result in InA                                   
     COMF InA, W                                              ;Complement the value in InA and store the result in W
     ANDLW 0x0F                                               ;perform a bitwise AND with the value 0x0F using ANDLW to isolate the least significant 4 bits 
     MOVWF   Result                                           ;Move the value in W to Result                             
     MOVFF   Result, PORTC                                    ;display the results to port C
     ;code to test for Zero flag
     ANDLW 0x0F                                               ;perform a bitwise AND with the value 0x0F using ANDLW to isolate the least significant 4 bits
     BZ Function1                                             ;Check the Z flag
     BCF PORTE,0                                              ;clear bit0 of port E if the above function was not exceted
     RETURN
    
 SUB_ADD:                                                     ;Addition
     MOVF PORTA, W                                            ;move the value from Port A into the working register using MOVF                                     
     ANDLW 0x0F                                               ;perform a bitwise AND with the value 0x0F using ANDLW to isolate the least significant 4 bits                                               
     MOVWF InA                                                ;store the result in InA                      
     MOVF PORTB, W                                            ;move the value from Port B into the working register using MOVF                                           
     ANDLW 0x0F                                               ;perform a bitwise AND with the value 0x0F using ANDLW to isolate the least significant 4 bits                                                 
     MOVWF InB                                                ;store the result in InB                    
     ADDWF InA, W                                             ;ADD the value that is in W register (InB) with InA and save the results in the W register again
     MOVWF Result                                             ;Move the value in W to Result                    
     MOVFF Result, PORTC                                      ;display the results to port C 
     ;code to test for Zero flag 
     ANDLW 0x0F                                               ;perform a bitwise AND with the value 0x0F using ANDLW to isolate the least significant 4 bits
     BZ Function1                                             ;Check the Z flag
     BCF PORTE,0                                              ;clear bit0 of port E if the above function was not exceted
     RETURN
    
 SUB_XOR:                                                     ;XOR
     MOVF PORTA, W                                            ;move the value from Port A into the working register using MOVF                                     
     ANDLW 0x0F                                               ;perform a bitwise AND with the value 0x0F using ANDLW to isolate the least significant 4 bits                                               
     MOVWF InA                                                ;store the result in InA                       
     MOVF PORTB, W                                            ;move the value from Port B into the working register using MOVF                                           
     ANDLW 0x0F                                               ;perform a bitwise AND with the value 0x0F using ANDLW to isolate the least significant 4 bits                                                 
     MOVWF InB                                                ;store the result in InB                     
     XORWF InA, W                                             ;XOR the value that is in W register (InB) with InA and save the results in the W register again
     MOVWF Result                                             ;Move the value in W to Result                    
     MOVFF Result, PORTC                                      ;display the results to port C 
     ;code to test for Zero flag 
     ANDLW 0x0F                                               ;perform a bitwise AND with the value 0x0F using ANDLW to isolate the least significant 4 bits
     BZ Function1                                             ;Check the Z flag
     BCF PORTE,0                                              ;clear bit0 of port E if the above function was not exceted
     RETURN
    
 SUB_OR:                                                      ;OR
     MOVF PORTA, W                                            ;move the value from Port A into the working register using MOVF                                     
     ANDLW 0x0F                                               ;perform a bitwise AND with the value 0x0F using ANDLW to isolate the least significant 4 bits                                               
     MOVWF InA                                                ;store the result in InA                       
     MOVF PORTB, W                                            ;move the value from Port B into the working register using MOVF                                           
     ANDLW 0x0F                                               ;perform a bitwise AND with the value 0x0F using ANDLW to isolate the least significant 4 bits                                                 
     MOVWF InB                                                ;store the result in InB                     
     IORWF InA, W                                             ;OR the value that is in W register (InB) with InA and save the results in the W register again  
     MOVWF Result                                             ;Move the value in W to Result                    
     MOVFF Result, PORTC                                      ;display the results to port C 
     ;code to test for Zero flag 
     ANDLW 0x0F                                               ;perform a bitwise AND with the value 0x0F using ANDLW to isolate the least significant 4 bits
     BZ Function1                                             ;Check the Z flag
     BCF PORTE,0                                              ;clear bit0 of port E if the above function was not exceted
     RETURN
    
    
 SUB_BCD:                                                     ;BCD
     MOVF PORTA, W                                            ;move the value from Port A into the working register using MOVF                                     
     ANDLW 0x0F                                               ;perform a bitwise AND with the value 0x0F using ANDLW to isolate the least significant 4 bits                                               
     MOVWF InA                                                ;store the result in InA                       
     MOVLW 0x09                                               ;Move 9 (1001) to the W register
     CPFSGT InA                                               ;Compare InA with W, skip the next instructor if InA>W. If InA<W, just go the next instruction and display the number            
     GOTO Normal_function                                     ;if InA less than 1001
     GOTO Addtion_function                                    ;if InA bigger than 1001, add 6
 
 
 
  Normal_function:
    MOVFF InA, PORTC                                          ;display the InA to port C
    MOVF PORTC, W                                             ;saves the value to register W to check for the zero in the next instructions
    ;---------- code to test for Zero flag
    ANDLW 0x0F                                                ;perform a bitwise AND with the value 0x0F using ANDLW to isolate the least significant 4 bits
    BZ Function1                                              ;Check the Z flag
    BCF PORTE,0                                               ;clear bit0 of port E
    GOTO MAIN_LOOP 
 Addtion_function: 
    MOVF InA, W                                               ;Move the value of InA to W register
    ADDLW 0x06                                                ;ADD 0110 (6) to the W
    MOVWF InA                                                 ;Move the value of W register to InA
    MOVFF InA, PORTC                                          ;display InA with Port C
 Function1:
    BSF PORTE, 0                                              ;set bit0 of Port E to 1
    
 END 
;====================================================== end ===========================================================
