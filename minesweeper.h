#ifndef MINESWEEPER_H_INCLUDED
#define MINESWEEPER_H_INCLUDED

#include <iostream>
#include <stdio.h>
using namespace std;

/*
*The game will first create an x-by-y grid, with some number being mines and the rest being normal squares.
*When an empty square is hit, all adjacent empty squares will be revealed and adjacent numbered squares.
*When a numbered square is hit (algorithm for number), only that square will be revealed.
*When a bomb is hit, the game is over (end while loop continuing game.)
*You win game if all empty and numbered squares are uncovered. (there will be a running total).
*/

//Bugs to fix: bounds are wrong when playing: the xbound and ybounds are switched. (I think)
//Reveal also switches the coordinates. Perhaps related error?
//works best for square sizes because of the bug. (I think). There may be bugs in square sizes too b/c of dynamic memory.

class square {
    //statuses: 0 = empty, 1-9 = numbered, -1 = mine.
    char status = '0';
public:
    char cover = 'x';
    int uncovered = 0;
    int x;
    int y;
    void setstatus(char argument);
    char checkstatus();
};

class grid {
    int gridht, gridwd;
    int bombCounter;
public:
    grid(int wd, int ht, int numBombs);
    //when programming games, get into habit of using destructors
    ~grid();
    void displaygrid();
    void reveal(int squarex, int squarey);
};

#endif // MINESWEEPER_H_INCLUDED
