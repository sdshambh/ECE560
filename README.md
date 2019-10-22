# ECE560
Embedded System Design

Project 1: μSD CARD READERS
----------------------------

Improving the responsiveness of system by giving control to the scheduler sooner and removing the busy wait loops.
The base program uses the open source SD card interface code ulibSD (available on github) by Nelson Lombardo based on
work by Chan. The program does the following:

1. Initializes card controller
2. Repeats these steps, starting with the first sector and then advancing.
-> Reads the next 100 sectors (blocks) of data (each 512 bytes long) from the μSD card.
->Writes test data to the next sector and reads it back to verify correct operation.
3. Also does make‐work (approximating π) which represents other processing/threads in the program.

The systems is improved by two startegies:
1) By building a scheduler that takes over the control and schedules tasks in non-preemptive (non-prioritize tasks) way. Which improves
the responsiveness of the system as the scheduler can respond faster to the tasks instead of waiting in busy wait loops.
2) By using RTX5 (real-time kernel) RTOS to schedule the tasks based on their priority and allow preemption.

The responsiveness improved from millisecond to some μsecond.


Project 2: Sharing the ADC
-----------------------------

The ADC is shared between the buck converter controller and the touchscreen code, while still maintaining correct timing for the 
buck converter controller.
On toughing the touchscreen either of the two things happens:
If touched in upper portion of screen (above the Dim  Bright text), draw a white line between previous and
current touch points.
 If touched in lower portion of screen (on or below the Dim  Bright text), set the peak current
(g_peak_set_current) to between 1 and 120 mA, based on the X position of the touch.

This functionality is achieved by using RTX5 RTOS and ADC is shared as a resource between the threads.
The conversion requests to ADC is buffered in message queue.

Thread_Read_TS calls LCD_TS_Read. LCD_TS_Read uses a digital input to determine if the screen is pressed. If it is pressed,
it uses the ADC twice to determine where the screen is pressed (once for the X axis, once for the Y axis).

LCD_TS_Read returns a 1 indicating the press, and writes the coordinates into the structure pointed to by the function’s
argument (called position). If the screen is not pressed, LCD_TS_Read does not use the ADC and returns a value of 0.
Thread_Read_TS checks to see which part of the screen is pressed. If the upper part is pressed, then it draws a line between
the previous and current touch locations. If the lower part is pressed, it updates the global variable g_peak_set_current
based on the X coordinate. Thread_Buck_Update_Setpoint manages the timing of the flash, and uses g_peak_set_current
to set the current levels for the flash.

There are two types of ADC conversion. Conversions for the buck converter are time‐critical, so they are triggered by the
overflow signal of timer TPM0. Conversions for the queued requests are triggered by software in the ADC ISR.
