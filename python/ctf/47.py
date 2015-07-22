#!/usr/bin/python2.7

import os

print """
Make 705 using 2 3 5 10 30 one time each.
Using the - + * operators.
No division allowed.
"""

print "705 should be made from one of these:"
print "---------------------------"
for factor in range(1, 47):
	if 705.00 / factor % 1 == 0:
		print str(factor) + " x " + str(705 / factor)
print "---------------------------"

yes_no = raw_input("Do you want the solution? ('yes'): ")

if yes_no == "yes":
	print "\nThe password to solution is \x1B[31m l33t \x1B[0m!\n"

	os.system("""
	echo "-----BEGIN PGP MESSAGE-----
	Version: GnuPG v2

	jA0EBwMCuDhynY+1/g7k0kQBT/PDaiHlObU2uKnkH/Ra7qlFsuh7m1OXWynsjafv
	C1c2Xqp78BzxWWIIEn8GvYYoisssBlhhfFD/MRtVFxmAD+nBFQ==
	=5HM7
	-----END PGP MESSAGE-----
	" | gpg -d
	""")

	print "\n\x1B[31m Let's keep it a secret \x1B[31m0"
