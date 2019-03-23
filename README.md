# Arduino-PWM-expander-tcl5940
TCL5940 with arduino and Embedded C/C++ 

 Arduino UNO, nano, and Pro mini has 6 independent PWM channels.
 But in some cases we need more than 6 PWM generators for projects like Hexapods, BiPods, Multicopters and LED drivers.
 TCL5940 is a popular IC made for LED display Drivers and also be used for Servo and ESC controllers.
 But this is an Gray Scale driver which means this IC generates the PWM signal but with a 90 degrees of phase shift.
 So to control a servo or an ESC, the electronics is kind of different from the Arduino connection.
 Following shows how to configure the electronics for Servo or Motor controllers.
                  ________            ________
 Vcc (+5V)<------|________|----------|PWM-pinx|
                                |    |   uC   |
                                |    |________|
                                |
                   (ESC, Servo or the MOTOR Driver.)    

 By Buddhika Abeysekera...
 For more tutorials like this, Check on:
 LinkedIn: https://www.linkedin.com/in/babey/
 GitHub: https://github.com/mbabeysekera/
