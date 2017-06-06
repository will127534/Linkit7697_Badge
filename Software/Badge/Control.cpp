#include "Arduino.h"
#include "Control.hpp"
//#################### Control ####################
Adafruit_IS31FL3731 matrix = Adafruit_IS31FL3731();

byte buttons[9] = {UP_BUTTON,DOWN_BUTTON,LEFT_BUTTON,RIGHT_BUTTON,CENTER_BUTTON,A_BUTTON,B_BUTTON,X_BUTTON,Y_BUTTON};

int scanning_button(){
  for(int i=0; i<9; i++){
      if(digitalRead(buttons[i])==0)
         return i;
  }
  return -1;
}
int scanning_button_block(){
  for(int i=0; i<9; i++){
      if(digitalRead(buttons[i])==0){
        while(digitalRead(buttons[i])==0){
            delay(1);
        }
        return i;
      }
         
  }
  return -1;
}
