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
enum output_format_t { OF_TEXT, OF_POSITIONS, OF_SVG };

#define SVG_UNIT_SIZE 	10
#define SVG_COLS	6
#define SVG_MARGIN	5

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

struct point_t {
    point_t(int _r, int _c) : r(_r), c(_c) {}

    bool operator==(const point_t &other) const {
	return (other.r == r && other.c == c);
    }

    bool operator!=(const point_t &other) const {
	return (other.r != r || other.c != c);
    }

    int r, c;
};

// Given a drawing and a starting position, return the points of a
// clockwise path representing the perimeter of the drawing.

using path_t = vector<point_t>;

path_t drawing_to_path(const drawing_t &moves, int r, int c)
{
    point_t start(r, c);

    vector<point_t> boxes;

    point_t box = start;	// Boxes denoted by their upper left point

    boxes.push_back(box);

    for (auto &m : moves) {
	switch (m) {
	case U:
	    --box.r;
	    break;
	case D:
	    ++box.r;
	    break;
	case L:
	    --box.c;
	    break;
	case R:
	    ++box.c;
	    break;
	default:
	    assert(false);
	}

	bool already_added = false;

	for (auto &b : boxes)
	    if (b == box)
		already_added = true;

	if (!already_added)
	    boxes.push_back(box);
    }

    //for (auto &b : boxes)
    //    cout << "box(" << b.r << ", " << b.c << ")" << endl;

    struct edge_t {
	edge_t(const point_t &_a, const point_t &_b) : a(_a), b(_b) {}

	bool same(const edge_t &e) {
	    return ((e.a == a && e.b == b) || (e.a == b && e.b == a));
	}

	point_t a, b;
    };

    vector<edge_t> edges;

    auto toggle =
	[&edges](const edge_t &e) {
	    for (auto ei = edges.begin(); ei != edges.end(); ei++)
		if (ei->same(e)) {
		    edges.erase(ei);
		    return;
		}

	    edges.push_back(e);
	};

    for (auto &b : boxes) {
	toggle(edge_t(point_t(b.r, b.c), point_t(b.r, b.c + 1)));
	toggle(edge_t(point_t(b.r, b.c + 1), point_t(b.r + 1, b.c + 1)));
	toggle(edge_t(point_t(b.r + 1, b.c + 1), point_t(b.r + 1, b.c)));
	toggle(edge_t(point_t(b.r + 1, b.c), point_t(b.r, b.c)));
    }

    //for (auto &e : edges)
    //    cout << "edge(" << e.rs << ", " << e.cs << ", " << e.re << ", " << e.ce << ")" << endl;

    path_t path;

    point_t prev(edges[0].a);
    point_t cur(edges[0].b);

    assert(prev == start);

    path.push_back(prev);

    while (cur != start) {
	path.push_back(cur);

	// Find an edge which starts/ends at cur but does not
	// end/start at prev (ensures forward movement around
	// the perimeter)

	for (auto &e : edges) {
	    if (e.a == cur && e.b != prev) {
		prev = cur;
		cur = e.b;
		break;
	    }

	    if (e.b == cur && e.a != prev) {
		prev = cur;
		cur = e.a;
		break;
	    }
	}
    }

    //for (auto &p : path)
    //    cout << "path(" << p.r << ", " << p.c << ")" << endl;

    return path;
}

struct grid_t {
    grid_t(int _w, int _h, bool allow_flipped, output_format_t _of) :
	w(_w), h(_h), of(_of) {
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

	svg_col = 0;
    }

    string to_text() const {
	static const char _names[] = ".ILMWVZUPYXRT", *names = &_names[1];
	stringstream t;

	for (int r = 0; r < h; r++) {
	    for (int c = 0; c < w; c++) {
		assert(pos[r][c] >= -1 && pos[r][c] < 12);
		t << names[pos[r][c]] << ' ';
	    }

	    t << endl;
	}

	t << endl;

	return t.str();
    }

    string to_positions() const {
	stringstream t;

	for (int s = 0; s < 12; s++)
	    if (pieces[s].placed) {
		int r = pieces[s].r;
		int c = pieces[s].c;
		int o = pieces[s].o;

		t << "shape " << s << " at (" << r << ", " << c <<
		    ") orientation " << o << endl;
	    }

	t << endl;

	return t.str();
    }

    string to_svg() const {
	stringstream t;

	t << "<svg width=\"" << (w * SVG_UNIT_SIZE + 2 * SVG_MARGIN) <<
	    "\" height=\"" << (h * SVG_UNIT_SIZE + 2 * SVG_MARGIN) << "\">" << endl;

	static const char *svg_colors[12] = {
            "cyan", "red", "yellow", "brown", "blue", "orange",
	    "green", "pink", "wheat", "violet", "indigo", "cadetblue"
	};

	for (int s = 0; s < 12; s++)
	    if (pieces[s].placed) {
		int r = pieces[s].r;
		int c = pieces[s].c;
		int o = pieces[s].o;

		path_t path(drawing_to_path(shapes[s][o], r, c));

	        t << " <polygon style=\"fill:" << svg_colors[s] <<
		    ";stroke:black;stroke-width:1\" points=\"";

		bool first = true;

		for (auto &p : path) {
		    if (!first)
			t << " ";
		    else
			first = false;
		    t << (p.c * SVG_UNIT_SIZE + SVG_MARGIN / 2) <<
			"," << (p.r * SVG_UNIT_SIZE + SVG_MARGIN / 2);
		}

		t << "\" />" << endl;
	    }

	t << "</svg>" << endl;

	return t.str();
    }

    bool place(int r, int c, int s, int o, int s_old, int s_new) {
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

    void try_pos(int r, int c) {
	// Find next empty hole on the grid
	while (pos[r][c] >= 0)
	    if (++c == w) {
		c = 0;
		if (++r == h) {
		    // Got to end, must have solution
		    switch (of) {
		    case OF_TEXT:
			cout << to_text();
			break;
		    case OF_POSITIONS:
			cout << to_positions();
			break;
		    case OF_SVG:
			cout << to_svg();
			if (++svg_col == SVG_COLS) {
			    svg_col = 0;
			    cout << "<br>" << endl;
			}
			break;
		    default:
			assert(false);
			break;
		    }

		    return;
		}
	    }

	// Try each remaining shape
	for (int s = 0; s < 12; s++)
	    if (!pieces[s].placed)
		for (int o = 0; o < shapes[s].size(); o++) {
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
    output_format_t of;
    vector<vector<int>> pos;
    struct {
	bool placed;
	int r, c, o;	// valid only when placed
    } pieces[12];
    vector<drawing_t> shapes[12];
    int svg_col;
};

void
usage()
{
    cerr << "Usage: pent [-w <width>] [-N] [-f <format>]" << endl;
    cerr << "  -w <width>    Specify grid width, one of 20, 15, 12 or 10," << endl;
    cerr << "                default 10. Height is 60 divided by width." << endl;
    cerr << "  -N            Don't allow flip (12 shapes instead of 18)" << endl;
    cerr << "  -f <format>   Output format (text, positions or svg)" << endl;

    exit(1);
}

int
main(int argc, char *argv[])
{
    int c, w = 10, h;
    bool allow_flipped = true;
    output_format_t of = OF_TEXT;

    while ((c = getopt(argc, argv, "w:Nf:h")) >= 0)
	switch (c) {
	case '?':
	case 'h':
	    usage();
	    break;
	case 'w':
	    w = atoi(optarg);
	    break;
	case 'N':
	    allow_flipped = false;
	    break;
	case 'f':
	    if (strcmp(optarg, "text") == 0)
		of = OF_TEXT;
	    else if (strcmp(optarg, "positions") == 0)
		of = OF_POSITIONS;
	    else if (strcmp(optarg, "svg") == 0)
		of = OF_SVG;
	    else
		usage();
	    break;
	}

    if (w != 20 && w != 15 && w != 12 && w != 10)
	usage();

    grid_t g(w, 60 / w, allow_flipped, of);

    g.try_pos(0, 0);

    return 0;
}
