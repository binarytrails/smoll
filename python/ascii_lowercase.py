#!/usr/bin/python3

import sys
from random import choice
from string import ascii_lowercase

try:
    n = int(sys.argv[1])
except Exception:
    print('./ascii_lowercase.py <number-of-char>')
    sys.exit(1)

s = ''.join(choice(ascii_lowercase) for i in range(n))
o = '_'.join(['ascii_lowercase', str(n)])

with open(o, "w") as f: f.write(s)
print("written to {}".format(o))
