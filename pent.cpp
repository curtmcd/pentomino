#include <cassert>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <memory.h>

using namespace std;

enum dir { F, B, L, R };

vector<dir> reorient[] = {
    { F, B, L, R },
    { B, F, R, L },
    { L, R, B, F },
    { R, L, F, B },
};

vector<dir> shapes[] = {
    { F, F, F, F },
    { F, F, F, R },
    { F, F, R, F },
    { F, R, F, R },
    { F, F, R, R },
    { R, F, F, R },
    { R, F, F, L },
    { R, F, R, B } ,
    { F, F, F, B, R },
    { F, L, R, F, B, R },
    { R, F, F, B, R },
    { R, R, L, F, F },
};

int grid[6][10];

void show()
{
    for (int r = 0; r < 6; r++) {
	for (int c = 0; c < 10; c++)
	    cout << setw(3) << grid[r][c] << ' ';
	cout << endl;
    }
    cout << endl;
}

// Laboriously determine the smallest "hole" on the grid, where a
// hole is an orthogonally-connected region of unoccupied positions.
int smallest_hole_size()
{
    int hole[6][10];
    memset(hole, 0, sizeof (hole));
    int next = 0;
    int h;
    map<int, int> counts;

    memset(grid, 0, sizeof (grid));
    grid[0][1] = grid[0][2] = grid[0][3] = grid[0][8] = 99;
    grid[1][0] = grid[1][1] = grid[1][3] = grid[1][4] = grid[1][8] = grid[1][9] = 99;
    grid[2][1] = grid[2][2] = grid[2][6] = grid[1][6] = 99;
    grid[3][8] = grid[3][9] = 99;
    grid[4][4] = grid[4][5] = grid[4][6] = grid[4][7] = grid[4][8] = grid[4][9] = 99;
    grid[5][0] = grid[5][1] = grid[5][9] = 99;
    for (int r = 0; r < 6; r++)
	for (int c = 0; c < 10; c++)
	    grid[r][c] = 99 - grid[r][c];

    show();

    for (int r = 0; r < 6; r++)
	for (int c = 0; c < 10; c++)
	    if (grid[r][c] == 0) {
		h = 0;
		if (c > 0 && hole[r][c - 1] > 0)
		    h = hole[r][c - 1];
		else if (r > 0) {
		    for (int c2 = c; c2 < 10 && grid[r][c2] == 0; c2++)
			if (hole[r - 1][c2] > 0)
			    h = hole[r - 1][c2];
		}
		if (h == 0)
		    h = ++next;
		hole[r][c] = h;
		auto it = counts.find(h);
		if (it != counts.end())
		    it->second++;
		else
		   counts.insert(pair<int, int>(h, 1));
	    }

    int m = 60;
    for (auto i : counts)
	if (i.second < m)
	    m = i.second;
    memcpy(grid, hole, sizeof (grid));
    show();
    cout << m << endl;
    return m;
}


    /*
+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
|xxxxx|     |     |     |xxxxx|xxxxx|xxxxx|xxxxx|     |xxxxx|
|xxxxx|  1  |  1  |  1  |xxxxx|xxxxx|xxxxx|xxxxx|  2  |xxxxx|
|xxxxx|     |     |     |xxxxx|xxxxx|xxxxx|xxxxx|     |xxxxx|
+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
|     |     |xxxxx|     |     |xxxxx|xxxxx|xxxxx|     |     |
| ->  |  ^  |xxxxx|  1  |  1  |xxxxx|xxxxx|xxxxx|  2  |  2  |
|     |     |xxxxx|     |     |xxxxx|xxxxx|xxxxx|     |     |
+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
|xxxxx|     |     |xxxxx|xxxxx|xxxxx|     |xxxxx|xxxxx|xxxxx|
|xxxxx|  1  |  1  |xxxxx|xxxxx|xxxxx|  3  |xxxxx|xxxxx|xxxxx|
|xxxxx|     |     |xxxxx|xxxxx|xxxxx|     |xxxxx|xxxxx|xxxxx|
+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
|xxxxx|xxxxx|xxxxx|xxxxx|xxxxx|xxxxx|xxxxx|xxxxx|     |     |
|xxxxx|xxxxx|xxxxx|xxxxx|xxxxx|xxxxx|xxxxx|xxxxx|  4  |  4  |
|xxxxx|xxxxx|xxxxx|xxxxx|xxxxx|xxxxx|xxxxx|xxxxx|     |     |
+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
|xxxxx|xxxxx|xxxxx|xxxxx|     |     |     |     |     |     |
|xxxxx|xxxxx|xxxxx|xxxxx| ->  | ->  | ->  | ->  |  4  |  4  |
|xxxxx|xxxxx|xxxxx|xxxxx|     |     |     |     |     |     |
+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
|     |xxxxx|xxxxx|xxxxx|xxxxx|xxxxx|xxxxx|xxxxx|xxxxx|xxxxx|
|  5  |xxxxx|xxxxx|xxxxx|xxxxx|xxxxx|xxxxx|xxxxx|xxxxx|xxxxx|
|     |xxxxx|xxxxx|xxxxx|xxxxx|xxxxx|xxxxx|xxxxx|xxxxx|xxxxx|
+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/

bool place(int r, int c, int s, dir orient, int g_old, int g_new)
{
    vector<dir> &o = reorient[orient];
    assert(grid[r][c] == g_old);
    grid[r][c] = g_new;
    for (auto d : shapes[s - 1]) {
	switch (o[d]) {
	case F:
	    if (--r < 0)
		return false;
	    break;
	case B:
	    if (++r >= 6)
		return false;
	    break;
	case L:
	    if (--c < 0)
		return false;
	    break;
	case R:
	    if (++c >= 10)
		return false;
	    break;
	default:
	    assert(false);
	}
	if (grid[r][c] != g_old && grid[r][c] != g_new)
	    return false;
	grid[r][c] = g_new;
    }
    // To do: optimize by returning false if a placement results in
    // a grid with any pocket of less than 5 free cells.
    return true;
}

void try_shape(int s);

void try_orient(int r, int c, int s, dir orient)
{
    if (place(r, c, s, orient, 0, s)) {
	if (s == 12)
	    show();
	else
	    try_shape(s + 1);
    }
    place(r, c, s, orient, s, 0);	// undo placement (full or partial)
}

void try_shape(int s)
{
    for (int r = 0; r < 6; r++)
	for (int c = 0; c < 10; c++) {
	    if (grid[r][c] == 0) {
		try_orient(r, c, s, F);
		try_orient(r, c, s, B);
		try_orient(r, c, s, L);
		try_orient(r, c, s, R);
	    }
	}
}

int main()
{
    memset(grid, 0, sizeof (grid));
    //    try_shape(1);
    smallest_hole_size();
    return 0;
}
