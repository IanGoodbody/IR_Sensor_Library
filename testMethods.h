/*
 * testMethods.h
 *
 *  Created on: Dec 4, 2014
 *      Author: Ian Goodbody
 */

#ifndef TESTMETHODS_H_
#define TESTMETHODS_H_

#define LEFT_LED BIT4
#define RIGHT_LED BIT5

void testMath();
void testLEDsTimer();
__interrupt void timerA0Overflow(void);
void testRangeFinders();
void testBuffers();

#endif /* TESTMETHODS_H_ */
