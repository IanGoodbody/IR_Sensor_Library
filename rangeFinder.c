/*
 * rangeFinder.c
 *
 *  Created on: Dec 4, 2014
 *      Author: Ian Goodbody
 *    Function: Provides the basic IR rangefinder function library. Allows the user to interface with the rangefinders
 *    			my specific robot (DFECE #3) with the wiring scematic and distance classifications provided.
 */

#include "rangeFinder.h"
#include <msp430.h>

/*
 * void initRangeFinders()
 * 		Initializes the ADC control registers, call in program initialization
 */
void initRangeFinders()
{
	ADC10CTL0 |= ADC10SHT_3|ADC10ON|ADC10IE;
	ADC10CTL1 |= ADC10SSEL1|ADC10SSEL0;

}

/*
 * void readRight(int *bufferPtr)
 * 		Read the ADC output connected to the right sensor then rotate it into the buffer array provided
 * 		Note: buffer arrays must be declarfed with length BUFFER_LN set in the header file.
 */
void readRight(int *bufferPtr)
{
	ADC10CTL1 &= ~INCH_15;
	ADC10CTL1 |= INCH_0;
	ADC10AE0 |= BIT0;
	ADC10CTL0 |= ENC|ADC10SC;
	__bis_SR_register(CPUOFF + GIE);
	ADC10CTL0 &= ~(ENC|ADC10SC);
	rotateIn(bufferPtr, ADC10MEM);
}

/*
 * void readLeft(int *bufferPtr)
 * 		Read the ADC output connected to the left sensor then rotate it into the buffer array provided
 * 		Note: buffer arrays must be declarfed with length BUFFER_LN set in the header file.
 */
void readLeft(int *bufferPtr)
{
	ADC10CTL1 &= ~INCH_15;
	ADC10CTL1 |= INCH_1;
	ADC10AE0 |= BIT1;
	ADC10CTL0 |= ENC|ADC10SC;
	__bis_SR_register(CPUOFF + GIE);
	ADC10CTL0 &= ~(ENC|ADC10SC);
	rotateIn(bufferPtr, ADC10MEM);
}

/*
 * void readFront(int *bufferPtr)
 * 		Read the ADC output connected to the Front sensor then rotate it into the buffer array provided
 * 		Note: buffer arrays must be declarfed with length BUFFER_LN set in the header file.
 */
void readFront(int *bufferPtr)
{
	ADC10CTL1 &= ~INCH_15;
	ADC10CTL1 |= INCH_2;
	ADC10AE0 |= BIT2;
	ADC10CTL0 |= ENC|ADC10SC;
	__bis_SR_register(CPUOFF + GIE);
	ADC10CTL0 &= ~(ENC|ADC10SC);
	rotateIn(bufferPtr, ADC10MEM);
}

/*
 * void fillBuffers(int *forwardBufferPtr, int *leftBufferPtr, int *rightBufferPtr)
 * 		Provides an initial read or reset for the IR distance buffers. It is important to call
 * 		prior to movement as the arbitrary values from memory may corrup the first set of moves
 * 		from the robot.
 */
void fillBuffers(int *forwardBufferPtr, int *leftBufferPtr, int *rightBufferPtr)
{
	int i;
	ADC10CTL1 &= ~INCH_15;
	ADC10CTL1 |= INCH_0;
	ADC10AE0 |= BIT0;
	for (i = BUFFER_LN + 1; i > 0; i--) // Take 9 measurements just to be sure
	{
		ADC10CTL0 |= ENC|ADC10SC;
		__bis_SR_register(CPUOFF + GIE);
		ADC10CTL0 &= ~(ENC|ADC10SC);
		rotateIn(leftBufferPtr, ADC10MEM);
	}

	ADC10CTL1 &= ~INCH_15;
	ADC10CTL1 |= INCH_1;
	ADC10AE0 |= BIT1;
	for (i = BUFFER_LN + 1; i > 0; i--)
	{
		ADC10CTL0 |= ENC|ADC10SC;
		__bis_SR_register(CPUOFF + GIE);
		ADC10CTL0 &= ~(ENC|ADC10SC);
		rotateIn(rightBufferPtr, ADC10MEM);
	}

	ADC10CTL1 &= ~INCH_15;
	ADC10CTL1 |= INCH_2;
	ADC10AE0 |= BIT2;
	for (i = BUFFER_LN + 1; i > 0; i--)
	{
		ADC10CTL0 |= ENC|ADC10SC;
		__bis_SR_register(CPUOFF + GIE);
		ADC10CTL0 &= ~(ENC|ADC10SC);
		rotateIn(forwardBufferPtr, ADC10MEM);
	}
}

/*
 * __interrupt void ADC10_ISR(void)
 * 		ISR for low power operation durring the ADC process
 */
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
  __bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
}

/*
 * void rotateIn(int *bufferPtr, int val)
 *		Moves all values down one index, dropes the value at index 0, then adds
 * 		val to the last index value. Used by the read in frunctions to cycle
 * 		values through the buffer
 */
void rotateIn(int *bufferPtr, int val)
{
	int i;
	for(i = BUFFER_LN-1; i > 0; i--) // Will run BUFFER_LN-1 times
	{
		*bufferPtr = *(bufferPtr+1);
		bufferPtr++;
	}
	*bufferPtr = val;
}

/*
 * int mean(int *bufferPtr)
 * 		Computes the average of the specified buffer array. The mean function
 * 		will only work for 8 item buffers unless the code is changed
 */
int mean(int *bufferPtr)
{
	int i;
	int sum = *(bufferPtr++);
	for(i = BUFFER_LN-1; i > 0; i--)
	{
		sum += *(bufferPtr++);
	}
	return sum >> 3; // Code built for a buffer size of 8, must be changed
}

/*
 * int median(int *bufferPtr)
 * 		Computes the median of the specified buffer array. This funciton works best if
 * 		BUFFER_LN is a power of 2, but unlike the mean is not intrinsicly tied to a single
 * 		value of BUFFER_LN
 */
int median(int *bufferPtr)
{
	int sortBuffer[BUFFER_LN];

	// Copy over the data buffer
	int *sortBufferPtr = sortBuffer;
	int i, j;
	for(i = BUFFER_LN; i > 0; i--)
	{
		*(sortBufferPtr++) = *(bufferPtr++);
	}

	// Bubble sort through half the sort array
	for(j = BUFFER_LN; j > (BUFFER_LN>>1)-1; j--)
	{
		sortBufferPtr = sortBuffer;
		for(i = 0; i < j-1; i++)
		{
			if(*sortBufferPtr > *(sortBufferPtr+1))
			{
				*sortBufferPtr ^= *(sortBufferPtr+1);
				*(sortBufferPtr+1) ^= *sortBufferPtr;
				*sortBufferPtr ^= *(sortBufferPtr+1);
			}
			sortBufferPtr++;
		}
	}
	return (sortBuffer[BUFFER_LN>>1]+sortBuffer[(BUFFER_LN>>1)-1])>>1; // Return the mean of the two middle values in the sort array.
}
