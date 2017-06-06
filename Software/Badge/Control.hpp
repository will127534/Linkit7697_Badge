#ifndef _Control_hpp_
#define _Control_hpp_

#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_IS31FL3731.h>

extern Adafruit_IS31FL3731 matrix ;


#define UP_BUTTON 4   //GPIO34
#define DOWN_BUTTON 16  //GPIO59
#define LEFT_BUTTON 5  //GPIO33
#define RIGHT_BUTTON 17  //GPIO60
#define CENTER_BUTTON 15 //GPIO58

#define X_BUTTON 10//GPIO32 
#define Y_BUTTON 12//GPIO30 
#define A_BUTTON 13//GPIO31
#define B_BUTTON 11//GPIO29 

extern byte buttons[9];
int scanning_button();
int scanning_button_block();

#endif // _Control_hpp_