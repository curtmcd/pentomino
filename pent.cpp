#include <cassert>
#include <iostream>
#include <iomanip>
#include <vector>
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

int pocket_size(int r, int c)
{
    if (grid[r][c] != 0)
	return 0;
    int ps = 1;
    if (r > 0) {
	ps += pocket_size(r - 1, c);
	if (ps >= 5)
	    return ps;
    }
    if (r < 5) {
	ps += pocket_size(r + 1, c);
	if (ps >= 5)
	    return ps;
    }
    if (c > 0) {
	ps += pocket_size(r, c - 1);
	if (ps >= 5)
	    return ps;
    }
    if (c < 9)
	ps += pocket_size(r, c + 1);
    return ps;
}

bool unsolvable()
{
    for (int r = 0; r < 6; r++)
	for (int c = 0; c < 10; c++)
	    if (grid[r][c] == 0) {
		int p = pocket_size(r, c);
		if (p < 5) {
		    cout << "Pocket size " << p << " at (" << r << ", " << c << ")" << endl;
		    return true;
		}
	    }
    return false;
}

void unplace(int r, int c, int s, dir orient)
{
    vector<dir> &o = reorient[orient];
    if (grid[r][c] != s)
	return;
    grid[r][c] = 0;
    for (auto &d : shapes[s - 1]) {
	switch (o[d]) {
	case F:
	    if (--r < 0)
		return;
	    break;
	case B:
	    if (++r >= 6)
		return;
	    break;
	case L:
	    if (--c < 0)
		return;
	    break;
	case R:
	    if (++c >= 10)
		return;
	    break;
	default:
	    assert(false);
	}
	if (grid[r][c] != 0 && grid[r][c] != s)
	    break;
	grid[r][c] = 0;
    }
}

bool place(int r, int c, int s, dir orient)
{
    vector<dir> &o = reorient[orient];
    if (grid[r][c] != 0)
	return false;
    grid[r][c] = s;
    for (auto &d : shapes[s - 1]) {
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
	if (grid[r][c] != 0 && grid[r][c] != s)
	    return false;
	grid[r][c] = s;
    }
    return true;
}

void try_shape(int s);

void try_orient(int r, int c, int s, dir orient)
{
    bool succ = place(r, c, s, orient);
    if (succ) {
	show();
	unsolvable();
	try_shape(s + 1);
    }
    unplace(r, c, s, orient);
}

void try_shape(int s)
{
    if (s == 13) {
	show();
	return;
    }

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
    try_shape(1);
    return 0;
}
