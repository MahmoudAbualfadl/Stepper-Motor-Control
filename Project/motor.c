/*
 * motor.c
 *
 *  Created on: May 15, 2024
 *      Author: Mahmoud
 */
#include <avr/io.h>

#include <avr/io.h>
#include <avr/interrupt.h>
typedef unsigned char         uint8;

#define FULL_WAVE 4
#define HALF_WAVE_STEPS 8



uint8 motor_steps_half[HALF_WAVE_STEPS] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08, 0x09};
//0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08, 0x09
//8,12,4,6,2,3,1,9
uint8 steps_2=0;
uint8 motor_steps[FULL_WAVE] = {12,6,3,9};
volatile uint8_t stepIndex = 0;;

volatile uint8 overflowCount = 0;
volatile uint8_t direction = 0;
volatile uint8_t steppingMode = 0;


ISR(TIMER0_OVF_vect) {
    overflowCount++;
    if (overflowCount >= 3) { // Approx. 50 ms (16.384 ms * 3 = 49.152 ms)
        overflowCount = 0;

        if (steppingMode == 0) { // Full-step mode
            LCD_clearScreen();
            LCD_displayStringRowColumn(0,0,"Full-step mode");
            PORTC = motor_steps[stepIndex];
            if (direction == 0) {
            	 LCD_clearScreen();
                 LCD_displayStringRowColumn(0,0,"clockwise");

                stepIndex = (stepIndex + 1) % 4;
            } else {
                stepIndex = (stepIndex == 0) ? 3 : stepIndex - 1;
                LCD_clearScreen();
                LCD_displayStringRowColumn(0,0,"clockwise");
            }
        } else { // Half-step mode
        	LCD_clearScreen();
        	LCD_displayStringRowColumn(0,0,"Half-step mode");
            PORTC = motor_steps_half[stepIndex];
            if (direction == 0) {
                stepIndex = (stepIndex + 1) % 8;
           	 LCD_clearScreen();
              LCD_displayStringRowColumn(0,0,"counter-clockwise");
            } else {
                stepIndex = (stepIndex == 0) ? 7 : stepIndex - 1;
                LCD_clearScreen();
                LCD_displayStringRowColumn(0,0,"clockwise");
            }
        }
    }
}

ISR(INT0_vect) {
    direction = !direction; // Toggle direction
}
ISR(INT1_vect) {
    steppingMode = !steppingMode; // Toggle stepping mode
}


void INT0n1_Init(void)
{
	  GICR |= (1 << INT0) | (1 << INT1); // Enable INT0 and INT1
	    MCUCR |= (1 << ISC01); // Trigger INT0 on falling edge
	    MCUCR |= (1 << ISC11); // Trigger INT1 on falling edge
}





void port_Init(void){
    // Setup pushbuttons with internal pull-ups
    DDRD &= ~(1 << PD2); // Set PD2 (INT0) as input
    PORTD |= (1 << PD2); // Enable pull-up on PD2

    DDRD &= ~(1 << PD3); // Set PD3 (INT1) as input
    PORTD |= (1 << PD3); // Enable pull-up on PD3

    DDRC |= 0x0F;

    PORTC= 0x00;
}
void Timer0(void){
	      TCCR0 |= (1 << CS02) | (1 << CS00); // Prescaler of 1024
		  TIMSK |= (1 << TOIE0); // Enable Timer0 overflow interrupt
		  sei(); // Enable global interrupts

}
int main(){
	INT0n1_Init();
	port_Init();
	Timer0();
    LCD_init();


	while(1){}



}

//	if((PIND &(1<<PD2)))
//	{
//		delay();
//
//		if(PIND &(1<<PD2)){
//			PORTC= (motor_steps[steps] & 0b00001111) | (PORTC & 0b11110000);
//					 steps++;
//			if(steps ==FULL_WAVE){
//				steps =0;
//			}
//		}
//		while(PIND & (1<<PD2));
//
//	}
//	else if((PIND &(1<<PD3)))
//	{
//      if(count ==1){
//
//
//		if(PIND &(1<<PD3)){
//			PORTC= (motor_steps_half[steps_2] & 0b00001111) | (PORTC & 0b11110000);
//			              steps_2++;
//			if(steps_2 ==HALF_WAVE_STEPS){
//				steps_2=0;
//			}
//		}
//		while(PIND & (1<<PD3));
//      }
//	}
//
// }


