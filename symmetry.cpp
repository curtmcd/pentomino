// Pentomino Packing - by Curt McDowell 2022-01-17
//
// Filter utility to eliminate symmetries (rotational, horizontal flip
// and vertical flip) from the output of pent.cpp.
//
// If pents were allowed to be flipped, this divides the number of
// solutions by 8. Otherwise, it divides the number of solutions by 2.

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

using chunk_t = vector<string>;

chunk_t
read_chunk(istream &f)
{
    chunk_t chunk;
    string buf;

    int w = 0;

    while (getline(f, buf)) {
	int n = buf.length();

	if (n == 0)
	    break;

	// Trim trailing space so lines are odd length
	if (buf[n - 1] != ' ') {
	    cerr << "Invalid input" << endl;
	    exit(1);
	}

	buf.pop_back();

	if (w == 0)
	    w = n;
	else if (w != n) {
	    cerr << "Non-rectangular input" << endl;
	    exit(1);
	}

	chunk.push_back(buf);
    }

    return chunk;
}

void
print_chunk(const chunk_t &chunk)
{
    for (auto row : chunk)
	cout << row << " " << endl;

    cout << endl;
}

using chunk_list_t = vector<chunk_t>;

bool
congruent(const chunk_t &c1, const chunk_t &c2)
{
    bool sym_rot = true;
    bool sym_vert = true;
    bool sym_horz = true;

    int h = (int)c1.size();
    int w = (int)c1[0].size();

    assert((int)c2.size() == h);
    assert((int)c2[0].size() == w);

    for (int r = 0; r < h; r++)
	for (int c = 0; c < w; c++) {
	    if (c1[r][c] != c2[h - 1 - r][w - 1 - c])
		sym_rot = false;

	    if (c1[r][c] != c2[h - 1 - r][c])
		sym_vert = false;

	    if (c1[r][c] != c2[r][w - 1 - c])
		sym_horz = false;
	}

    return sym_rot || sym_vert || sym_horz;
}

int
main(int argc, char *argv[])
{
    chunk_list_t chunks;

    while (true) {
	chunk_t chunk(read_chunk(cin));

	if (chunk.size() == 0)
	    break;

	chunks.push_back(chunk);
    }

    if (chunks.size() < 2) {
	for (auto c : chunks)
	    print_chunk(c);

	return 0;
    }

    // Print chunks for which congruent peers have not already been printed

    for (auto cp1 = chunks.begin(); cp1 != chunks.end(); cp1++) {
	bool already = false;

	for (auto cp2 = chunks.begin(); cp2 < cp1; cp2++) {
	    if (congruent(*cp1, *cp2)) {
		already = true;
		break;
	    }
	}

	if (!already)
	    print_chunk(*cp1);
    }

    return 0;
}
