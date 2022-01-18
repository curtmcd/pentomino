#include <cassert>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <memory.h>

using namespace std;

enum dir { F, B, L, R };	// Forward, backward, left, right

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

// Laboriously determine the smallest "pocket" on the grid, where a pocket
// is an orthogonally-connected region of holes (empty grit spots).
int smallest_pocket_size()
{
    int hole[6][10];
    memset(hole, 0, sizeof (hole));
    int pocket_no = 0;
    map<int, int> counts;

    for (int r = 0; r < 6; r++)
	for (int c = 0; c < 10; c++)
	    if (grid[r][c] == 0) {
		int p = 0;
		if (c > 0 && hole[r][c - 1] > 0)
		    p = hole[r][c - 1];
		else if (r > 0) {
		    for (int c2 = c; c2 < 10 && grid[r][c2] == 0; c2++)
			if (hole[r - 1][c2] > 0)
			    p = hole[r - 1][c2];
		}
		if (p == 0)
		    p = ++pocket_no;
		hole[r][c] = p;
		auto it = counts.find(p);
		if (it != counts.end())
		    it->second++;
		else
		   counts.insert(pair<int, int>(p, 1));
	    }

    for (int r = 0; r < 6; r++) {
	for (int c = 0; c < 10; c++)
	    cout << setw(3) << hole[r][c] << ' ';
	cout << endl;
    }
    cout << endl;

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
    // Greatly optimize by denying placement if it would result in the
    // grid having any pocket of empty cells less than 5 in size that
    // would become dead space.
    int a=smallest_pocket_size();
    cout << "sps " <<a  << endl;
    if (a < 5)
	return false;
    return true;
}

bool placed[13];

bool next(int &r, int &c)
{
    if (++c == 10) {
	c = 0;
	if (++r == 6)
	    return false;
    }
    return true;
}

void try_pos(int r, int c)
{
    // Find next hole on grid
    while (grid[r][c] != 0)
	if (!next(r, c)) {
	    show();	// Got to end, must have solution
	    return;
	}

    cout << "next hole at (" << r << ", " << c << ")" << endl;
    for (int s = 1; s <= 12; s++) {
	if (!placed[s])
	    // XXX don't need to try all orientations, can't go left or up
	    for (dir o = F; o <= R; o = dir(o + 1)) {
		cout << "try " << s << endl;
		if (place(r, c, s, o, 0, s)) {
		    cout << "placed " << s << " orient " << o << endl;
		    show();
		    placed[s] = true;
		    try_pos(r, c);
		}
		else {
		    cout << "NOT placed " << s << " orient " << o << endl;
		    show();
		}
		place(r, c, s, o, s, 0);  // undo placement
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
