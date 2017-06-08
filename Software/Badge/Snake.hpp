#ifndef _Snake_hpp_
#define _Snake_hpp_

#include <deque>
#include "hal_trng.h"
#include "Arduino.h"
#include "Control.hpp"

struct dot_position
{
    int x;
    int y;
};

void Snake_init();
void Snake();
bool Snake_loop();
void Snake_updateScreen();
bool CheckColition(dot_position test_pos);
int GetRand(int pA, int pB);

#endif // _Snake_hpp_