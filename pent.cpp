#include <cassert>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <memory.h>

using namespace std;

enum dir { U, D, L, R };	// Up, down, left, right

#define H 6
#define W 10

vector<vector<dir>> shapes[] = {
    {{ R, R, R, R },		// I
     { D, D, D, D }},
    {{ R, R, R, D },		// L
     { D, D, D, L },
     { D, R, R, R },
     { R, L, D, D, D },
     { D, U, R, R, R },		// L flipped
     { D, D, D, R },
     { D, L, L, L },
     { D, D, D, R }},
    {{ R, R, D, R },		// N
     { D, D, L, D },
     { R, D, R, R },
     { D, L, D, D },
     { R, R, L, L, D, L },	// N flipped
     { D, R, D, D },
     { R, L, D, L, L },
     { D, D, R, D }},
    {{ R, D, R, D },		// W
     { D, L, D, L },
     { D, R, D, R },
     { R, L, D, L, D }},
    {{ R, R, D, D },		// V
     { D, D, L, L },
     { D, D, R, R },
     { R, R, L, L, D, D }},
    {{ D, R, R, D },		// Z
     { L, D, D, L },
     { D, L, L, D },		// Z flipped
     { R, D, D, R }},
    {{ D, U, R, R, D },		// U
     { R, L, D, D, R },
     { D, R, R, U },
     { R, D, D, L }},
    {{ D, R, U, R },		// P
     { R, L, D, R, D },
     { R, D, L, L },
     { D, R, D, L },
     { R, R, D, L },		// P flipped
     { D, L, D, R },
     { R, L, D, R, R },
     { R, D, L, D }},
    {{ R, R, R, L, D },		// Y
     { D, D, L, R, D },
     { D, L, R, R, R },
     { D, R, L, D, D },
     { R, D, U, R, R },		// Y flipped
     { D, L, R, D, D },
     { D, R, L, L, L },
     { D, D, R, L, D }},
    {{ D, L, R, R, L, D }},	// X
    {{ D, R, R, L, D },		// R
     { R, L, D, L, R, D },
     { D, L, R, R, D },
     { D, R, L, D, L },
     { D, L, L, R, D },		// R flipped
     { D, L, R, D, R },
     { D, R, L, L, D },
     { R, D, R, L, D }},
    {{ R, R, L, D, D },		// T
     { D, L, L, R, R, D },
     { D, D, L, R, R },
     { D, R, R, L, L, D }}
};

int grid[H][W];

void show()
{
    for (int r = 0; r < H; r++) {
	for (int c = 0; c < W; c++)
	    cout << setw(3) << grid[r][c] << ' ';
	cout << endl;
    }
    cout << endl;
}

bool place(int r, int c, int s, int o, int g_old, int g_new)
{
    vector<dir> &moves = shapes[s - 1][o];
    assert(grid[r][c] == g_old);
    grid[r][c] = g_new;
    for (auto m : moves) {
	switch (m) {
	case U:
	    if (--r < 0)
		return false;
	    break;
	case D:
	    if (++r >= H)
		return false;
	    break;
	case L:
	    if (--c < 0)
		return false;
	    break;
	case R:
	    if (++c >= W)
		return false;
	    break;
	default:
	    assert(false);
	}
	if (grid[r][c] != g_old && grid[r][c] != g_new)
	    return false;
	grid[r][c] = g_new;
    }
    return true;
}

bool placed[13];

bool next_pos(int &r, int &c)
{
    if (++c == W) {
	c = 0;
	if (++r == H)
	    return false;
    }
    return true;
}

void try_pos(int r, int c)
{
    // Find next hole on grid
    while (grid[r][c] != 0)
	if (!next_pos(r, c)) {
	    show();	// Got to end, must have solution
	    return;
	}

    for (int s = 1; s <= 12; s++) {
	if (!placed[s])
	    for (int o = 0; o < shapes[s - 1].size(); o++) {
		if (place(r, c, s, o, 0, s)) {
		    placed[s] = true;
		    try_pos(r, c);
		}
		place(r, c, s, o, s, 0);
		// undo full or partial placement
		placed[s] = false;
	    }
    }
}

int main()
{
    for (bool &p : placed)
	p = false;
    memset(grid, 0, sizeof (grid));
    try_pos(0, 0);
    return 0;
}
