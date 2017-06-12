#include "Floppy.hpp"
#include "Control.hpp"


std::deque<box_position> boxes;

Floppy_dot_position current_Floppy_position;



uint32_t Floppy_last_update = 0;
uint32_t Floppy_last_pulse = 0;
int Floppy_y_move;
bool Floppy_pulse = 0;
uint32_t Floppy_pulse_cnt=0;
int gravaty = -1;
int y_speed = 0;
uint8_t Floppy_frame_cnt = 0;


void Floppy(){
  Floppy_init();
  bool loop = 1;
  while (loop)
  {
    loop = Floppy_loop();
  }
}
void Floppy_init() {
  // put your setup code here, to run once:

  hal_trng_status_t ret = hal_trng_init();  //Initializes the TRNG source clock.
  

  boxes.erase(boxes.begin(), boxes.end());
   Floppy_last_update = 0;
   Floppy_last_pulse = 0;
   Floppy_y_move;
   Floppy_pulse = 0;
   Floppy_pulse_cnt=0;
   gravaty = -1;
   y_speed = 0;
   Floppy_frame_cnt = 0;

  Floppy_last_update = millis();

  Floppy_y_move = 0;

  box_position a;
  a.x = 10;
  a.h_TOP = 5;
  a.h_BOTTOM = 5;
  boxes.push_back(a);
  
  box_position a2;
  a2.x = 18;
  a2.h_TOP = 8;
  a2.h_BOTTOM = 3;
  boxes.push_back(a2);

  box_position a3;
  a2.x = 26;
  a2.h_TOP = 7;
  a2.h_BOTTOM = 4;
  boxes.push_back(a2);

  current_Floppy_position.x = 4;
  current_Floppy_position.y = 6;

  Floppy_updateScreen();

}

bool Floppy_loop() {
  // put your main code here, to run repeatedly:
    if (digitalRead(USR_BUTTON))
    {
      return 0;
    }
    int button = scanButton();
    switch (button) {
        case UP_BUTTON: //UP
          Floppy_y_move = 1;
          break;

        case CENTER_BUTTON: //X
          if( (millis() - Floppy_last_pulse)>1000 ){
            Floppy_pulse = !Floppy_pulse;
            Floppy_last_pulse = millis();
            Floppy_last_update = millis()-1000;
          }
          break;        
    }
    
  if(Floppy_pulse == 1 && millis()-Floppy_last_update>1000 ){
    Floppy_updateScreen();
    if(Floppy_pulse_cnt%2){
      matrix.setTextColor(100*analogRead(A0)/512.0);
      matrix.setCursor(7,4);
      matrix.print("P");
    }
    Floppy_pulse_cnt ++;
    Floppy_last_update = millis();
  }
  
  if(millis()-Floppy_last_update>200 && Floppy_pulse != 1){

    if(Floppy_y_move == 1){
        y_speed = -1;
        Floppy_y_move = 0;
    }
    current_Floppy_position.y += y_speed;
    y_speed += 1;
    if(current_Floppy_position.x>17){
      current_Floppy_position.x -= 18;
    }
    if(current_Floppy_position.y>15){
      current_Floppy_position.y=15;
      Floppy_updateScreen();
      Serial.println("Game END");
      for (int16_t x = 18; x >= -8 * 10; x--)
      {
        matrix.clear();
        matrix.setCursor(x, 0);
        matrix.print("Gameover");
        delay(25);
      }
      return 0;
    }
    for(std::deque<box_position>::iterator it = boxes.begin(); it != boxes.end(); it++){
     it->x --;
     if(it->x < -BOX_WITH){
        boxes.pop_front();
     }
    if(it->x == 0  ){
        box_position new_block;
        new_block.x = 25;
        new_block.h_BOTTOM = Floppy_GetRand(3,8);
        new_block.h_TOP = Floppy_GetRand(3,16-new_block.h_BOTTOM-5);
        boxes.push_back(new_block);
     }

    }

    
    if(Floppy_CheckColition()){
      Serial.println("Game END");
      for (int16_t x = 18; x >= -8 * 10; x--)
      {
        matrix.clear();
        matrix.setCursor(x, 0);
        matrix.print("Gameover");
        delay(25);
      }
      return 0;
    }
    
    Serial.print(current_Floppy_position.x);Serial.print(" ");Serial.println(current_Floppy_position.y);
    Floppy_updateScreen();
    Floppy_last_update = millis();
  }
  return 1;
}


void Floppy_updateScreen(){
  matrix.setFrame(Floppy_frame_cnt%2);
  matrix.clear();
  float light = analogRead(A0)/512.0;
  matrix.drawPixel(current_Floppy_position.x,current_Floppy_position.y,180*light);
  
  for(std::deque<box_position>::iterator it = boxes.begin(); it != boxes.end(); it++){
     box_position pos = *it;
     matrix.fillRect(pos.x,0,BOX_WITH,pos.h_TOP,60*light);
     matrix.fillRect(pos.x,16-pos.h_BOTTOM,BOX_WITH,pos.h_BOTTOM,60*light);
  }
  matrix.displayFrame((Floppy_frame_cnt)%2);

  Floppy_frame_cnt ++;
}

bool Floppy_CheckColition(){
  for(std::deque<box_position>::iterator it = boxes.begin(); it != boxes.end(); it++){
     box_position pos = *it;
     /*
     |
     |==>check A
     |
     |     
     |------ ==> Check B

     |------ ==>Check C
     |
     |
     |

     */
     if(current_Floppy_position.x >= pos.x && current_Floppy_position.x < pos.x+BOX_WITH && (current_Floppy_position.y <= pos.h_TOP || current_Floppy_position.y >= 16-pos.h_BOTTOM) ){
      if(current_Floppy_position.x == pos.x){  //Check A ==>Drop animation 
          current_Floppy_position.x = pos.x -1;
          while(current_Floppy_position.y < 15){
            current_Floppy_position.y ++;
            Floppy_updateScreen();
            delay(100);
          }
          delay(1000);
      }
      else if(current_Floppy_position.y <= pos.h_TOP){  //Check B ==>Drop animation to bottom
          current_Floppy_position.y = pos.h_TOP -1;
          while(current_Floppy_position.y < 16-pos.h_BOTTOM -1){
            current_Floppy_position.y ++ ;
            Floppy_updateScreen();
            delay(100);
          }
          delay(1000);

      }
      else{   //Check C
        current_Floppy_position.y = 16-pos.h_BOTTOM -1;
        Floppy_updateScreen();
        return 1;
      }


      
      return 1;
     }
  }
  return 0;
}

int Floppy_GetRand (int pA, int pB)
{
  uint32_t number = 0;
  hal_trng_get_generated_random_number(&number); 
  return number % (pB - pA + 1) + pA;
}
