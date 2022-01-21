# Pentomino Packing with SAT

![Screenshot of 10x6 solution](/backtrack/sol-10x6-N.png)

This is a solution using the techniques of Boolean Satisfiability.

For exquisite detail on how this works, please see the article on my website.

- [Pentomino Packing on fishlet.com](https://fishlet.com/games/pentomino/)

## How it Works

Basically, a Python program is used to generate a CNF file in DIMACS
form which can then be solved by most any SAT solver
implementation. Then the output is parsed and the result displayed.

One interesting thing that came out of it is a Python module sat.py
which makes it very easy to keep track of variables by parameterized
names so the author of a SAT generator doesn't have to deal painfully
with raw variable numbers. It also supports putting in-line comments in
the .cnf file for readability.

## Author

- [Profile](https://github.com/curtmcd "Curt McDowell")
- Email: *coder＠fishlet.com*
- [Website](https://www.fishlet.com)
