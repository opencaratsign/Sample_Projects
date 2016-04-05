/*
ID: reginig1
PROG: castle
LANG: C++
*/

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <assert.h>
using namespace std;

ifstream fin("castlein.txt");
ofstream fout("castleout.txt");

/* I spent about three hours on this problem today, although technically I've been thinking about it for at least
*a week.
* There were so many tricky little bugs that I had to resolve before I could get an exactly correct answer.
* About an hour and a half on those tricky bugs.
* As regards developing an algorithm, I should remember not to become too dependent on what the USACO people teach
* (although their techniques have helped in the past.)
*At first I thought of flood-filling each room to obtain number of cells, then I got stuck as to how to
*find the best adjacent room combo to get a high sum.
*I was thinking of some really complicated things, like storing neighbors in each room, when it hit me:
*the memory was small enough that I could check each cell individually to test out max sums,
*and work on a case-by-case basis to obtain the most optimal wall to remove.
*Details on each section are given below.
*BTW it took me way too long to come up with an algorithm. Mainly because I was intimidated at working with a new
*technique (graph theory.)
*/

//this shall be the convention: 0 means south, 1 east, 2 north, 3 west (like an upside-down clock.)
//I never used neighborindex. It would have become messy.
struct cell {
    int cellx, celly;
    int haswall[4];
    int index = 0;
    int neighborindex[4];
};

//I expected more complexity from this, especially because of the graph theory lessons. This is rather simple.
struct room {
    int roomindex = 0;
    int numcells = 0;
};

cell cells[52][52];
room rooms[2500];

//The given exp function is ugly (uses floating point) so I always use my own substitute.
//If there's a better standard I'll use that.
int exp(int base, int power)
{
    if (power == 0) return 1;
    else {
        base *= exp(base, power - 1);
        return base;
    }
}

//don't forget to make reference variables if you want to actually change what's inside
//gives each cell walls based on the rules.
void assignwalls(cell &assigned)
{
    int id;
    fin >> id;
    for (int i = 3; i >= 0; i--) {
        if (id >= exp(2, i)) {
            assigned.haswall[3-i] = 1;
            id -= exp(2,i);
        }
        else {
            assigned.haswall[3-i] = 0;
        }
    }
    return;
}

//the flood-fill algorithm. A little lengthy.
int totalrooms;
void addtoroom(cell &added, int xbound, int ybound)
{
    //cout << "right now at " << added.cellx << " " << added.celly << "\n";
    if (added.index == 0) {
        rooms[totalrooms].numcells++;
        added.index = totalrooms + 1;
        //cout << "added.index is now " << added.index << "\n";
        if (added.haswall[0] == 0 && added.celly + 1 < ybound) {
            //cout << "entering loop1\n";
            addtoroom(cells[added.cellx][added.celly + 1], xbound, ybound);
        }
        if (added.haswall[1] == 0 && added.cellx + 1 < xbound) {
            //cout << "entering loop2\n";
            addtoroom(cells[added.cellx + 1][added.celly], xbound, ybound);
        }
        if (added.haswall[2] == 0 && added.celly > 0) {
            //cout << "entering loop3\n";
            addtoroom(cells[added.cellx][added.celly - 1], xbound, ybound);
        }
        if (added.haswall[3] == 0 && added.cellx > 0) {
            //cout << "entering loop4\n";
            addtoroom(cells[added.cellx - 1][added.celly], xbound, ybound);
        }
    }
    return;
}

//I eventually integrated this into the main function because I needed consecutive if statements
//rather than consecutive else if statements
//moral of the story: consecutive if statements are more versatile, although both have their place.
int diffrooms(cell input, int xbound, int ybound)
{
    if (input.celly + 1 < ybound && cells[input.cellx][input.celly + 1].index != input.index)
        return 1;
    if (input.cellx + 1 < xbound && cells[input.cellx + 1][input.celly].index != input.index)
        return 2;
    if (input.celly > 0 && cells[input.cellx][input.celly - 1].index != input.index)
        return 3;
    if (input.cellx > 0 && cells[input.cellx - 1][input.celly].index != input.index)
        return 4;
    return 0;
}

int main()
{
    int xbound, ybound;
    fin >> xbound >> ybound;
    //I forgot to give each cell coordinates, and spent about 20 min puzzling on that.
    for (int y = 0; y < ybound; y++) {
        for (int x = 0; x < xbound; x++) {
            cells[x][y].cellx = x;
            cells[x][y].celly = y;
            assignwalls(cells[x][y]);
        }
    }

    //I forgot to give each room an index, and remedied that here
    for (int y = 0; y < ybound; y++) {
        for (int x = 0; x < xbound; x++) {
            if (cells[x][y].index == 0) {
                //cout << "Empty room found at " << x << " " << y << "\n";
                addtoroom(cells[x][y], xbound, ybound);
                rooms[totalrooms].roomindex = totalrooms + 1;
                totalrooms++;
            }
        }
    }

    for (int i = 0; i < totalrooms; i++) {
        cout << rooms[i].roomindex << " " << rooms[i].numcells << "\n";
    }

    //this was really annoyingly nitpicky. I needed consecutive if statements, and I needed to split
    //the cases of an equal sum found vs. a higher sum found.
    //Since the code is so similar, I feel like I could have developed a function. Oh well.
    int maxsouth = 0;
    int mineast = 55;
    int maxcombinedroom = 0;
    int testsum;
    for (int y = 0; y < ybound; y++) {
        for (int x = 0; x < xbound; x++) {
            if (y + 1 < ybound && cells[x][y + 1].index != cells[x][y].index) {
                //cout << "the indices are " << cells[x][y].index << cells[x][y+1].index << "\n";
                testsum = rooms[cells[x][y].index-1].numcells + rooms[cells[x][y+1].index-1].numcells;
                if (testsum > maxcombinedroom) {
                    //cout << x << " " << y << " " << testsum << "\n";
                    maxcombinedroom = testsum;
                    mineast = x;
                    maxsouth = y;
                    cout << maxsouth << " " << mineast << " " << testsum << "\n";
                }
                else if (testsum == maxcombinedroom) {
                    if (x <= mineast) {
                        mineast = x;
                        if (y >= maxsouth) maxsouth = y;
                    }
                    cout << maxsouth << " " << mineast << " " << testsum << "\n";
                }
            }
            if (x + 1 < xbound && cells[x + 1][y].index != cells[x][y].index) {
                //cout << "the indices are " << cells[x][y].index << cells[x+1][y].index << "\n";
                testsum = rooms[cells[x][y].index-1].numcells + rooms[cells[x+1][y].index-1].numcells;
                if (testsum > maxcombinedroom) {
                    //cout << x << " " << y << " " << testsum << "\n";
                    maxcombinedroom = testsum;
                    mineast = x;
                    maxsouth = y;
                    cout << maxsouth << " " << mineast << " " << testsum << "\n";
                }
                else if (testsum == maxcombinedroom) {
                    if (x <= mineast) {
                        mineast = x;
                        if (y >= maxsouth) maxsouth = y;
                    }
                    cout << maxsouth << " " << mineast << " " << testsum << "\n";
                }
            }
            if (y > 0 && cells[x][y - 1].index != cells[x][y].index) {
                //cout << "the indices are " << cells[x][y].index << cells[x][y-1].index << "\n";
                testsum = rooms[cells[x][y].index-1].numcells + rooms[cells[x][y-1].index-1].numcells;
                if (testsum > maxcombinedroom) {
                    //cout << x << " " << y << " " << testsum << "\n";
                    maxcombinedroom = testsum;
                    mineast = x;
                    maxsouth = y;
                    cout << maxsouth << " " << mineast << " " << testsum << "\n";
                }
                else if (testsum == maxcombinedroom) {
                    if (x <= mineast) {
                        mineast = x;
                        if (y >= maxsouth) maxsouth = y;
                    }
                    cout << maxsouth << " " << mineast << " " << testsum << "\n";
                }
            }
            if (x > 0 && cells[x - 1][y].index != cells[x][y].index) {
                //cout << "the indices are " << cells[x][y].index << cells[x-1][y].index << "\n";
                testsum = rooms[cells[x][y].index-1].numcells + rooms[cells[x-1][y].index-1].numcells;
                if (testsum > maxcombinedroom) {
                    //cout << x << " " << y << " " << testsum << "\n";
                    maxcombinedroom = testsum;
                    mineast = x;
                    maxsouth = y;
                    cout << maxsouth << " " << mineast << " " << testsum << "\n";
                }
                else if (testsum == maxcombinedroom) {
                    if (x <= mineast) {
                        mineast = x;
                        if (y >= maxsouth) maxsouth = y;
                    }
                    cout << maxsouth << " " << mineast << " " << testsum << "\n";
                }
            }
        }
    }

    cout << diffrooms(cells[10][10], xbound, ybound) << "\n";

    fout << totalrooms << "\n";

    int highestnumcells = 0;

    for (int i = 0; i < totalrooms; i++)
        if (rooms[i].numcells > highestnumcells)
            highestnumcells = rooms[i].numcells;

    fout << highestnumcells << "\n" << maxcombinedroom << "\n";

    //x and y coordinates are flipped when printing. Annoying.
    //The origin is at 1,1. Also annoying.
    fout << maxsouth + 1 << " " << mineast + 1 << " ";

    //Really annoying. maxsouth = 0 gives runtime errors (bad array allocations) and
    //you actually need to check for a wall and the cells have to correspond to the correct rooms. Agh.
    if (maxsouth == 0) fout << "E\n";
    else if (rooms[cells[mineast][maxsouth].index-1].numcells +
        rooms[cells[mineast][maxsouth-1].index-1].numcells == maxcombinedroom &&
        cells[mineast][maxsouth].haswall[2] == 1) {
            fout << "N\n";
        }
    else fout << "E\n";

    for (int y = 0; y < ybound; y++) {
        for (int x = 0; x < xbound; x++) {
            /*
            for (int i = 0; i < 4; i++) {
                cout << cells[x][y].haswall[i];
            }
            */
            if (cells[x][y].index >= 10) cout << cells[x][y].index << " ";
            else cout << cells[x][y].index << "  ";
        }
        cout << "\n";
    }

    return 0;
}
