#!/usr/bin/python

import sys, socket, struct, msgpack

if __name__ == "__main__":

    group = sys.argv[1] if sys.argv[1] else '239.192.0.1' # organization wide
    port = sys.argv[2] if sys.argv[2] else 2222
    source = '' # all
    if (len(sys.argv) > 3):
        source = sys.argv[3]

    print('capturing with group={} port={} source={}'.format(group, port,
        'all 'if source == '' else source))

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind((source, int(port)))
    mreq = struct.pack("4sl", socket.inet_aton(group), socket.INADDR_ANY)
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

    while True:
        data, addr = sock.recvfrom(1024)
        msg = msgpack.unpackb(data)
        print(addr, ": ", msg)

