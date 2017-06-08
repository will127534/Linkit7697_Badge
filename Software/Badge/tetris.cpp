#include <Arduino.h>
#include "Tetris_Game.h"
#include "Control.hpp"

bool tetris()
{

    IO mIO;
    static const int SCREEN_HEIGHT = 16; // we only got 16 LED as height...

    // Pieces
    Pieces mPieces;

    // Board
    Board mBoard(&mPieces, SCREEN_HEIGHT);

    // Game
    Game mGame(&mBoard, &mPieces, &mIO, SCREEN_HEIGHT);

    // Get the actual clock in milliseconds
    static unsigned long mTime1 = 0;
    Serial.println("setup()");
    mTime1 = millis();
    bool pulse = 0;
    uint8_t pulse_cnt = 0;
    uint32_t last_pulse = millis();
    mIO.Begin();
    mIO.ClearScreen(); // Clear screen
    Serial.println("Game Begin");
    // Drawing Logic
    while (1)
    {
        // ----- Draw -----
        if (pulse == 0)
        {
            mGame.DrawScene();  // Draw staff
            mIO.UpdateScreen(); // Put the graphic context in the screen
        }
        else
        {
            if ((millis() - mTime1) > 1000 && pulse)
            {
                if (pulse_cnt % 2)
                {
                    mGame.DrawScene();
                    matrix.setTextColor(100 * analogRead(A0) / 512.0);
                    matrix.setCursor(7, 4);
                    matrix.print("P");
                    mIO.UpdateScreen();
                }
                else
                {
                    mGame.DrawScene();
                    mIO.UpdateScreen();
                }
                pulse_cnt++;
                mTime1 = millis();
            }
        }

        // ----- Input -----
        // USR button leaves the game
        if (digitalRead(USR_BUTTON))
        {
            return 0;
        }
        const int mKey = mIO.Pollkey();
        
        if (mKey == CENTER_BUTTON)
        {
            Serial.println("PULSE");
            if ((millis() - last_pulse) > 1000)
            {
                pulse = !pulse;
                last_pulse = millis();
                Serial.println("PULSE");
                mGame.DrawScene();
                matrix.setTextColor(100 * analogRead(A0) / 512.0);
                matrix.setCursor(7, 4);
                matrix.print("P");
                mIO.UpdateScreen();
            }
        }

        if (!pulse)
        {
            switch (mKey)
            {
            case RIGHT_BUTTON:
            {
                if (mBoard.IsPossibleMovement(mGame.mPosX + 1, mGame.mPosY, mGame.mPiece, mGame.mRotation))
                    mGame.mPosX++;
                break;
            }

            case LEFT_BUTTON:
            {
                if (mBoard.IsPossibleMovement(mGame.mPosX - 1, mGame.mPosY, mGame.mPiece, mGame.mRotation))
                    mGame.mPosX--;
                break;
            }

            case DOWN_BUTTON:
            {
                if (mBoard.IsPossibleMovement(mGame.mPosX, mGame.mPosY + 1, mGame.mPiece, mGame.mRotation))
                    mGame.mPosY++;
                break;
            }
            case X_BUTTON:
            {
                // Check collision from up to down
                while (mBoard.IsPossibleMovement(mGame.mPosX, mGame.mPosY, mGame.mPiece, mGame.mRotation))
                {
                    mGame.mPosY++;
                }

                mBoard.StorePiece(mGame.mPosX, mGame.mPosY - 1, mGame.mPiece, mGame.mRotation);

                mBoard.DeletePossibleLines();

                if (mBoard.IsGameOver())
                {
                    matrix.setFrame(7);
                    matrix.displayFrame(7);
                    matrix.clear();
                    Serial.println("Game Over");
                    matrix.setTextWrap(false);
                    for (int16_t x = 18; x >= -8 * 10; x--)
                    {
                        matrix.clear();
                        matrix.setCursor(x, 0);
                        matrix.setTextColor(100 * analogRead(A0) / 512.0);
                        matrix.print("Gameover");
                        delay(25);
                    }
                    return 1;
                }

                mGame.CreateNewPiece();
                delay(100);

                break;
            }

            case Y_BUTTON:
            {
                if (mBoard.IsPossibleMovement(mGame.mPosX, mGame.mPosY, mGame.mPiece, (mGame.mRotation + 1) % 4))
                    mGame.mRotation = (mGame.mRotation + 1) % 4;

                break;
            }
            }
        }
        // ----- Vertical movement -----

        unsigned long mTime2 = millis();

        if ((mTime2 - mTime1) > WAIT_TIME && pulse == 0)
        {
            if (mBoard.IsPossibleMovement(mGame.mPosX, mGame.mPosY + 1, mGame.mPiece, mGame.mRotation))
            {
                mGame.mPosY++;
            }
            else
            {
                mBoard.StorePiece(mGame.mPosX, mGame.mPosY, mGame.mPiece, mGame.mRotation);

                mBoard.DeletePossibleLines();

                if (mBoard.IsGameOver())
                {
                    matrix.setFrame(7);
                    matrix.displayFrame(7);
                    matrix.setTextColor(100 * analogRead(A0) / 512.0);
                    matrix.setTextWrap(false);
                    Serial.println("Game Over");
                    for (int16_t x = 18; x >= -8 * 10; x--)
                    {
                        matrix.clear();
                        matrix.setCursor(x, 0);
                        matrix.print("Gameover");
                        delay(25);
                    }
                    return 1;
                }

                mGame.CreateNewPiece();
            }

            mTime1 = millis();
        }
        delay(100);
    }
}
