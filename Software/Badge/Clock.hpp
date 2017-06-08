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

#ifndef _SNAKE_HPP_
#define _SNAKE_HPP_

#include <Arduino.h>
#include <stdint.h>
#include <time.h>

/// Update local clock time from NTP protocol
void syncClock();

/// Register RTC interrupt and update time every second
void startClock();

/// Check if 1 second have passed;
/// If it is passed, true is returned and
/// the flag is cleared.
///
/// \returns true if 1 second is passed
/// \returns false if haven't passed 1 second since last call.
bool getAndClearUpdateFlag();

/// Get current time since 1900
unsigned long getEpoch();

/// Convert from epoch time to time struct
struct tm NTPparse(uint32_t epoch_time);

#endif // _SNAKE_HPP_