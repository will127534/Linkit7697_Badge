/*****************************************************************************************
 * File: Game.cpp
 * Desc: General class for the game
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
#include <Arduino.h>
#include "Tetris_Game.h"

/* 
======================================									
Init
====================================== 
*/
Game::Game(Board *pBoard, Pieces *pPieces, IO *pIO, int pScreenHeight)
{
	mScreenHeight = pScreenHeight;

	// Get the pointer to the Board and Pieces classes
	mBoard = pBoard;
	mPieces = pPieces;
	mIO = pIO;

	// Game initialization
	InitGame();
}

/* 
======================================									
Get a random int between to integers

Parameters:
>> pA: First number
>> pB: Second number
====================================== 
*/
int Game::GetRand(int pA, int pB)
{
	return rand() % (pB - pA + 1) + pA;
}

/* 
======================================									
Initial parameters of the game
====================================== 
*/
void Game::InitGame()
{
	// Init random numbers
	srand((unsigned int)millis());

	// First piece
	mPiece = GetRand(0, 6);
	mRotation = GetRand(0, 3);
	mPosX = (BOARD_WIDTH / 2) + mPieces->GetXInitialPosition(mPiece, mRotation);
	mPosY = mPieces->GetYInitialPosition(mPiece, mRotation);

	//  Next piece
	mNextPiece = GetRand(0, 6);
	mNextRotation = GetRand(0, 3);
}

/* 
======================================									
Create a random piece
====================================== 
*/
void Game::CreateNewPiece()
{
	// The new piece
	mPiece = mNextPiece;
	mRotation = mNextRotation;
	mPosX = (BOARD_WIDTH / 2) + mPieces->GetXInitialPosition(mPiece, mRotation);
	mPosY = mPieces->GetYInitialPosition(mPiece, mRotation);

	// Random next piece
	mNextPiece = GetRand(0, 6);
	mNextRotation = GetRand(0, 3);
}

/* 
======================================									
Draw piece

Parameters:

>> pX:		Horizontal position in blocks
>> pY:		Vertical position in blocks
>> pPiece:	Piece to draw
>> pRotation:	1 of the 4 possible rotations
====================================== 
*/
void Game::DrawPiece(int pX, int pY, int pPiece, int pRotation)
{
	color mColor; // Color of the block
				  /*
	Serial.println("draw piece");
	Serial.print(pX);
	Serial.print(" ");
	Serial.print(pY);
	Serial.println();
*/
	// Obtain the position in pixel in the screen of the block we want to draw
	int mPixelsX = mBoard->GetXPosInPixels(pX);
	int mPixelsY = mBoard->GetYPosInPixels(pY);

	// Travel the matrix of blocks of the piece and draw the blocks that are filled
	for (int i = 0; i < PIECE_BLOCKS; i++)
	{
		for (int j = 0; j < PIECE_BLOCKS; j++)
		{
			// Get the type of the block and draw it with the correct color
			switch (mPieces->GetBlockType(pPiece, pRotation, j, i))
			{
			case 1:
				mColor = GREEN;
				break; // For each block of the piece except the pivot
			case 2:
				mColor = BLUE;
				break; // For the pivot
			}

			if (mPieces->GetBlockType(pPiece, pRotation, j, i) != 0)
				mIO->DrawRectangle(mPixelsX + i * BLOCK_SIZE,
								   mPixelsY + j * BLOCK_SIZE,
								   (mPixelsX + i * BLOCK_SIZE) + BLOCK_SIZE - 1,
								   (mPixelsY + j * BLOCK_SIZE) + BLOCK_SIZE - 1,
								   mColor);
		}
	}
}

/* 
======================================									
Draw board

Draw the two lines that delimit the board
====================================== 
*/
void Game::DrawBoard()
{
	// Drawing the blocks that are already stored in the board
	for (int i = 0; i < BOARD_WIDTH; i++)
	{
		for (int j = 0; j < BOARD_HEIGHT; j++)
		{
			// Check if the block is filled, if so, draw it
			if (!mBoard->IsFreeBlock(i, j))
				mIO->DrawRectangle(i * BLOCK_SIZE,
								   j * BLOCK_SIZE,
								   i * BLOCK_SIZE + BLOCK_SIZE - 1,
								   j * BLOCK_SIZE + BLOCK_SIZE - 1,
								   RED);
		}
	}
}

/* 
======================================									
Draw scene

Draw all the objects of the scene
====================================== 
*/
void Game::DrawScene()
{
	DrawBoard();								// Draw the delimitation lines and blocks stored in the board
	DrawPiece(mPosX, mPosY, mPiece, mRotation); // Draw the playing piece
}
