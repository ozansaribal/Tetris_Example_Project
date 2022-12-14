// Tetris.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include <thread>
#include <vector>


using namespace std;

wstring tetrisArray[7];

int nFieldWidth = 12;

int nFieldHeight = 18;

unsigned char  *pField = nullptr;

int nScreenWidth = 120;             // Console Screen Size X (columns)

int nScreenHeight = 54;			   // Console Screen Size Y (rows)

								   
								   
int Rotate(int px, int py, int r)
{
	// in this method, px represents position of x, py represents position of y and r represents how much degree is used for rotation, shortly represents the rotation

	switch (r % 4)
	{

	case 0: return py * 4 + px;			// 0 degrees

	case 1: return 12 + py - (px * 4);  // 90 degrees

	case 2: return 15 - (py * 4) - px;  // 180 degrees

	case 3: return 3 - py + (px * 4);   // 270 degrees

	}

	return 0;

}

// in this method, nTetrisArray represents the id of an array element, nRotation represents what the current rotation for that piece is, and

// nPosX and nPosY represent the location of the piece in the array

bool DoesPieceFit(int nTetrisArray, int nRotation, int nPosX, int nPosY)
{
	// here, we need to enter and iterate through tetris array to work with tetris game field properly

	for (int px = 0; px < 4; px++)
	{
		for (int py = 0; py < 4; py++)
		{

			// we get the index into piece using our rotation method

			// this is the index to the array of the tetris array

			int pi = Rotate(px, py, nRotation);

			// we also need to get index without rotation into field based on where the current playing position is

			int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

			// we also need to be careful that we're not going out of bounds when we're doing array checks

			// because we'll be accessing memory which is not ours 

			if (nPosX + px >= 0 && nPosX + px < nFieldWidth)
			{
				if (nPosY + py >= 0 && nPosY + py < nFieldHeight)
				{

					// the collision detection is a single one line

					// we take our tetris piece which is specified as an argument of the method

					// we've worked out our rotated index so we know that we check the right index 

					// if the tetris piece's index is equal to an X and the playing field is not equal to 0, then there must be a collision

					// so here it says that there's something already in that playing field so we return false

					// the piece cannot fit there. So, straight away fail.

					if (tetrisArray[nTetrisArray][pi] == L'X' && pField[fi] != 0)
					{
						// fail on first hit

						return false;
					}

				}
			}

		}
	}

	return true;

}

int main()
{

	// Creating assets

	tetrisArray[0].append(L"..X.");
	tetrisArray[0].append(L"..X.");
	tetrisArray[0].append(L"..X.");
	tetrisArray[0].append(L"..X.");


	tetrisArray[1].append(L"..X.");
	tetrisArray[1].append(L".XX.");
	tetrisArray[1].append(L".X..");
	tetrisArray[1].append(L"....");


	tetrisArray[2].append(L".X..");
	tetrisArray[2].append(L".XX.");
	tetrisArray[2].append(L"..X.");
	tetrisArray[2].append(L"....");


	tetrisArray[3].append(L"....");
	tetrisArray[3].append(L".XX.");
	tetrisArray[3].append(L".XX.");
	tetrisArray[3].append(L"....");


	tetrisArray[4].append(L"..X.");
	tetrisArray[4].append(L".XX.");
	tetrisArray[4].append(L"..X.");
	tetrisArray[4].append(L"....");


	tetrisArray[5].append(L"....");
	tetrisArray[5].append(L".XX.");
	tetrisArray[5].append(L"..X.");
	tetrisArray[5].append(L"..X.");


	tetrisArray[6].append(L"....");
	tetrisArray[6].append(L".XX.");
	tetrisArray[6].append(L".X..");
	tetrisArray[6].append(L".X..");

	pField = new unsigned char[nFieldWidth*nFieldHeight]; // Create play field buffer

	for (int x = 0; x < nFieldWidth; x++) {				  // Board Boundary
		for (int y = 0; y < nFieldHeight; y++) {

			// here, we determine the boundaries of tetris game field

			pField[y*nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;

		}
	}

	// here, we create a field with 80x30. We choose this size according to console size.

	wchar_t *screen = new wchar_t[nScreenWidth*nScreenHeight];

	// after creating a new field for tetris, then we fill the field with empty characters in for loop

	for (int i = 0; i < nScreenWidth*nScreenHeight; i++)
	{
		screen[i] = L' ';
	}

	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	SetConsoleActiveScreenBuffer(hConsole);

	DWORD dwBytesWritten = 0;

	// Game Logic Stuff

	bool bGameOver = false;

	// we need to know what current piece is falling

	int nCurrentPiece = 0;

	// we need to know whether the current piece is rotated or not and if it's rotated, then we need to know that it's rotated to what angle

	int nCurrentRotation = 0;

	// we need to know where the piece is located in the field

	int nCurrentX = nFieldWidth / 2;

	int nCurrentY = 0;

	// we store current keys. these are down arrow, left arrow, right arrow and z for rotation

	bool bKey[4];

	// we check whether the user is holding down the rotate button or not

	bool bRotateHold = true;

	// this speed variable actually determines the difficulty of the level.

	int nSpeed = 20;

	// here we count the game ticks like we did with the thread function in the GAME TIMING

	int nSpeedCounter = 0;

	// we also want to force the piece down

	bool bForceDown = false;

	// we want this little visual indication of lines to disappear at a later point in time.

	// we store the fact that some lines exist. So we use vector

	vector<int> vLines;

	// here, we need to keep the score

	int nScore = 0;

	// here, we need to keep track of how many pieces we've currently delivered to the player

	int nPieceCount = 0;

	while (!bGameOver) {

		// GAME TIMING ================================

		// game timing is important because every computer is not the same. Some are faster and some are slower. We provide a consistent game experience with game timing

		// game tick

		this_thread::sleep_for(50ms);

		// for every game tick, we increase the speed counter.

		nSpeedCounter++;

		// if our speed counter is equal to current speed, then we want to force the piece down. Otherwise we do not force the piece down.

		bForceDown = (nSpeedCounter == nSpeed);

		// INPUT =================================

		// we'll also handle with the user input. Because we sometimes want to change shapes

		for (int k = 0; k < 4; k++)
		{
			// we used GetAsyncKeyState here to get an input. GetAsyncKeyState method tells us whether that key is pressed or not. If it is pressed, it returns true.

			// So in the string, x27 represents R key for right, x25 represents L key for left, x28 represents D key for down and Z for rotation.

			// x27,x25 and x28 are virtual key codes in Hexadecimal format. Then, it checks which key codes are false and which key codes are true.

			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;

		}

		// GAME LOGIC =================================

		// here, we want to determine the shapes falling, the collision detection and the scoring.

		// we check whether the piece can go to left or there is no space to go to left

		nCurrentX += (bKey[0] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;

		// we check whether the piece can go to right or there is no space to go to right

		nCurrentX -= (bKey[1] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0;

		// we check whether the piece can go to down or there is no space to go to down

		nCurrentY += (bKey[2] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0;

		// we check whether the piece can rotate or there is no space to rotate

		if (bKey[3])
		{

			nCurrentRotation += (bRotateHold && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;

			// after rotation, we need to set the boolean variable as true to prevent the loop of rotation while pressing down the button

			// Because how many times we hold on the button, we want the rotation to be done once

			bRotateHold = false;

		}
		// if z button is not pressed, then rotation is not done
		else
		{

			bRotateHold = true;

		}

		// we've already handled with the player movement but now we need to handle with basically the game when it forces the piece down

		// tetris keeps forcing the piece down until it cannot no more. When the piece cannot go down anymore, then it's done, it's locked into place.

		// That's when we will take out the next piece

		if (bForceDown)
		{
			// So when the piece is forced down in this particular game tick, then the first thing we need to do is that we check whether the piece fits any further down or not.

			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
			{
				// if the piece can continue to go down, then we provide this movement here

				nCurrentY++;

			}
			// if the piece cannot go down, we got a whole sequence of things that we need to do
			else
			{
				// first of all, we need to lock the current piece into the field	

				// here we just take the tetris 2D array and that translated into the field in its current position and rotation so we go through it element by element.

				// we take that from tetris array if there is an X there then we update our field with the current value plus 1 

				// because 0 in this case is empty space but 0 also represents one of shapes in our tetris array.

				for (int px = 0; px < 4; px++)
				{

					for (int py = 0; py < 4; py++)
					{

						if (tetrisArray[nCurrentPiece][Rotate(px, py, nCurrentRotation)] != L'.')
						{

							pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;

						}

					}

				}

				// when we lock the piece in, we increase the count and if the account is a multiple of ten, we want to decrease our speed

				nPieceCount++;

				if(nPieceCount % 10 == 0)
				{

					if(nSpeed >= 10)
					{

						nSpeed--;

					}

				}

				// we're accumulating tetris array elements now and the current piece gets put basically into the background. 

				// It becomes part of the furniture. It's an obstacle for the player

				// Once we've locked the piece in we want to check whether we have created any full horizontal lines or not.

				// we do not need to check the whole plain. It's enough if we only check the last tetris element location.

				for(int py = 0; py < 4; py++)
				{
					
					// with minus 1, we put in a boundary check to make sure that we do not check stuff beyond the boundary of the array.

					if(nCurrentY + py < nFieldHeight - 1)
					{
						// we assume that there is a line and the way we want this to work is if there are any empty space in the line, this will be set as false 

						bool bLine = true;

						for (int px = 1; px < nFieldWidth - 1; px++)
						{
							// so it cannot be a line. We check everywhere in the playing field for one row except for the first and last cells

							// because they are going to be boundaries I do not want to include those in the check and if any of these is set as zero, then this becomes false

							// PLATONIC_COMMENT: you have yo check whole the row first, if there are no space in the whole row, then you activate other part.

							bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;

							// if we get to this point in the code and bLine is still true, then it must be a line

							// so we're going to set all of those elements to be number eight which represents the equal symbol when we draw it out into the screen

						}

						if (bLine) 
						{
							// Removing line

							for(int px = 1; px < nFieldWidth - 1; px++)
							{

								pField[(nCurrentY + py) * nFieldWidth + px] = 8;

							}

							// if the line exists, then we simply push back the current row 

							vLines.push_back(nCurrentY + py);

						}

					}

					// there are two approaches to scoring. First one is simply every piece we get 25 additional points 

					// but the other one to encourage the player to take risks. We want to score the lines.

					// if the player gets one line, we give them a hundred points but if they get four lines, we'll give them sixteen hundred points

					// so there's an exponential thrust in order to keep the player trying to go for bigger line removals in one go

					nScore += 25;

					if(!vLines.empty())
					{

						nScore += (1 << vLines.size()) * 100;

					}

				}

				// If we have got some lines, we'll probably want to do something with them ------ we'll come back to that

				// as the final step, we choose the next piece

				nCurrentX = nFieldWidth / 2;

				nCurrentY = 0;

				nCurrentRotation = 0;

				// we choose the next piece randomly and this is pseudo-random

				nCurrentPiece = rand() % 7;

				// but if we cannot fit the next piece in, then it's game over.

				bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);


			}

			// then we set the counter as 0 again because the movement to down is finished here whether the piece moved down or the piece is locked at the bottom.

			// So the whole process about moving down can start again with setting speed counter as 0 again

			nSpeedCounter = 0;

		}

		// RENDER OUTPUT ============================

		// Draw Field

		for (int x = 0; x < nFieldWidth; x++) {
			for (int y = 0; y < nFieldHeight; y++)
			{
				// here, we represently draw and fill fields starting with an empty character and continuing with A.

				// # symbol represents the border

				// we take whatever position is in our field array and that will give us a number. 

				// Then, we'll use that number to index into the string array to decide what we display on the screen 

				screen[(y + 2)*nScreenWidth + (x + 2)] = L" ABCDEFG=#"[pField[y*nFieldWidth + x]];

			}
		}

		// Draw Current Piece

		// To draw the current piece, we need to iterate through the tetris array

		for (int px = 0; px < 4; px++) {
			for (int py = 0; py < 4; py++) {

				// here, we first get the current piece index that is falling and then we want to get the index which is rotated

				// we go from 0 to 4 of x and y. We use the current rotation to give us the correct index

				// If that is equal to X, then we want to draw something to the screen otherwise draw an empty space with it's the part of the tetris array which we do not care

				if (tetrisArray[nCurrentPiece][Rotate(px, py, nCurrentRotation)] != L'.') {

					// we just draw this by using again our Y times width plus X total

					// and we offset it by the current position of the piece plus 2 because our playing field is also offset by 2

					// so that's the same Y and X but this time the screen width is not the field width. It's the width of entire console buffer

					// and we set that to rather oddly here the current piece plus 65. We add 65 because in ASCII, 65 means letter A. 

					// Using this respectively means that we begin with A and then proceed with B, C.

					screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;
				}
			}
		}

		// Draw Score

		// in this case, we'll display it slightly to the right we're just using the printf string function here

		swprintf_s(&screen[2 * nScreenWidth + nFieldWidth + 6], 16, L"SCORE: %8d", nScore);

		// we can use the vector empty function to say that have we got some lines and behave differently if we have

		if (!vLines.empty())
		{

			// Display Frame (cheekily to draw lines)

			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

			// we use a bit delay here

			this_thread::sleep_for(400ms);

			// then we want to remove the lines but then also move down all of the pieces above the lines and this is simply done by iterating through the vectors

			// if a line exists, we've got a number for the line which comes back. 

			for (auto &v : vLines)
				
				for (int px = 1; px < nFieldWidth - 1; px++)
				{
					// That gives us the row and then we want to iterate a column by column across the row simply moving the pieces down

					// In this case, we update the current field position again with Y times width plus X is gonna hammer this point home with the position above it

					for (int py = v; py > 0; py--) {

						pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
						
					}

					// and the top row is set with zeros empty space

					pField[px] = 0;

				}

				// then we finally clear lines out

				vLines.clear();
			
		}

		// Display Frame

		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

	}

	// First of all, we tidy up of our console because we cannot really use a see out whilst we've got hold of the screen buffer.

	CloseHandle(hConsole);

	// Also we'll display a game over and the score

	cout << "Game Over!! Score:" << nScore << endl;

	system("pause");

	return 0;

}



