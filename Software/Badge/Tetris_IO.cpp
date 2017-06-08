/*****************************************************************************************
 * File: IO.cpp
 * Desc: Class for handling inpung & drawing, it uses SDL for the rendering. Change the methods 
 * of this class in order to use a different renderer
 *       
 *
 * gametuto.com - Javier Lopez Lopez (javilop.com)
 *
 *****************************************************************************************
 *
 * Creative Commons - Attribution 3.0 Unported
 * You are free:
 *	to Share o to copy, distribute and transmit the work
 *	to Remix o to adapt the work
 *
 * Under the following conditions:
 * Attribution. You must attribute the work in the manner specified by the author or licensor 
 * (but not in any way that suggests that they endorse you or your use of the work).
 *
 *****************************************************************************************/

// ------ Includes -----

#include "Tetris_IO.h"
#include "Control.hpp"

// initialize framebuffer

const size_t SCREEN_W = 18;
const size_t SCREEN_H = 16;
const size_t FRAMEBUF_SIZE = SCREEN_W * SCREEN_H;
extern unsigned char frameBuffer[FRAMEBUF_SIZE] = 
{
 0
};

/* 
======================================									
Init
====================================== 
*/
IO::IO():
	mLastFrame(0)
{
	
}

void IO::Begin()
{
	
	matrix.setRotation(1);
  	matrix.setTextWrap(false);

	mLastFrame = 0;
	matrix.setFrame(mLastFrame);
	matrix.clear();

}


int IO::getFrameIndex()
{
	return mLastFrame;
}

void IO::flip()
{
	matrix.displayFrame(mLastFrame);

	// flip frame number between 1/0
	mLastFrame = 1 - mLastFrame;

	matrix.setFrame(mLastFrame);
	matrix.clear();
}

/* 
======================================									
Clear the screen to black
====================================== 
*/
void IO::ClearScreen() 
{
	matrix.clear();

	// test (0, 0) position
	//matrix.drawPixel(0, 0, 255);
	
	flip();
}

/* 
======================================									
Draw a rectangle of a given color

Parameters:
>> pX1, pY1: 		Upper left corner of the rectangle
>> pX2, pY2: 		Lower right corner of the rectangle
>> pC				Rectangle color
====================================== 
*/
void IO::DrawRectangle (int pX1, int pY1, int pX2, int pY2, enum color pC)
{
	if(pX1 < 0 || pY1 < 0)
	{
		return;
	}
	/*
	Serial.print("drawpixel=");
	Serial.print(pX1);
	Serial.print(pY1);
	Serial.print(pX2);
	Serial.print(pY2);
	Serial.println();
	*/
	// TODO: each block is a single dot - draw dot only
	matrix.drawPixel(pX1, pY1, 100*analogRead(A0)/512.0);
}


/* 
======================================									
Return the screen height
====================================== 
*/
int IO::GetScreenHeight()
{
	return 16;
}


/* 
======================================									
Update screen
====================================== 
*/
void IO::UpdateScreen()
{
	flip();
}


/* 
======================================									
Keyboard Input
====================================== 
*/
int IO::Pollkey()
{
	return scanning_button();
}

/* 
======================================									
Keyboard Input
====================================== 
*/
int IO::Getkey()
{
	// TODO
	return 0;
}

/* 
======================================									
Keyboard Input
====================================== 
*/
int IO::IsKeyDown (int pKey)
{
	// TODO: check if a key is pressed
	return 0;
}

/* 
======================================									
SDL Graphical Initialization
====================================== 
*/
int IO::InitGraph()
{
	// TODO: initializes the graph
    return 0;
}
