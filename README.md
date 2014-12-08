# IR Sensor Library

### IR Range Sensors for ECE382 Robot Number 003

## Summary

This library provides support and testing functions for the IR distance sensor
interfacing. This library is meant to be used in conjunction with motor control
functions to allow the robot to successfully and autonomously navigate a maze.

## Hardware Setup

The only hardware constraints are that each IR sensor must be tied to a specific
pin on the MSP430, and that the two LED outputs be somewhere on Pin 1. 

For the IR sensor pins: forward is linked with pin 4, left with pin 3, and
right with pin 2. 

The LED pins are by default set so that the left LED is tied to pin 5 and the
right LED to pin 6. If the user elects to change these pins, they must modify
the bit address in the header file to match.

## Software Use

### Library functions

The library functions are meant to be used as standalone tasks to read distances
from the IR sensors. These functions include the read commands, which will 
take the value from a specific sensor then store it in a buffer array; support 
functions, initialize and reset the read buffers; and math commands, intended 
to give the end user flexability over how to read and interpret the data stored
in the buffers.

#### Using the library functions

In order to use the read commands, the user will have to initialize buffer 
arrays for each of the three sensors, populate those arrays, then read and 
interpret the data in those buffers. The buffers must be declared with length
BUFFER_LN which is defined in the rangeFinder.h header file. The buffer length
can be changed, but must be a powewr of 2 for the median function to work 
properly, and must stay at 8 for the mean to work properly. After declaration 
the user should call the `fillBuffers()` method to populate the buffer arrays 
and then can begin measuring. 

The "distance" can be read from the buffers, the median function is recommended
as it is is not dependnent on the a single buffer size and is less responsive 
to outliers in the data reads. The practical distances can be interpolated from
the distance and value chart and plot provided in the pdf. 

#### Using the test functions

A seperate file is included containing testing software to ensure the proper 
hardware setup and code function. It can be useful in troubleshooting any 
problems that arise while working with the library and can provide a tempate 
of functioning code.
