#include "minesweeper.h";
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <new>
#include <time.h>
#include <assert.h>
using namespace std;

square* squares;
int hasBomb = 0;
//0 is lose, 1 is keep going, 2 is you've won.
int winstate = 1;

void square::setstatus(char argument)
{
    status = argument;
    //cover = status;
}

char square::checkstatus()
{
    return status;
}

grid::grid(int wd, int ht, int numBombs)
{
    gridwd = wd;
    gridht = ht;
    bombCounter = numBombs;
    //beware: if too much memory used, will not work.
    squares = new square[wd*ht];
    for (int i = 0; i < ht; i++) {
        for (int j = 0; j < wd; j++) {
            squares[i*wd + j].x = i;
            squares[i*wd + j].y = j;
            /*
            if ((i*wd + j) % 3 == 0) {
                squares[i*wd + j].setstatus('Q');
            }
            else if ((i*wd + j) % 3 == 1) {
                squares[i*wd + j].setstatus(' ');
            }
            else {
                squares[i*wd + j].setstatus(' ');
            }
            */

            //printf("squares[%d] is located at %d,%d\n", i*wd + j, i, j);
        }
    }
    srand(time(NULL));
    while (numBombs > 0) {
        int randx = rand() % wd;
        int randy = rand() % ht;
        //printf("randx and randy are %d and %d\n", randx, randy);
        if (squares[randx*wd + randy].checkstatus() == '0') {
            squares[randx*wd + randy].setstatus('Q');
            numBombs--;
            //printf("numBombs is now %d\n", numBombs);
        }
    }
    for (int i = 0; i < ht; i++) {
        for (int j = 0; j < wd; j++) {
            if (squares[i*wd + j].checkstatus() != 'Q'){
                int numQs = 48;
                //cout << char(numQs) << "\n";
                for (int m = i-1; m <= i+1; m++) {
                        //j++? AAAGH!
                    for (int n = j-1; n <= j+1; n++) {
                        //cout << m << n << "\n";
                        if (m < 0 || n < 0 || m >= gridht || n >= gridwd) {;}
                        else if (squares[m*wd + n].checkstatus() == 'Q') {
                            numQs++;
                        }
                    }
                }
                squares[i*wd + j].setstatus((char)numQs);
                if (squares[i*wd + j].checkstatus() == '0') {
                    squares[i*wd + j].setstatus(' ');
                }
            }
        }
    }
}

grid::~grid()
{
    //need brackets for arrays
    delete[] squares;
}

void grid::displaygrid()
{
    int revealed = 0;
    for (int i = 0; i < gridht; i++) {
        for (int j = 0; j < gridwd; j++) {
            cout << squares[i*gridwd + j].cover;
            if (squares[i*gridwd +j].cover == 'Q') {
                winstate = 0;
                revealed++;
            }
            else if (squares[i*gridwd + j].cover != 'x') {
                revealed++;
            }
        }
        cout << " " << i << "\n";
    }
    cout << "\n";
    for (int k = 0; k < gridwd; k++) {
        int tens = (k - (k%10)) /10;
        if ((k - (k%10)) /10 > 0)
            cout << (k - (k%10)) /10;
        else cout << " ";
    }
    cout << "\n";
    for (int k = 0; k < gridwd; k++)
        cout << k%10;
    cout << "\n\n";
    if ((gridwd * gridht) - revealed == bombCounter) {
        winstate = 2;
    }
    //printf("revealed is %d\n", revealed);
}

int firstloop = 1;
int hasEmpty = 1;
void grid::reveal(int squarex, int squarey)
{
    if (squares[squarex*gridwd + squarey].checkstatus() == 'Q' && firstloop) {
        squares[squarex*gridwd + squarey].cover = 'Q';
        return;
    }
    else if (squares[squarex*gridwd + squarey].checkstatus() == 'Q') {;}
    else if (squares[squarex*gridwd + squarey].checkstatus() != ' ') {
        squares[squarex*gridwd + squarey].cover = squares[squarex*gridwd + squarey].checkstatus();
    }
    else {
        //keep looping until all empty squares are gone. (Recursion)
        //make sure not out of bounds when running.
        while (hasEmpty) {
            hasEmpty = 0;
            for (int i = squarex - 1; i <= squarex + 1; i++) {
                if (i < 0 || i >= gridht) {;}
                else {
                    for (int j = squarey -1; j <= squarey + 1; j++) {
                        //assert(firstloop);
                        if (i == squarex && j == squarey) {;}
                        else if (j < 0 || j >= gridwd) {;}
                        else {
                            squares[i*gridwd + j].cover = squares[i*gridwd + j].checkstatus();
                            if (squares[i*gridwd + j].cover == 'Q') {
                                squares[i*gridwd + j].cover = 'x';
                            }
                            //To avoid nasty forever loops, check if the square has already been uncovered.
                            else if (squares[i*gridwd + j].cover == ' ' && squares[i*gridwd + j].uncovered == 0) {
                                hasEmpty = 1;
                                firstloop = 0;
                                squares[i*gridwd + j].uncovered = 1;
                                //printf("Entering recursion at %d,%d\n", i, j);
                                reveal(i,j);
                                hasEmpty = 0;
                            }
                            squares[i*gridwd + j].uncovered = 1;
                        }
                    }
                }
            }
            firstloop = 1;
            return;
            //when I put return here it doesn't work. Should check this out. (Bug)
            //See above for fix.
        }
    }
    return;
}

int main()
{
    int xbound, ybound, mines, xinput, yinput;
    cout << "Welcome to ASCII Minesweeper! (copyright Nicholas Lai)\n";
    cout << "To begin, please enter what size grid you want as x followed by y.\n";
    cin >> xbound >> ybound;
    cout << "Now enter the number of mines you want.\n";
    cin >> mines;
    if (mines > xbound * ybound) {
        cout << "Too many mines. Lowered to random value.\n";
        mines = rand() % (xbound * ybound);
    }
    cout << "Now, every time a new grid pops up, enter a coordinate to test\n(within your grid)! \nIt's that simple!\n";
    cout << "*note: all coordinates must be entered in this fashion: x,y and no spaces.\n";
    grid griddy(xbound,ybound,mines);
    griddy.displaygrid();
    cout << "\n";
    while (winstate) {
        if (winstate == 2) {
            cout << "Congratulations! You win!\n";
            return 0;
        }
        scanf("%d,%d", &xinput, &yinput);
        //cout << "xinput and yinput are" << xinput << yinput <<"\n";
        //When I get bored, I should fix this function so that x input goes first.  If and when.
        griddy.reveal(yinput, xinput);
        griddy.displaygrid();
    }
    cout << "Sorry! Try again next time!\n";
    return 0;
}
