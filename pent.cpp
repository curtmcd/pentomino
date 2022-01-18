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
}

typedef vector<pair<int, int>> undo_list;

bool place(int r, int c, int n, dir orient)
{
    vector<dir> &o = reorient[orient];
    if (grid[r][c] != 0)
	return false;
    grid[r][c] = n;
    undo_list u;
    u.push_back(make_pair(r, c));
    for (auto &d : shapes[n - 1]) {
	switch (o[d]) {
	case F:
	    if (--r < 0)
		goto fail;
	    break;
	case B:
	    if (++r > 9)
		goto fail;
	    break;
	case L:
	    if (--c < 0)
		goto fail;
	    break;
	case R:
	    if (++c > 9)
		goto fail;
	    break;
	default:
	    assert(false);
	}
	if (grid[r][c] != 0 && grid[r][c] != n)
	    break;
	grid[r][c] = n;
	u.push_back(make_pair(r, c));
    }
    return true;
 fail:
    for (auto &p : u)
	grid[p.first][p.second] = 0;
    return false;
}

int main()
{
    memset(grid, 0, sizeof (grid));
    place(2, 3, 9, L);
    show();
    return 0;
}
