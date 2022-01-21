#!/usr/bin/env python3

# Pentomino packing
#
# There are 12 pentominos unique under rotation and flipping. They occupy
# 60 squares on a grid. Possible grid sizes are 10 x 6, 12 x 5, 15 x 4,
# and 20 x 3. (A 3D grid of 3 x 4 x 5 is also possible).

import sys
import sat

FLIPS_ALLOWED = True
#FLIPS_ALLOWED = False

W, H = 10, 6
#W, H = 12, 5
#W, H = 15, 4
#W, H = 20, 3

class Shape:
    def __init__(self, rows):
        self.rows = rows
    def height(self):
        return len(self.rows)
    def width(self):
        return len(self.rows[0])
    def rot90(self):
        return Shape([''.join([self.rows[c][self.width() - 1 - r]
                               for c in range(self.height())])
                      for r in range(self.width())])
    def flip(self):
        return Shape([r[: : -1] for r in self.rows])
    def orient(self, n):
        if n == 0:
            return self
        elif n == 1:
            return self.rot90()
        elif n == 2:
            return self.rot90().rot90()
        elif n == 3:
            return self.rot90().rot90().rot90()
        elif n == 4:
            return self.flip()
        elif n == 5:
            return self.flip().rot90()
        elif n == 6:
            return self.flip().rot90().rot90()
        elif n == 7:
            return self.flip().rot90().rot90().rot90()
        else:
            assert(False)
    def pixel(self, r, c):
        return self.rows[r][c] == 'x'
    def show(self):
        print('\n'.join(self.rows))

# Pentomino shapes
pents = [
    Shape(['xxxxx']),                   # I
    Shape(['xxxx', '   x']),            # L
    Shape(['xxx ', '  xx']),            # N
    Shape(['xx ',' xx','  x']),         # W
    Shape(['xxx','  x','  x']),         # V
    Shape(['x  ','xxx','  x']),         # Z
    Shape(['x x','xxx']),               # U
    Shape(['xxx','xx ']),               # P
    Shape(['xxxx','  x ']),             # Y
    Shape([' x ','xxx',' x ']),         # X
    Shape(['x  ','xxx',' x ']),         # R
    Shape(['xxx',' x ',' x '])          # T
]

# All unique orientations for each pent.
# Total 67 figures if flips are allowed, 43 if not.
orientations = [
    [ 0, 1 ],
    [ 0, 1, 2, 3, 4, 5, 6, 7 ],
    [ 0, 1, 2, 3, 4, 5, 6, 7 ],
    [ 0, 1, 2, 3 ],
    [ 0, 1, 2, 3 ],
    [ 0, 1, 2, 3, 4, 5, 6, 7 ],
    [ 0, 1, 2, 3 ],
    [ 0, 1, 2, 3, 4, 5, 6, 7 ],
    [ 0, 1, 2, 3, 4, 5, 6, 7 ],
    [ 0 ],
    [ 0, 1, 2, 3, 4, 5, 6, 7 ],
    [ 0, 1, 2, 3 ]
] if FLIPS_ALLOWED else [
    [ 0, 1 ],
    [ 0, 1, 2, 3 ],
    [ 0, 1, 2, 3 ],
    [ 0, 1, 2, 3 ],
    [ 0, 1, 2, 3 ],
    [ 0, 1, 2, 3 ],
    [ 0, 1, 2, 3 ],
    [ 0, 1, 2, 3 ],
    [ 0, 1, 2, 3 ],
    [ 0 ],
    [ 0, 1, 2, 3 ],
    [ 0, 1, 2, 3 ]
]

# Construct variables and clauses to encode problem
def encode():
    cnf = sat.CNF()

    for r in range(H):
        for c in range(W):
            # Assert that each grid spot must have some pent in it.
            # This is redundant but may improve performance.
            rc_nonempty = []
            for p in range(12):
                rc_nonempty.append(cnf.var('gridded', p, r, c))
            cnf.comment('Grid (%d, %d) must have a pent' % (r, c))
            cnf.clause(rc_nonempty)

            # Assert that each grid spot has at most one pent in it.
            cnf.comment('Grid (%d, %d) may not have more than one pent' % (r, c))
            for p1 in range(11):
                for p2 in range(p1 + 1, 12):
                    rc_no_dup = [-cnf.var('gridded', p1, r, c),
                                 -cnf.var('gridded', p2, r, c)]
                    cnf.clause(rc_no_dup)

    # Associate a command var c with every possible grid position and
    # orientation of every pent. For example,
    #   -c v1      E.g., if c is true then all of v1-v5 must be true,
    #   -c v2      where v1-5 assert that the 5 ominoes of pentomino
    #   ...        p with orientation o exist in specific grid positions.
    #   -c v5
    for p in range(12):
        placements = []
        orients = orientations[p]
        for o in range(len(orients)):
            figure = pents[p].orient(orients[o])
            figure_w = figure.width()
            figure_h = figure.height()
            for r in range(H - figure_h + 1):
                for c in range(W - figure_w + 1):
                    cnf.comment('Command var for pent %d placed in orientation %d at (%d, %d)' %
                                (p, o, r, c))
                    v_placed = cnf.var('placed', p, o, r, c)
                    for sr in range(figure_h):
                        for sc in range(figure_w):
                            if figure.pixel(sr, sc):
                                v_gridded = cnf.var('gridded',
                                                    p, r + sr, c + sc)
                                omino = [-v_placed, v_gridded]
                                cnf.clause(omino)
                    placements.append(v_placed)

        # Assert that this pent must exist in any of its possible placements
        cnf.comment('Pent %d must exist in one of its possible placements' % p)
        cnf.clause(placements)

        # Assert that this pent occurs no more than once.
        # This is redundant but greatly improves performance.
        cnf.comment('Pent %d should not occur in more than one placement' % p)
        for i in range(len(placements) - 1):
            for j in range(i + 1, len(placements)):
                cnf.clause([-placements[i], -placements[j]])

    return cnf

class Grid:
    def __init__(self):
        self.rows = [[-1 for _ in range(W)] for _ in range(H)]
    def assign(self, r, c, p):
        self.rows[r][c] = p
    def __str__(self):
        s = ''
        for gr in self.rows:
            for p in gr:
                s += '%3d' % p if p >= 0 else '  .'
            s += '\n'
        s += '\n'
        return s

def decode(cnf, sol):
    grid = Grid()
    for r in range(H):
        for c in range(W):
            for p in range(12):
                if cnf.var('gridded', p, r, c) in sol:
                    grid.assign(r, c, p)
    return grid

def main():
    cnf = encode()

    sol = cnf.solve()
    #print(list(sol))

    grid = decode(cnf, sol)

    print(grid)

if __name__ == '__main__':
    main()
