// 		
// The following code is a derivative work of the code from OneLoneCoder.com - Command Line Tetris, 		
// which is licensed GPLv3. This code therefore is also licensed under the terms 		
// of the GNU Public License, verison 3.
//
#include <iostream>
#include <ncurses.h>
#include <thread>
#include <vector>
#include <cstdio>

std::string tetromino[7];
int nFieldWidth = 12;
int nFieldHeight = 18;
unsigned char *pField = nullptr;
unsigned char *screen = nullptr;

int nScreenWidth  = 80;
int nScreenHeight = 30;

WINDOW *win;

int Rotate(int px, int py, int r)
{
 switch (r % 4)
 {
    case 0: return py * 4 + px;         //   0 degrees
    case 1: return 12 + py - (px * 4);  //  90 degrees
    case 2: return 15 - (py * 4) - px;  // 180 degrees
    case 3: return 3 - py + (px * 4);   // 270 degrees 
 }   
 return 0;
}

bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY) 
{
    for (int px = 0; px < 4; px++)
        for (int py = 0; py <4; py++ )
        {
            // get index into piece
            int pi = Rotate(px, py, nRotation);

            // get index into field
            int fi = (nPosY + py) * nFieldWidth + (nPosX + px);
            if (nPosX + px >= 0 && nPosX + px < nFieldWidth)
            {
                if (nPosY + py >= 0 && nPosY + py < nFieldHeight)
                {
                    if (tetromino[nTetromino][pi] == 'X' && pField[fi] != 0)
                        return false;
                }
            }
        }

    return true;
}       


WINDOW* InitBoard(){
    WINDOW *w;
    w = newwin(nScreenHeight, nScreenWidth, 0, 0);
    if (w == NULL) {
        addstr("unable to create window");
        refresh();
        return w;
    } 
    // box(w,'|','=');
    noecho();
    return w;
}

void DisplayFrame(){
    for (int y = 1; y < nScreenHeight; y++)
            for (int x = 1; x < nScreenWidth; x++)
                mvwaddch(win,y,x,screen[y * nScreenWidth + x]);
    wrefresh(win);
}

int main() {

    initscr();

    // Create assets
    tetromino[0].append("..X.");
    tetromino[0].append("..X.");
    tetromino[0].append("..X.");
    tetromino[0].append("..X.");

    tetromino[1].append("..X.");
    tetromino[1].append(".XX.");
    tetromino[1].append(".X..");
    tetromino[1].append("....");

    tetromino[2].append(".X..");
    tetromino[2].append(".XX.");
    tetromino[2].append("..X.");
    tetromino[2].append("....");
    
    tetromino[3].append("....");
    tetromino[3].append(".XX.");
    tetromino[3].append(".XX.");
    tetromino[3].append("....");

    tetromino[4].append("..X.");
    tetromino[4].append(".XX.");
    tetromino[4].append("..X.");
    tetromino[4].append("....");

    tetromino[5].append("....");
    tetromino[5].append(".XX.");
    tetromino[5].append("..X.");
    tetromino[5].append("..X.");

    tetromino[6].append("....");
    tetromino[6].append(".XX.");
    tetromino[6].append(".X..");
    tetromino[6].append(".X..");

    screen = new unsigned char [nScreenWidth * nScreenHeight];
    for (int i = 0; i < nScreenWidth * nScreenHeight; i++)
        screen[i] = ' ';

    pField = new unsigned char [nFieldWidth * nFieldHeight]; // Create board
    for (int x = 0; x < nFieldWidth; x++) // board boundary
        for (int y = 0; y < nFieldHeight; y++) 
            pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;



    // game logic stuff
    bool bGameOver = false;
    int ch;
    int nCurrentPiece = 0;
    int nCurrentRotation = 0;
    int nCurrentX = nFieldWidth / 2;
    int nCurrentY = 0;
    int nSpeed = 20;
    int nSpeedCounter = 0;
    bool bForceDown = false;
    int nPieceCount = 0;
    int nScore = 0;
    std::vector<int> vLines;


    win = InitBoard();
    

    keypad(win, TRUE);
    while ( !bGameOver)
    {
        // Game timing =====================================================
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        nSpeedCounter++;
        bForceDown = (nSpeedCounter == nSpeed);

        // INPUT ==========================================================
        nodelay(win,TRUE);
        ch = wgetch(win);

        // Game logic =====================================================
        switch(ch){
            case KEY_DOWN:
                if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
                    nCurrentY++;
                
                break;
            case KEY_LEFT:
                if ( DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY))
                    nCurrentX--;
                break;
            case KEY_RIGHT:
                if ( DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY))
                    nCurrentX++;
                break;
            case '\n':
                if ( DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY))
                    nCurrentRotation++;
                    break;
            case 'q':
                bGameOver = true;
                break;
        }

        if (bForceDown) 
        {
            nSpeedCounter = 0;

            if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
                    nCurrentY++;
                else {
                    // Lock the current piece in the field
                    for (int px = 0; px < 4; px++)
                        for (int py = 0; py < 4; py++ )
                            if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] != '.')
                                pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;

                    nPieceCount++;
                    if (nPieceCount % 10 == 0)
                        if (nSpeed >= 10)   
                            nSpeed--;

                    // Check we have got any lines
                    for (int py = 0; py < 4; py++)
                        if (nCurrentY + py < nFieldHeight - 1)
                        {
                            bool bLine = true;
                            for (int px = 1; px < nFieldWidth - 1; px++)
                                bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;

                            if (bLine)
                            {
                                // Remove line set to =
                                for (int px = 1; px < nFieldWidth - 1; px++)
                                    pField[(nCurrentY + py) * nFieldWidth + px] = 8;
                                    vLines.push_back(nCurrentY + py);
                            }
                        }

                    nScore+=25;
                    if (!vLines.empty())
                        nScore+=(1 << vLines.size()) * 100;

                    // Choose next piece
                    nCurrentX = nFieldWidth / 2;
                    nCurrentY = 0;
                    nCurrentRotation = 0;
                    nCurrentPiece = rand() % 7;

                    // if piece do  not fit
                    bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
                }
        }

        // Draw field ===============================================
        for (int y = 0; y < nFieldHeight; y++)
            for (int x = 0; x < nFieldWidth; x++)
                screen[(y + 2) * nScreenWidth + (x + 2)] = " ABCDEFG=#"[pField[y * nFieldWidth + x]];

        // Draw current piece
        for (int px = 0; px < 4; px++)
            for (int py = 0; py < 4; py++ )
                if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == 'X')
                    screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;

        // Draw score
        
        sprintf((char *)screen+(2 * nScreenWidth + nFieldWidth + 6), "SCORE: %d", nScore);

        if (!vLines.empty())
        {
            DisplayFrame();
            std::this_thread::sleep_for(std::chrono::milliseconds(400));

            for (auto &v : vLines)
                for (int px = 1; px < nFieldWidth - 1; px++)
                {
                    for (int py = v; py > 0; py--)
                        pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
                    pField[px] = 0;
                }
            vLines.clear();
        }

        // Display frame
        DisplayFrame();
    }
    
    mvwprintw(win,1,18,"*** GAME OVER ***");
    mvwprintw(win,2,18,"SCORE: %d", nScore);
    nodelay(win,FALSE);
    ch = wgetch(win);


    endwin();
    return 0;
};