    
#include <msp430.h>
#include "msp430g2553.h"
#define PWM_Period	20000
#define MAXDUTY 2300		// 160 Degrees
#define MINDUTY 600			// 0 Degrees


/* Luis Santos	LAB 5 Controlling servo with PWM and timers
 * main.c
 */

unsigned int incdec_speed = 50; //controls motors' increment value(higher the value, the faster the motor goes


int go = 0;						//toggles PWM from on to off in Port1 Interrupt


int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    //Setup P1.3 for stop and start
    P1DIR &= ~BIT3;
    P1REN |= BIT3;
    P1OUT |= BIT3;
    P1IE |= BIT3;
    P1IES |= BIT3;
    P1IFG =0;

    //Setup leds (red for stoped, green for moving)
    P1DIR |= BIT0 + BIT6;
    P1OUT |= BIT0;
    P1OUT &= ~BIT6;

    //Setupt PWM - TIMER A0.1 - P2.6
	P2DIR |=BIT6;
	P2SEL &=BIT6;

	//PWM TIMER
	TA0CTL = TASSEL_2 + MC_0 ;
	TA0CCTL0 = CCIE;		//Enable interrupt for TimerA0
	TA0CCTL1 = OUTMOD_7;   //Using togle set output mode
	TA0CCR0 = PWM_Period;//Period of 20ms (50hz)
	TA0CCR1 = MINDUTY;//duty cycle of 6ms

	_enable_interrupts();
	while(1)
	{

	}
}

//Toggles PWM timer on and off, lights up indicating led
#pragma vector = PORT1_VECTOR
__interrupt void Port1(void)
{
	if (go == 0)
	{
		TA0CTL ^= MC_1;
		go = 1;
		P1OUT &= ~BIT0;
		P1OUT |= BIT6;
	}
	else if(go==1)
	{
		TA0CTL &= ~MC_1;
		go = 0;
		P1OUT |=BIT0;
		P1OUT &= ~BIT6;
	}
	P1IFG &= ~BIT3; // Clear Button Flag
}


//Increments TA0CCR1 for full 160 degree movement back and forth
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
	if (go == 1)
	{
		TA0CCR1 += incdec_speed;
		if (TA0CCR1 > MAXDUTY || TA0CCR1 < MINDUTY )	//changes directions of servo
		{
			incdec_speed = -incdec_speed;
		}
	}
}
