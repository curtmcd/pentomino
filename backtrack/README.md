# Pentomino Packing with Backtracking

This set of programs implements a simple recursive backtracking solver
to find all possible ways to pack the twelve pentominos into a
rectangular grid of size 10x6, 12x5, 15x4 or 20x3. It optionally
eliminates symmetry redundancies and renders packings as pretty HTML5
files using SVG drawings.

![Screenshot of 10x6 solution](/sol-10x6-N.png)

# Programs

The programs here are written in C++11 and are ready to be compiled on a
basic GNU/Linux system using `gmake` (see compilation instructions
below).

## pent

This program generates the packings and writes them to stdout in a
simple ASCII format like this, with a blank line between each packing.

    I I I I I L L L L M 
    P P P W W X U U L M 
    P P W W X X X U M M 
    V Z W R R X U U M T 
    V Z Z Z R R Y T T T 
    V V V Z R Y Y Y Y T 

Options include the ability to select the grid size (`-w <width>`) and whether
or not flipping is disallowed (`-N`).

Example: `./pent -w 10 -N > sol-10x6-N.txt`

## symmetry

This program is a filter that reads from stdin expecting the output
format of **pent**. It removes all packings that are redundant through
the symmetries of rotation, horizontal flipping and vertical flipping,
and writes the remaining packings to stdout in the same format.

Six of the pentominos are not rotationally symmetric. If they are
allowed to be flipped, the number of packings increases substantially.

Eliminating symmetries from the flips-allowed packings reduces the
number of packings to one eighth.

Eliminating symmetries from the flips-disallowed packings reduces the
number of packings to one half.

Example: `./symmetry < sol-10x6-N.txt > sol-10x6-N.symm`

## sol2html

This program is a filter that reads from stdin expecting the same format
as output by **pent** or **symmetry**. It writes to stdout an HTML page
containing a table of SVG pictures. Command line options allow setting
the size of each SVG picture (`-W 120`) and the number of pictures per
column (`-C 8`).

To see the output in your browser, dump the stdout into a .html file and
point your browser at it (`file://home/blah/blah/sol-10x6.html`).

Example 1: `./sol2html < sol-10x6-N.txt > sol-10x6-N-huge.html`

Example 2: `./sol2html < sol-10x6-N.symm > sol-10x6-N.html`

# Build and run

Running `make` compiles the programs.

Running `make all` is comprehensive and equivalent to `make test_html`.

Running `make test_sol` produces output files for all combinations of flags:

- sol-10x6.txt
- sol-10x6-N.txt
- sol-12x5.txt
- sol-12x5-N.txt
- sol-15x4.txt
- sol-15x4-N.txt
- sol-20x3.txt
- sol-20x3-N.txt

Running `make test_symm` processes these output files to remove redundant
symmetries, producing files in the same format:

- sol-10x6.symm
- sol-10x6-N.symm
- sol-12x5.symm
- sol-12x5-N.symm
- sol-15x4.symm
- sol-15x4-N.symm
- sol-20x3.symm
- sol-20x3-N.symm

Running `make test_html` processes the .symm files into .html files
containing SVG drawings.

- sol-10x6.html
- sol-10x6-N.html _(title screenshot)_
- sol-12x5.html
- sol-12x5-N.html
- sol-15x4.html
- sol-15x4-N.html
- sol-20x3.html
- sol-20x3-N.html

## Notes

I started this project after downloading a 3D printed pentomino packing
puzzle from Thingiverse and wondering how many ways there were to solve
it. The object included the 12 pieces and a 10x6 board to hold them (but
unfortunately did not include the other board sizes).

Initially I created a solver that generated a boolean satisfiability
equation (SAT) in the .cnf format and used zChaff to solve it. This
worked, but runs very slowly and only finds one solution before exiting.
So I gave up on that and did the backtracking version. It doesn't take
long, maybe a few minutes in the worst case to find all solutions for a
given grid width.

## Author

- [Profile](https://github.com/curtmcd "Curt McDowell")
- Email: *coder＠fishlet.com*
- [Website](https://www.fishlet.com)
