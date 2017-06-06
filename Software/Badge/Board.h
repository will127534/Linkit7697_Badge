/*****************************************************************************************
/* File: Board.h
/* Desc: Board of the game. A matrix of n x n holes.
/*
/* gametuto.com - Javier L�pez L�pez (javilop.com)
/*
/*****************************************************************************************
/*
/* Creative Commons - Attribution 3.0 Unported
/* You are free:
/*	to Share � to copy, distribute and transmit the work
/*	to Remix � to adapt the work
/*
/* Under the following conditions:
/* Attribution. You must attribute the work in the manner specified by the author or licensor 
/* (but not in any way that suggests that they endorse you or your use of the work).
/*
/*****************************************************************************************/

#ifndef _BOARD_
#define _BOARD_

// ------ Includes -----

#include "Pieces.h"

// ------ Defines -----

#define BOARD_LINE_WIDTH 6			// Width of each of the two lines that delimit the board
#define BLOCK_SIZE 1				// Width and Height of each block of a piece

#define BOARD_WIDTH 18				// Board width in blocks
#define BOARD_HEIGHT 16				// Board height in blocks

#define PIECE_BLOCKS 5				// Number of horizontal and vertical blocks of a matrix piece


// --------------------------------------------------------------------------------
//									 Board
// --------------------------------------------------------------------------------

class Board
{
public:

	Board						(Pieces *pPieces, int pScreenHeight);

	int GetXPosInPixels			(int pPos);
	int GetYPosInPixels			(int pPos);
	bool IsFreeBlock			(int pX, int pY);
	bool IsPossibleMovement		(int pX, int pY, int pPiece, int pRotation);
	void StorePiece				(int pX, int pY, int pPiece, int pRotation);
	void DeletePossibleLines	();
	bool IsGameOver				();

private:

	enum { POS_FREE, POS_FILLED };			// POS_FREE = free position of the board; POS_FILLED = filled position of the board
	int mBoard [BOARD_WIDTH][BOARD_HEIGHT];	// Board that contains the pieces
	Pieces *mPieces;
	int mScreenHeight;

	void InitBoard();
	void DeleteLine (int pY);
};

#endif // _BOARD_
