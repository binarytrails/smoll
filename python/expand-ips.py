#!/usr/bin/python3
# @author Vsevolod Ivanov
# @description Expand IPs including subnets to one IP per line.

import sys
import ipaddress

if (len(sys.argv) != 3):
    print('./expand-ips.py <in-file> <out-file>')
    sys.exit(1)

ips = list()

with open(sys.argv[1]) as f:
    for line in f.readlines():
        #print(line.strip())
        try:
            ips = ips + [str(ip) for ip in ipaddress.IPv4Network(line.strip())]
        except Exception as e:
            if ("has host bit set" in str(e)):
                ips = ips + [str(ip) for ip in ipaddress.ip_network(line, False)]

with open(sys.argv[2], 'w+') as f:
    for ip in ips:
        f.write(ip + '\n')
print(ips)
print("Expanded into '"+sys.argv[2]+"'")
