#include <cassert>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <memory.h>

using namespace std;

enum dir { U, D, L, R };	// Up, down, left, right

#define W 10
#define H 6

//#define W 12
//#define H 5

//#define W 15
//#define H 4

//#define W 20
//#define H 3

#define ALLOW_FLIPPED		1
//#define ALLOW_FLIPPED		0

vector<vector<dir>> shapes[] = {
    {
     { R, R, R, R },		// I
     { D, D, D, D }
    },
    {
     { R, R, R, D },		// L
     { D, D, D, L },
     { D, R, R, R },
     { R, L, D, D, D },
#if ALLOW_FLIPPED
     { D, U, R, R, R },		// L flipped
     { D, D, D, R },
     { D, L, L, L },
     { R, D, D, D },
#endif
    },
    {
     { R, R, D, R },		// N
     { D, D, L, D },
     { R, D, R, R },
     { D, L, D, D },
#if ALLOW_FLIPPED
     { R, R, L, L, D, L },	// N flipped
     { D, R, D, D },
     { R, L, D, L, L },
     { D, D, R, D },
#endif
    },
    {
     { R, D, R, D },		// W
     { D, L, D, L },
     { D, R, D, R },
     { R, L, D, L, D }
    },
    {
     { R, R, D, D },		// V
     { D, D, L, L },
     { D, D, R, R },
     { R, R, L, L, D, D }
    },
    {
     { D, R, R, D },		// Z
     { L, D, D, L },
#if ALLOW_FLIPPED
     { D, L, L, D },		// Z flipped
     { R, D, D, R },
#endif
    },
    {
     { D, U, R, R, D },		// U
     { R, L, D, D, R },
     { D, R, R, U },
     { R, D, D, L }
    },
    {
     { D, R, U, R },		// P
     { R, L, D, R, D },
     { R, D, L, L },
     { D, R, D, L },
#if ALLOW_FLIPPED
     { R, R, D, L },		// P flipped
     { D, L, D, R },
     { R, L, D, R, R },
     { R, D, L, D },
#endif
    },
    {
     { R, R, R, L, D },		// Y
     { D, D, L, R, D },
     { D, L, R, R, R },
     { D, R, L, D, D },
#if ALLOW_FLIPPED
     { R, D, U, R, R },		// Y flipped
     { D, L, R, D, D },
     { D, R, L, L, L },
     { D, D, R, L, D },
#endif
    },
    {
     { D, L, R, R, L, D }	// X
    },
    {
     { D, R, R, L, D },		// R
     { R, L, D, L, R, D },
     { D, L, R, R, D },
     { D, R, L, D, L },
#if ALLOW_FLIPPED
     { D, L, L, R, D },		// R flipped
     { D, L, R, D, R },
     { D, R, L, L, D },
     { R, D, R, L, D },
#endif
    },
    {
     { R, R, L, D, D },		// T
     { D, L, L, R, R, D },
     { D, D, L, R, R },
     { D, R, R, L, L, D }
    }
};

struct Grid {
    Grid() {
	memset(grid, 0, sizeof (grid));

	for (bool &p : placed)
	    p = false;
    }

    void show() {
	for (int r = 0; r < H; r++) {
	    for (int c = 0; c < W; c++)
		cout << setw(3) << grid[r][c] << ' ';
	    cout << endl;
	}

	cout << endl;
    }

    bool place(int r, int c, int s, int o, int g_old, int g_new) {
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

    void try_pos(int r, int c) {
	// Find next empty hole on the grid
	while (grid[r][c] != 0)
	    if (++c == W) {
		c = 0;

		if (++r == H) {
		    // Got to end, must have solution
		    show();
		    return;
		}
	    }

	// Try each remaining shape
	for (int s = 1; s <= 12; s++)
	    if (!placed[s])
		for (int o = 0; o < shapes[s - 1].size(); o++) {
		    if (place(r, c, s, o, 0, s)) {
			placed[s] = true;
			try_pos(r, c);
		    }

		    // undo full or partial placement
		    place(r, c, s, o, s, 0);
		    placed[s] = false;
		}
    }

    int grid[H][W];
    bool placed[13];
};

int main()
{
    Grid g;

    g.try_pos(0, 0);

    return 0;
}
