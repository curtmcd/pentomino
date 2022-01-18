#include <cassert>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <memory.h>
#include <stdlib.h>
#include <getopt.h>

// To do:
//
//   This prints all solutions but should not print trivially symmetric
//   ones. Also, it may not be finding all of the solutions because the
//   count for 10x6 should equal Wikipedia's 2239 times 8, I think.

using namespace std;

enum dir { U, D, L, R };	// Up, down, left, right

#define ALLOW_FLIPPED		1
//#define ALLOW_FLIPPED		0

vector<vector<dir>> shapes[] = {
    { R, R, R, R },		// I0
    { D, D, D, D },		// I1
    { R, R, R, D },		// L0
    { D, D, D, L },		// L1
    { D, R, R, R },		// L2
    { R, L, D, D, D },		// L3
    { D, U, R, R, R },		// L'0		('=flipped)
    { D, D, D, R },		// L'1
    { D, L, L, L },		// L'2
    { R, D, D, D },		// L'3
    { R, R, D, R },		// N0
    { D, D, L, D },		// N1
    { R, D, R, R },		// N2
    { D, L, D, D },		// N2
    { R, R, L, L, D, L },	// N'0
    { D, R, D, D },		// N'1
    { R, L, D, L, L },		// N'2
    { D, D, R, D },		// N'3
    { R, D, R, D },		// W0
    { D, L, D, L },		// W1
    { D, R, D, R },		// W2
    { R, L, D, L, D }		// W3
    { R, R, D, D },		// V0
    { D, D, L, L },		// V1
    { D, D, R, R },		// V2
    { R, R, L, L, D, D }	// V3
    { D, R, R, D },		// Z0
    { L, D, D, L },		// Z1
    { D, L, L, D },		// Z'0
    { R, D, D, R },		// Z'1
    { D, U, R, R, D },		// U0
    { R, L, D, D, R },		// U1
    { D, R, R, U },		// U2
    { R, D, D, L }		// U3
    { D, R, U, R },		// P0
    { R, L, D, R, D },		// P1
    { R, D, L, L },		// P2
    { D, R, D, L },		// P3
    { R, R, D, L },		// P'0
    { D, L, D, R },		// P'1
    { R, L, D, R, R },		// P'2
    { R, D, L, D },		// P'3
    { R, R, R, L, D },		// Y0
    { D, D, L, R, D },		// Y1
    { D, L, R, R, R },		// Y2
    { D, R, L, D, D },		// Y3
    { R, D, U, R, R },		// Y'0
    { D, L, R, D, D },		// Y'1
    { D, R, L, L, L },		// Y'2
    { D, D, R, L, D },		// Y'3
    { D, L, R, R, L, D }	// X0
    { D, R, R, L, D },		// R0
    { R, L, D, L, R, D },	// R1
    { D, L, R, R, D },		// R2
    { D, R, L, D, L },		// R3
    { D, L, L, R, D },		// R'0
    { D, L, R, D, R },		// R'1
    { D, R, L, L, D },		// R'2
    { R, D, R, L, D },		// R'3
    { R, R, L, D, D },		// T0
    { D, L, L, R, R, D },	// T1
    { D, D, L, R, R },		// T2
    { D, R, R, L, L, D }	// T3
};

struct Grid {
    Grid(int _w, int _h) : w(_w), h(_h) {
	for (int r = 0; r < h; r++) {
	    vector<int> row;
	    for (int c = 0; c < w; c++)
		row.push_back(0);
	    grid.push_back(row);
	}

	for (int i = 0; i < 13; i++)
	    placed.push_back(false);
    }

    void show() {
	for (int r = 0; r < h; r++) {
	    for (int c = 0; c < w; c++)
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
		if (++r >= h)
		    return false;
		break;
	    case L:
		if (--c < 0)
		    return false;
		break;
	    case R:
		if (++c >= w)
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
	    if (++c == w) {
		c = 0;

		if (++r == h) {
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

    int w, h;
    vector<vector<int>> grid;
    vector<bool> placed;
};

void
usage()
{
    cerr << "Usage: pent [-w <width>] [-F]" << endl;
    cerr << "  -w <width>    Specify grid width (height is 60 / width)" << endl;
    cerr << "                Valid widths are 20, 15, 12 and 10" << endl;
    cerr << "  -F            Don't let pents be flipped (12 shapes instead of 18)" << endl;

    exit(1);
}

int
main(int argc, char *argv[])
{
    int c, w = 10, h;
    bool allow_flipped = true;

    while ((c = getopt(argc, argv, "w:Fh")) >= 0)
	switch (c) {
	case '?':
	case 'h':
	    usage();
	    break;
	case 'w':
	    w = atoi(argv[optind]);
	    break;
	case 'F':
	    allow_flipped = false;
	    break;
	}

    if (w != 20 && w != 15 && w != 12 && w != 10)
	usage();

    Grid g(w, 60 / w, allow_flipped);

    g.try_pos(0, 0);

    return 0;
}
