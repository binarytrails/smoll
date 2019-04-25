putget:
	g++ putget.cpp -std=c++11 -lopendht -lgnutls -o putget.out

dhtproxyserver:
	g++ dhtproxyserver.cpp -std=c++11 -lpistache -I/usr/include/pistache \
		-lopendht -lgnutls -I/usr/include/opendht -I. \
		-o dhtproxyserver.out
clean:
	rm -f *.out
