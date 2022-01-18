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

typedef vector<pair<int, int>> undo_list;

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
    for (int r1 = 0; r1 < 6; r1++)
	for (int c1 = 0; c1 < 10; c1++)
	    assert(grid[r1][c1] != s);
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

bool placed[13];

bool next_pos(int &r, int &c)
{
    for (r = 0; r < 6; r++)
	for (c = 0; c < 10; c++)
	    if (grid[r][c] == 0)
		return true;
    return false;
}

void try_orients();

void try_orient(int r, int c, int s, dir orient)
{
    bool succ;
    succ = place(r, c, s, orient);
    if (succ) {
	show();
	placed[s] = true;
	try_orients();
	placed[s] = false;
    }

    unplace(r, c, s, orient);
    for (int r1 = 0; r1 < 6; r1++)
	for (int c1 = 0; c1 < 10; c1++)
	    assert(grid[r1][c1] != s);
}

void try_orients()
{
    int r, c, s;

    if (!next_pos(r, c)) {
	show();
	return;
    }

    for (s = 1; s <= 12; s++)
	if (!placed[s]) {
	    try_orient(r, c, s, F);
	    try_orient(r, c, s, B);
	    try_orient(r, c, s, L);
	    try_orient(r, c, s, R);
	}
}

int main()
{
    memset(grid, 0, sizeof (grid));
    for (int i = 0; i < 13; i++)
	placed[i] = false;
    try_orients();
    return 0;
}
