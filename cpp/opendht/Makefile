putget:
	g++ putget.cpp -std=c++11 -lopendht -lgnutls -o putget.out

proxyserv_dyad:
	g++ proxyserv_dyad.cpp -std=c++11 ../dyad/dyad.c -I../dyad \
		-lopendht -lgnutls -I/usr/include/opendht -I. \
		-o proxyserv_dyad.out

clean:
	rm -f *.out
