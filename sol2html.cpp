// Pentomino Packing - by Curt McDowell 2022-01-17
//
// Filter utility to convert output of pent.cpp to SVG drawings in HTML

#include <cassert>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <getopt.h>
#include <vector>

using namespace std;

#define DEFAULT_W		120
#define DEFAULT_COLS		8
#define DEFAULT_MARGIN		2

static const char shape_names[] = ".ILMWVZUPYXRT";

static const char *svg_colors[12] = {
    "cyan", "red", "yellow", "brown", "blue", "orange",
    "green", "pink", "wheat", "violet", "indigo", "cadetblue"
};

using grid_t = vector<vector<int>>;

grid_t
sol_read(istream &f, int &w, int &h)
{
    vector<vector<int>> grid;
    string buf;

    w = 0;
    h = 0;

    while (getline(f, buf)) {
	int n = buf.length();
	if (n == 0)
	    break;
	if (w == 0)
	    w = (n + 1) / 2;
	else if (w != (n + 1) / 2) {
	    cerr << "Non-rectangular input" << endl;
	    exit(1);
	}
	h++;
	vector<int> row;
	const char *s = buf.c_str();
	for (int i = 0; i < n; i++) {
	    if (i % 2 == 0) {
		const char *p = strchr(shape_names, s[i]);
		if (p == NULL) {
		    cerr << "Unrecognized character '" <<
			s[i] << "' in input" << endl;
		    exit(1);
		}
		int d = (p - shape_names) - 1;
		row.push_back(d);
	    } else if (s[i] != ' ') {
		cerr << "Invalid whitespace in grid" << endl;
		exit(1);
	    }
	}
	assert((int)row.size() == w);
	grid.push_back(row);
    }

    return grid;
}

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

// Given a grid value, return the points of a clockwise path
// representing the perimeter of the shape corresponding to that value.

using path_t = vector<point_t>;

path_t
shape_to_path(grid_t &grid, int w, int h, int s)
{
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

    for (int r = 0; r < h; r++)
	for (int c = 0; c < w; c++)
	    if (grid[r][c] == s) {
		toggle(edge_t(point_t(r, c), point_t(r, c + 1)));
		toggle(edge_t(point_t(r, c + 1), point_t(r + 1, c + 1)));
		toggle(edge_t(point_t(r + 1, c + 1), point_t(r + 1, c)));
		toggle(edge_t(point_t(r + 1, c), point_t(r, c)));
	    }

    //for (auto &e : edges)
    //    cout << "edge(" << e.a.r << ", " << e.a.c << ", " <<
    //      e.b.r << ", " << e.b.c << ")" << endl;

    path_t path;

    point_t prev(edges[0].a);
    point_t cur(edges[0].b);

    point_t start = prev;
    path.push_back(start);

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

void
show_text(grid_t &grid)
{
    for (auto row : grid) {
	for (auto col : row)
	    cout << setw(3) << col << " ";

	cout << endl;
    }

    cout << endl;
}

void
grid2svg(grid_t &grid, int w, int h, int svg_w)
{
    int draw_x0 = DEFAULT_MARGIN;
    int draw_y0 = DEFAULT_MARGIN;
    int draw_w = svg_w - 2 * DEFAULT_MARGIN;
    int draw_h = svg_w * h / w;
    int svg_h = draw_h + 2 * DEFAULT_MARGIN;

    auto x = [=](int c) -> int {
		 return draw_x0 + (draw_w * c) / w;
	     };

    auto y = [=](int r) -> int {
		 return draw_y0 + (draw_h * r) / h;
	     };

    cout << "<svg width=\"" << svg_w <<
	"\" height=\"" << svg_h << "\">" << endl;

    for (int s = 0; s < 12; s++) {
	path_t path(shape_to_path(grid, w, h, s));

	cout << " <polygon style=\"fill:" << svg_colors[s] <<
	    ";stroke:black;stroke-width:1\" points=\"";

	bool first = true;

	for (auto &p : path) {
	    if (!first)
		cout << " ";
	    else
		first = false;
	    cout << x(p.c) << "," << y(p.r);
	}

	cout << "\" />" << endl;
    }

    for (int r = 1; r < h; r++)
	cout << " <line style=\"stroke:black;stroke-opacity:0.1;stroke-width:1\" " <<
	    "x1=\"" << x(0) << "\" y1=\"" << y(r) <<
	    "\" x2=\"" << x(w) << "\" y2=\"" << y(r) << "\" />" << endl;

    for (int c = 1; c < w; c++)
	cout << " <line style=\"stroke:black;stroke-opacity:0.1;stroke-width:1\" " <<
	    "x1=\"" << x(c) << "\" y1=\"" << y(0) <<
	    "\" x2=\"" << x(c) << "\" y2=\"" << y(h) << "\" />" << endl;

    cout << "</svg>" << endl;
}

void
usage()
{
    cerr << "Usage: sol2html [-W <width>] [-C <cols>] < sol.txt > sol.html" << endl;
    cerr << "  -W <width>      Specify SVG width (height gets calculated)" << endl;
    cerr << "  -C <cols>       Number of columns in table" << endl;
    exit(1);
}

int
main(int argc, char *argv[])
{
    int W = DEFAULT_W;
    int C = DEFAULT_COLS;
    int c;

    while ((c = getopt(argc, argv, "W:C:")) >= 0)
	switch (c) {
	case 'W':
	    W = atoi(optarg);
	    break;
	case 'C':
	    C = atoi(optarg);
	    break;
	default:
	    usage();
	    break;
	}

    cout << "<html>" << endl;
    cout << " <head>" << endl;
    cout << "  <title>Packed Pentominos</title>" << endl;
    cout << " </head>" << endl;
    cout << "<body>" << endl;

    int col = 0;

    cout << "<table>" << endl;
    cout << " <tr>" << endl;

    int count = 0;

    while (true) {
	int w, h;

	grid_t grid(sol_read(cin, w, h));
	if (h == 0)
	    break;

	count++;

	//show_text(grid);

	cout << "  <td>" << endl;
	cout << "  " << count << "<br>" << endl;
	grid2svg(grid, w, h, W);
	cout <<  "  </td>" << endl;

	if (++col == C) {
	    col = 0;
	    cout << " </tr>" << endl;
	    cout << " <tr>" << endl;
	}
    }

    cout << " </tr>" << endl;
    cout << "</table>" << endl;
    cout << "</body>" << endl;
    cout << "</html>" << endl;

    return 0;
}
