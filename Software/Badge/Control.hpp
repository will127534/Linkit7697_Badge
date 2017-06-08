#ifndef _Control_hpp_
#define _Control_hpp_

#include "Arduino.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_IS31FL3731.h>
#include "SparkFunLIS3DH.h"

/// This matrix object can be used to control the on-board LED matrix.
extern Adafruit_IS31FL3731 matrix ;

/// Default constructor is I2C, addr 0x18.
extern LIS3DH myIMU;

#define USR_BUTTON (6)      ///< Arduino Pin # for USR button on the side of the board, GPIO37. Active HIGH.
#define USR_LED (3)      ///< Arduino Pin # for USR button on the side of the board, GPIO37. Active HIGH.

#define UP_BUTTON (4)       ///< Arduino Pin # for on-board directional key, GPIO34, active LOW.
#define DOWN_BUTTON (16)    ///< Arduino Pin # for on-board directional key, GPIO59, active LOW.
#define LEFT_BUTTON (5)     ///< Arduino Pin # for on-board directional key, GPIO33, active LOW.
#define RIGHT_BUTTON (17)   ///< Arduino Pin # for on-board directional key, GPIO60, active LOW.
#define CENTER_BUTTON (15)  ///< Arduino Pin # for on-board directional key, GPIO58, active LOW.

#define X_BUTTON (10)       ///< Arduino Pin # for on-board button, GPIO32, active LOW.
#define Y_BUTTON (12)       ///< Arduino Pin # for on-board button, GPIO30, active LOW.
#define A_BUTTON (13)       ///< Arduino Pin # for on-board button, GPIO31, active LOW.
#define B_BUTTON (11)       ///< Arduino Pin # for on-board button, GPIO29, active LOW. 
#define NO_BUTTON (-1)      ///< No button pressed

#define BUTTONS_SIZE (9)

/// This array contains Arduino Pin # for all active-LOW buttons. 
/// Useful for enumerating through all button
extern byte buttons[BUTTONS_SIZE];


/// Initialize button modes (input mode & pull up/down)
void initButtons();

/// Check if any button is pressed
///
/// \returns key code (Arduino Pin #) such as X_BUTTON if a button is pressed.
/// \returns -1 (NO_BUTTON) if no button pressed.
int scanButton();

/// Check if any button is pressed.
/// If a button is pressed, BLOCK until it is released.
///
/// \returns key code (Arduino Pin #) such as X_BUTTON if a button is pressed and then released.
/// \returns -1 (NO_BUTTON) if no button pressed.
int scanButtonUp();

#endif // _Control_hpp_