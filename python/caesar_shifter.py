#!/usr/bin/python

encoded = "rdqtajqdmtwxjwzssnslymwtzlmymjknjqibmjwjfwjdtzltnslbnymdtzwgnlf"
shiftUpTo = 25; d = {i: '' for i in range(shiftUpTo)}
for group in [(i, chr(ord(j)-i)) for i in range(shiftUpTo) for j in encoded]: d[group[0]] += str(group[1])

print('up shift : result')
print('-----------------')
for s in d: print('%s : %s' % (str(s),d[s]))
print('Look for the running horse...')
