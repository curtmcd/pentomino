import sys
import os
import re
import subprocess
from datetime import datetime

CNF_FILE = 'out.cnf'

TYP_TITLE = 0
TYP_COMMENT = 1
TYP_CLAUSE = 2

# Had luck with Chaff II version (zchaff.2004.5.13.tar.gz) but not zchaff64
ZCHAFF = './zChaff/zchaff'

# Run zchaff SAT solver, passing it the input file and parsing the output to a
# list of positive and negative integers representing the solution variables.
def solve_zchaff(fname):
    p = subprocess.run([ZCHAFF, fname], capture_output=True)
    if p.returncode != 0:
        print('Error code %d running %s' % (p.returncode, ZCHAFF))
        sys.exit(3)

    os.remove('resolve_trace')          # side effect log file

    output = p.stdout.decode()
    m = re.match(r'.*\nInstance Satisfiable\n([-0-9 ]*)', output, re.DOTALL)
    if not m:
        print('Solution not found', file=sys.stderr)
        sys.exit(1)

    sol_str = m.group(1)
    var_strs = sol_str.split()
    return [int(v) for v in var_strs]

class CNF:
    class Entry:
        def __init__(self, typ, data):
            self.typ = typ
            self.data = data
        def __str__(self):
            if self.typ == TYP_COMMENT:
                return 'c %s' % self.data
            elif self.typ == TYP_CLAUSE:
                return ' '.join([str(v) for v in self.data]) + ' 0'

    def __init__(self):
        self.vars = {}
        self.var_count = 0
        self.entries = []
        self.clause_count = 0

    # Each variable has a "description" comprising what must be
    # multiple arbitrary hashable objects
    def var(self, *hashables):
        key = tuple(hashables)
        if key not in self.vars:
            self.var_count = self.var_count + 1
            self.vars[key] = self.var_count
        return self.vars[key]

    def clause(self, disj_lits):
        self.entries.append(self.Entry(TYP_CLAUSE, disj_lits))
        self.clause_count += 1

    def comment(self, msg):
        self.entries.append(self.Entry(TYP_COMMENT, msg))

    # Write CNF file in DIMACS format
    def write_cnf(self, fname):
        datestamp = datetime.now().astimezone().strftime("%d/%m/%Y %H:%M:%S %Z")
        with open(fname, 'w') as fh:
            print('c Command: ' + ' '.join(sys.argv),
                  file=fh)
            print('c Date: ' + datestamp,
                  file=fh)
            print('p cnf %d %d' % (self.var_count, self.clause_count),
                  file=fh)
            for entry in self.entries:
                print(str(entry),
                      file=fh)
            print('c Variables:',
                  file=fh)
            for k, v in self.vars.items():
                print('c %d = %s' % (v, str(k)),
                      file=fh)

    def solve(self, tool='zchaff'):
        assert tool == 'zchaff'
        self.write_cnf(CNF_FILE)
        return solve_zchaff(CNF_FILE)
