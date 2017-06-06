#include "Snake.hpp"


std::deque<dot_position> history_positions;

dot_position current_position;
dot_position feed;

uint16_t length = 5;
uint32_t last_update = 0;
uint32_t last_pulse = 0;
int x_move,y_move;
bool pulse = 0;
uint32_t pulse_cnt=0;
uint8_t frame_cnt = 0;
uint8_t UPDATE_RATE = 250;

void Snake_init() {

 // std::deque<dot_position> history;
  
  history_positions.erase(history_positions.begin(),history_positions.end());
  hal_trng_status_t ret = hal_trng_init();  //Initializes the TRNG source clock.
  length = 5;
  last_update = millis();
  for(int i=0;i<length;i++){
    dot_position pos;
    pos.x = i;
    pos.y = 7;
    history_positions.push_back(pos);
  }
  current_position= history_positions.back();
  x_move = 1;
  y_move = 0;
  feed.x = GetRand(0,17);
  feed.y = GetRand(0,15);
  UPDATE_RATE = 250;
  Snake_updateScreen();
  Serial.print("SEED");
  Serial.print(feed.x);
  Serial.print(" ");
  Serial.println(feed.y);
  //history_positions = history;
}

void Snake(){
  Snake_init();
  bool loop_snake = 1;
  while(loop_snake){
    loop_snake = Snake_loop(); 
  }
}
bool Snake_loop() {
  // put your main code here, to run repeatedly:
  int button = scanning_button();
  int x_d = 0;
  int y_d = 0;
  switch (button) {
        case 0: //UP
          x_d = 0;
          y_d = -1;
          break;
        case 1: //Down
          x_d = 0;
          y_d = 1;
          // do something
          break;
        case 2: //Left
          x_d = -1;
          y_d = 0;
          break;
        case 3: //Right
          x_d = 1;
          y_d = 0;
          break;
        case 5:{
          uint32_t start_timer = millis();
          while(scanning_button()==5){
          }
          Serial.println(millis()-start_timer);
          if((millis()-start_timer)>800){
            Serial.println("EXIT");
              return 0;
          }
          break;
        }
        case 4: //B
          if( (millis() - last_pulse)>1000 ){
            pulse = !pulse;
            last_pulse = millis();
            last_update = millis()-1000;
          }
          break;        
  }
  
  if(digitalRead(6)){
     return 0;
  }
  //(x_move,y_move) (x_d,y_d)
  if((x_move*x_d)+(y_move*y_d)>0){ //not the reverse of the current movement
      x_move = x_d;
      y_move = y_d;
  }

  //pulse P filcker
  if(pulse == 1 && millis()-last_update>1000 ){
    Snake_updateScreen();
    if(pulse_cnt%2){
      matrix.setTextColor(100*analogRead(A0)/512.0);
      matrix.setCursor(7,4);
      matrix.print("P");
    }
    pulse_cnt ++;
    last_update = millis();
  }
  
  if(millis()-last_update>UPDATE_RATE && pulse != 1){
    current_position.x += x_move;
    current_position.y += y_move;
    if(current_position.x>17){
      current_position.x -= 18;
    }
    if(current_position.x<0){
      current_position.x += 18;
    }
    if(current_position.y>15){
      current_position.y -= 16;
    }
    if(current_position.y<0){
      current_position.y += 16;
    }
    if(CheckColition(current_position)){
      Serial.println("Game END");
        for (int16_t x=18; x>=-8*10; x--) {
                        matrix.clear();
                        matrix.setCursor(x,0);
                        matrix.print("Gameover");
                        delay(25);
        }
        return 0;
    }
    if(current_position.x == feed.x && current_position.y == feed.y){
          length+=1;
          feed.x = GetRand(0,17);
          feed.y = GetRand(0,15);
          while(CheckColition(feed)){
             feed.x = GetRand(0,17);
             feed.y = GetRand(0,15);
          }
          Serial.print("SEED");
          Serial.print(feed.x);
          Serial.print(" ");
          Serial.println(feed.y);
          UPDATE_RATE = 1.0/length*1000.0;

    }
    Serial.print(current_position.x);
    Serial.print(" ");
    Serial.println(current_position.y);
    history_positions.push_back(current_position);
    if(history_positions.size()>length){
        history_positions.pop_front();
    }
    Snake_updateScreen();
    last_update = millis();
  }
  return 1;
}
void Snake_updateScreen(){
  matrix.setFrame(frame_cnt%2);
  matrix.clear();
  for(std::deque<dot_position>::iterator it = history_positions.begin(); it != history_positions.end(); it++){
     dot_position pos = *it;
     matrix.drawPixel(pos.x,pos.y,100*analogRead(A0)/512.0);
  }
  matrix.drawPixel(feed.x,feed.y,100*analogRead(A0)/512.0);
  matrix.displayFrame((frame_cnt)%2);
  frame_cnt ++;
}
bool CheckColition(dot_position test_pos){
  for(std::deque<dot_position>::iterator it = history_positions.begin(); it != history_positions.end(); it++){
     dot_position pos = *it;
     if(test_pos.x == pos.x && test_pos.y == pos.y){
      return 1;
     }
  }
  return 0;
}

int GetRand (int pA, int pB)
{
  uint32_t number = 0;
  hal_trng_get_generated_random_number(&number); 
  return number % (pB - pA + 1) + pA;
}

