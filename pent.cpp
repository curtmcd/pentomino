#include <cassert>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <memory.h>

using namespace std;

enum dir { F = 0, B = 1, L = 2, R = 3 };

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
    map<int, int> counts;

    for (int r = 0; r < 6; r++)
	for (int c = 0; c < 10; c++)
	    if (grid[r][c] == 0) {
		int h = 0;
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
    for (auto it : counts)
	if (it.second < m)
	    m = it.second;

    return m;
}

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
    // Optimize by denying placement if it would result in the grid
    // having any hole of empty cells less than 5 in size that would
    // become dead space.
    if (smallest_hole_size() < 5)
	return false;
    return true;
}

void try_shape(int s)
{
    for (int r = 0; r < 6; r++)
	for (int c = 0; c < 10; c++)
	    if (grid[r][c] == 0)
		for (dir o = F; o <= R; o = dir(o + 1)) {
		    if (place(r, c, s, o, 0, s)) {
			if (s == 12)
			    show();
			else
			    try_shape(s + 1);
		    }
		    place(r, c, s, o, s, 0);  // undo placement
		}
}

int main()
{
    memset(grid, 0, sizeof (grid));
    try_shape(1);
    return 0;
}
