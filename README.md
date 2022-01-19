# Pentomino packing

![Screenshot of 10x6 solution](/sol-10x6-N.png)

This application finds all possible ways to pack the twelve pentominos
into a rectangular grid of size 10x6, 12x5, 15x4 or 20x3. It will also
render those packings as HTML5 files using SVG drawings.

# Programs

The programs here are written in C++11 and are ready to be compiled on
a GNU/Linux system using `gmake`.

## pent

This program generates the packings and writes them to stdout in a
simple ASCII format like this:

    I I I I I L L L L M 
    P P P W W X U U L M 
    P P W W X X X U M M 
    V Z W R R X U U M T 
    V Z Z Z R R Y T T T 
    V V V Z R Y Y Y Y T 

Options include the ability to select the grid size (by width) and whether
or not flipping is disallowed (-N).

## symmetry

This program is a filter that reads from stdin expecting the output
format f **pent**. It removes all packings that are redundant through
the symmetries of rotation, horizontal flipping and vertical flipping,
and writes the remaining packings to stdout.

Six of the pentominos are not rotationally symmetric. If they are
allowed to be flipped, the number of packings increases substantially.

Eliminating symmetries from the non-flipping pentominos reduces the
number of packings to one half.

Eliminating symmetries from the flipping pentominos reduces the number
of packings to one eighth.

## sol2html

This program is a filter that reads the output of **pent** or **symmetry**
and generates an HTML file. Command line options allow setting the
size of each SVG picture and the number of pictures per column.

Point your browser at an output file to render
graphic drawings of the packings.

# Build and run

Running `make` compiles the programs.

Running `make all` is comprehensive and equivalent to `make test_html`.

Running `make test_sol' produces output files for all combinations of flags:

- sol-10x6.txt
- sol-10x6-N.txt
- sol-12x5.txt
- sol-12x5-N.txt
- sol-15x4.txt
- sol-15x4-N.txt
- sol-20x3.txt
- sol-20x3-N.txt

Running 'make test_symm' processes these output files to remove redundant
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
- sol-10x6-N.html (title screenshot)
- sol-12x5.html
- sol-12x5-N.html
- sol-15x4.html
- sol-15x4-N.html
- sol-20x3.html
- sol-20x3-N.html

## Author

- [Profile](https://github.com/curtmcd "Curt McDowell")
- Email *coder＠fishlet.com*
- [Website](https://www.fishlet.com)
