putget:
	g++ putget.cpp -std=c++11 -lopendht -lgnutls -o putget.out

proxy_pistache:
	g++ dhtproxyserver/proxy_pistache.cpp -std=c++11 -I./dhtproxyserver \
		-lpistache -I/usr/include/pistache \
		-lopendht -lgnutls -I/usr/include/opendht \
		-o proxy_pistache.out

proxy_cpr:
	g++ dhtproxyserver/proxy_cpr.cpp -std=c++11 -I./dhtproxyserver \
		-lcpr -I/usr/include/cpr -lmongoose -I/usr/include \
		-lopendht -lgnutls -I/usr/include/opendht \
		-o proxy_cpr.out
clean:
	rm -f *.out
