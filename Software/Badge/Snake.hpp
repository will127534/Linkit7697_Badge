////////////////////////////////////////////////////////////////////////////
///
/// Please report bugs and suggestions to https://github.com/MediaTek-Labs/Linkit7697_Badge
///
/// Author: Will Huang
///
/// The "Tetris" part is based on code from gametuto.com - Javier Lopez Lopez (javilop.com)
///
/// Copyright (C) 2017 Will Huang
/// 
/// Permission is hereby granted, free of charge, to any person obtaining a 
/// copy of this software and associated documentation files (the "Software"), 
/// to deal in the Software without restriction, including without limitation the
/// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
/// sell copies of the Software, and to permit persons to whom the Software is 
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included 
/// in all copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
/// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
/// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
/// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
/// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
/// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

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