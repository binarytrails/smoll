#! @file
#! @author Vsevolod Ivanov

CXX=g++
CXXFLAGS=-Wall -Wextra -Wfatal-errors -pedantic -g \
		-lopendht -lgnutls -ljsoncpp -I/usr/include/opendht \
		-DOPENDHT_JSONCPP

putget:
	${CXX} ${CXXFLAGS} -std=c++11 putget.cpp -o putget.out

proxy_pistache:
	${CXX} ${CXXFLAGS} -std=c++11 \
		dhtproxyserver/proxy_pistache.cpp -I./dhtproxyserver \
		-lpistache -I/usr/include/pistache \
		-o proxy_pistache.out

proxy_cpr:
	${CXX} ${CXXFLAGS} -std=c++11 \
		dhtproxyserver/proxy_cpr.cpp -I./dhtproxyserver \
		-lcpr -I/usr/include/cpr -lmongoose -I/usr/include \
		-o proxy_cpr.out

proxy_restinio:
	${CXX} ${CXXFLAGS} -std=c++14 \
		dhtproxyserver/proxy_restinio.cpp -I./dhtproxyserver \
		-I/usr/local/include/restinio -lfmt -lpthread -lhttp_parser \
		-o proxy_restinio.out

dhtclient:
	${CXX} ${CXXFLAGS} -std=c++14 dhtclient.cpp -lpthread -o dhtclient.out

dhtclient_async:
	${CXX} ${CXXFLAGS} -std=c++14 dhtclient_async.cpp -lpthread -o dhtclient_async.out

clean:
	rm -f *.out
