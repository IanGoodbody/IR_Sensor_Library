/*
 * testMethods.c
 *
 *  Created on: Dec 4, 2014
 *      Author: Ian Goodbody
 *    Function: Provides an array of methods for testing various range finder functions.
 *    			It requires interfacing with LEDs on the MSP430, and the code is built so that
 *    			values may be observed in the CCS debugging interface.
 */

#include <MSP430.h>
#include "rangeFinder.h"
#include "testMethods.h"


/*
 * void testMath()
 * 		Tests the array mathematics functions available for use by the user.
 * 		Ideal function would be to step through each computation for each array.
 */
void testMath()
{
    int i;
    volatile int testArray[BUFFER_LN];
    for(i = 8; i > 0; i--)
    {
    	rotateIn(testArray, i);
    }
    volatile int average = mean(testArray); // Place Breakpoint here to skip array population
    volatile int middle = median(testArray);

    volatile int testArray2[BUFFER_LN] = {16, 6, 12, 10, 4, 14, 8, 2};
    average = mean(testArray2);
    middle = median(testArray2);
}

/*
 * void testLEDsTimer()
 * 		Ensures that the LEDs are connected to the correct pins on the MSP430. Simply run
 * 		this function, one should observe the two LEDs flashing on the launchpad
 */
void testLEDsTimer()
{
	BCSCTL1 = CALBC1_8MHZ;
	DCOCTL = CALDCO_8MHZ;

	P1DIR |= LEFT_LED|RIGHT_LED;
	P1OUT |= LEFT_LED;
	P1OUT &= ~RIGHT_LED;

	TACTL &= ~(MC_3);
	TACTL &= ~TAIFG;
	TACTL |= TASSEL_2|ID_3|TAIE;
	TACCR0 = 0xFFFF;
	TAR = 0x0000;

	//IFG1 = 1;

	TACTL |= MC_1;

}

/*
 * __interrupt void timerA0Overflow(void)
 * 		ISR for the preceding function, toggles the LEDs on timer overflow.
 */
#pragma vector = TIMER0_A1_VECTOR
__interrupt void timerA0Overflow(void)
{
	P1OUT ^= (LEFT_LED|RIGHT_LED);
	TA0CTL &= ~TAIFG;
}

/*
 * testRangeFinders()
 * 		This function is used to test and classify the idealized function for the range finders, as well as
 * 		implementing the basic functionality. It will turn LEDs on and off depending on if an object comes
 * 		within a certain range (see spreadsheet for distance and readouts). Setting a breakpoint at the top
 * 		of the while loop while the code is running will give a good value for what the ADC reads at a particular
 * 		distance.
 */
void testRangeFinders()
{
	BCSCTL1 = CALBC1_8MHZ;
	DCOCTL = CALDCO_8MHZ;

	initRangeFinders();

	P1DIR |= LEFT_LED|RIGHT_LED;


	int fBuffer[BUFFER_LN];
	int lBuffer[BUFFER_LN];
	int rBuffer[BUFFER_LN];
	int mf;
	int ml;
	int mr;

	fillBuffers(fBuffer, lBuffer, rBuffer);

	while(1)
	{
		readFront(fBuffer); // Place Breakpoint here
		readLeft(lBuffer);
		readRight(rBuffer);
		mf = median(fBuffer); // Medians were chosen as they are less influenced by outliers
		ml = median(lBuffer); // The user is free to change these functions to test and classify the
		mr = median(rBuffer); // means, however they should be close to the median values.

		if(mf > 0x01F0)
		{
			P1OUT |= (RIGHT_LED|LEFT_LED);
		}
		else if(ml > 0x0220)
		{

			P1OUT &= ~RIGHT_LED;
			P1OUT |= LEFT_LED;
		}
		else if(mr > 0x0220)
		{
			P1OUT &= ~LEFT_LED;
			P1OUT |= RIGHT_LED;
		}
		else
		{ // Important reset condition, do not forget in robot mevement code
			P1OUT &= ~(LEFT_LED|RIGHT_LED);
		}
	}

}

/*
 * void testBuffers(void)
 * 		Tests the fillBuffers function. Placing a break point at the top of the while loop allows
 * 		the user to ensure that the buffer resets on each iteration and that the math functions change
 * 		appropriately (origionally intended for use in classifying sensors)
 */
void testBuffers(void)
{
	initRangeFinders();

	int fBuffer[BUFFER_LN];
	int lBuffer[BUFFER_LN];
	int rBuffer[BUFFER_LN];

	volatile int fMean;
	volatile int fMed;
	volatile int rMean;
	volatile int rMed;
	volatile int lMean;
	volatile int lMed;

	while(1)
	{
		fillBuffers(fBuffer, lBuffer, rBuffer); // Place Breakpoint here
		fMean = mean(fBuffer);
		fMed = median(fBuffer);
		lMean = mean(lBuffer);
		lMed = median(lBuffer);
		rMean = mean(rBuffer);
		rMed = median(rBuffer);
	}
}
