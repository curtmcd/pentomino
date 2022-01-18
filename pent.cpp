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

enum dir { U, D, L, R };

const vector<vector<dir>> draw_normal[] = {
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
     { L, D, D, L 	}},
    {{ D, U, R, R, D 	},	// U
     { R, L, D, D, R 	},
     { D, R, R, U 	},
     { R, D, D, L 	}},
    {{ D, R, U, R 	},	// P
     { R, L, D, R, D 	},
     { R, D, L, L 	},
     { D, R, D, L 	}},
    {{ R, R, R, L, D 	},	// Y
     { D, D, L, R, D 	},
     { D, L, R, R, R 	},
     { D, R, L, D, D 	}},
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

const vector<vector<dir>> draw_flipped[] = {
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
    {{ R, D, U, R, R 	},	// Y
     { D, L, R, D, D 	},
     { D, R, L, L, L 	},
     { D, D, R, L, D 	}},
    {},
    {{ D, L, L, R, D 	},	// R
     { D, L, R, D, R 	},
     { D, R, L, L, D 	},
     { R, D, R, L, D 	}},
    {}
};

struct Grid {
    Grid(int _w, int _h, bool allow_flipped) : w(_w), h(_h) {
	for (int r = 0; r < h; r++) {
	    vector<int> row;

	    for (int c = 0; c < w; c++)
		row.push_back(-1);

	    grid.push_back(row);
	}

	for (auto &p : placed)
	    p = false;

	for (int i = 0; i < 12; i++) {
	    for (auto d : draw_normal[i])
		shapes[i].push_back(d);

	    if (allow_flipped)
		for (auto d : draw_flipped[i])
		    shapes[i].push_back(d);
	}
    }

    void show() const {
	for (int r = 0; r < h; r++) {
	    for (int c = 0; c < w; c++)
		cout << setw(3) << (grid[r][c] + 1) << ' ';
	    cout << endl;
	}

	cout << endl;
    }

    bool place(int r, int c, int s, int o, int g_old, int g_new) {
	const vector<dir> &moves = shapes[s][o];

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
	while (grid[r][c] >= 0)
	    if (++c == w) {
		c = 0;

		if (++r == h) {
		    // Got to end, must have solution
		    show();
		    return;
		}
	    }

	// Try each remaining shape
	for (int s = 0; s < 12; s++)
	    if (!placed[s])
		for (int o = 0; o < shapes[s].size(); o++) {
		    if (place(r, c, s, o, -1, s)) {
			placed[s] = true;
			try_pos(r, c);
		    }

		    // undo full or partial placement
		    place(r, c, s, o, s, -1);
		    placed[s] = false;
		}
    }

    int w, h;
    vector<vector<int>> grid;
    bool placed[12];
    vector<vector<dir>> shapes[12];
};

void
usage()
{
    cerr << "Usage: pent [-w <width>] [-F]" << endl;
    cerr << "  -w <width>    Specify grid width, one of 20, 15, 12 or 10," << endl;
    cerr << "                default 10. Height is 60 divided by width." << endl;
    cerr << "  -F            Don't allow flip (12 shapes instead of 18)" << endl;

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
	    w = atoi(optarg);
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
