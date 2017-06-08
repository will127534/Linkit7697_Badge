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