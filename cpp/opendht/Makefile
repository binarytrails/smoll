putget:
	g++ putget.cpp -std=c++11 -lopendht -lgnutls -o putget.out

proxy_pistache:
	g++ dhtproxyserver/proxy_pistache.cpp -std=c++11 -I./dhtproxyserver \
		-lpistache -I/usr/include/pistache \
		-lopendht -lgnutls -I/usr/include/opendht \
		-o proxy_pistache.out
clean:
	rm -f *.out
