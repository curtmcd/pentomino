// Pentomino Packing - by Curt McDowell 2022-01-17
//
// There are twelve unique pentominos. They can be packed into a
// rectangular grid of 60 square units in many ways, where the grid size
// may be 10x6, 12x5, 15x4 or 20x3. If it's allowed to flip the six
// asymmetric pentominos, there are more ways to pack them.
//
// This program uses backtracking to find and print all packings
// (including trivially symmetric ones), given the grid size and choice
// of whether flips are allowed.

#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <getopt.h>

using namespace std;

enum dir_t { U, D, L, R };

static const char _shape_names[] = ".ILMWVZUPYXRT",
    *shape_names = &_shape_names[1];

// Turtle drawings for each orientation of each shape. The origin of
// each shape is always the left-most pixel of its top row of
// pixels. The first instruction can be only down or right since the
// shapes are placed from left to right, top to bottom.

using drawing_t = vector<dir_t>;

const vector<drawing_t> draw_normal[] = {
    {{ R, R, R, R 	},	// I
     { D, D, D, D 	}},
    {{ R, R, R, D 	},	// L
     { D, D, D, L 	},
     { D, R, R, R 	},
     { R, L, D, D, D 	}},
    {{ R, R, D, R 	},	// N
     { D, D, L, D 	},
     { R, D, R, R 	},
     { D, L, D, D 	}},
    {{ R, D, R, D 	},	// W
     { D, L, D, L 	},
     { D, R, D, R 	},
     { R, L, D, L, D 	}},
    {{ R, R, D, D 	},	// V
     { D, D, L, L 	},
     { D, D, R, R 	},
     { R, R, L, L, D, D }},
    {{ D, R, R, D 	},	// Z
     { R, L, D, D, L	}},
    {{ D, U, R, R, D 	},	// U
     { R, L, D, D, R 	},
     { D, R, R, U 	},
     { R, D, D, L 	}},
    {{ D, R, U, R 	},	// P
     { R, L, D, R, D 	},
     { R, D, L, L 	},
     { D, R, D, L 	}},
    {{ R, D, U, R, R 	},	// Y
     { D, L, R, D, D 	},
     { D, R, L, L, L 	},
     { D, D, R, L, D 	}},
    {{ D, L, R, R, L, D }},	// X
    {{ D, R, R, L, D 	},	// R
     { R, L, D, L, R, D },
     { D, L, R, R, D 	},
     { D, R, L, D, L 	}},
    {{ R, R, L, D, D 	},	// T
     { D, L, L, R, R, D },
     { D, D, L, R, R 	},
     { D, R, R, L, L, D }}
};

const vector<drawing_t> draw_flipped[] = {
    {},
    {{ D, U, R, R, R 	},	// L
     { D, D, D, R 	},
     { D, L, L, L 	},
     { R, D, D, D 	}},
    {{ R, R, L, L, D, L },	// N
     { D, R, D, D 	},
     { R, L, D, L, L 	},
     { D, D, R, D 	}},
    {},
    {},
    {{ D, L, L, D 	},	// Z
     { R, D, D, R 	}},
    {},
    {{ R, R, D, L 	},	// P
     { D, L, D, R 	},
     { R, L, D, R, R 	},
     { R, D, L, D 	}},
    {{ R, R, R, L, D 	},	// Y
     { D, D, L, R, D 	},
     { D, L, R, R, R 	},
     { D, R, L, D, D 	}},
    {},
    {{ D, L, L, R, D 	},	// R
     { D, L, R, D, R 	},
     { D, R, L, L, D 	},
     { R, D, R, L, D 	}},
    {}
};

struct grid_t {
    grid_t(int _w, int _h, bool allow_flipped) : w(_w), h(_h)
    {
	for (int r = 0; r < h; r++) {
	    vector<int> row;

	    for (int c = 0; c < w; c++)
		row.push_back(-1);

	    pos.push_back(row);
	}

	for (auto &p : pieces)
	    p.placed = false;

	for (int i = 0; i < 12; i++) {
	    for (auto &d : draw_normal[i])
		shapes[i].push_back(d);

	    if (allow_flipped)
		for (auto &d : draw_flipped[i])
		    shapes[i].push_back(d);
	}
    }

    string
    to_text() const
    {
	stringstream t;

	for (int r = 0; r < h; r++) {
	    for (int c = 0; c < w; c++) {
		assert(pos[r][c] >= -1 && pos[r][c] < 12);
		t << shape_names[pos[r][c]] << ' ';
	    }

	    t << endl;
	}

	t << endl;

	return t.str();
    }

    bool
    place(int r, int c, int s, int o, int s_old, int s_new)
    {
	const drawing_t &moves = shapes[s][o];

	assert(pos[r][c] == s_old);

	pos[r][c] = s_new;

	for (auto &m : moves) {
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

	    if (pos[r][c] != s_old && pos[r][c] != s_new)
		return false;

	    pos[r][c] = s_new;
	}

	return true;
    }

    void
    try_pos(int r, int c)
    {
	// Find next empty hole on the grid
	while (pos[r][c] >= 0)
	    if (++c == w) {
		c = 0;
		if (++r == h) {
		    // Got to end, must have solution
		    cout << to_text();
		    return;
		}
	    }

	// Try each remaining shape
	for (int s = 0; s < 12; s++)
	    if (!pieces[s].placed)
		for (int o = 0; o < (int)shapes[s].size(); o++) {
		    if (place(r, c, s, o, -1, s)) {
			pieces[s].placed = true;
			pieces[s].r = r;
			pieces[s].c = c;
			pieces[s].o = o;
			try_pos(r, c);
		    }

		    // undo full or partial placement
		    place(r, c, s, o, s, -1);
		    pieces[s].placed = false;
		}
    }

    int w, h;
    vector<vector<int>> pos;
    struct {
	bool placed;
	int r, c, o;	// valid only when placed
    } pieces[12];
    vector<drawing_t> shapes[12];
};

void
usage()
{
    cerr << "Usage: pent [-w <width>] [-N]" << endl;
    cerr << "  -w <width>    Specify grid width, one of 20, 15, 12 or 10," << endl;
    cerr << "                default 10. Height is 60 divided by width." << endl;
    cerr << "  -N            Don't allow flip (12 shapes instead of 18)" << endl;

    exit(1);
}

int
main(int argc, char *argv[])
{
    int c, w = 10;
    bool allow_flipped = true;

    while ((c = getopt(argc, argv, "w:Nh")) >= 0)
	switch (c) {
	case 'w':
	    w = atoi(optarg);
	    break;
	case 'N':
	    allow_flipped = false;
	    break;
	default:
	    usage();
	    break;
	}

    if (w != 20 && w != 15 && w != 12 && w != 10)
	usage();

    grid_t g(w, 60 / w, allow_flipped);

    g.try_pos(0, 0);

    return 0;
}
