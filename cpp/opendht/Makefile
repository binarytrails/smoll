#! @file
#! @author Vsevolod Ivanov

CXX=g++
CXXFLAGS=-Wall -Wextra -Wfatal-errors -pedantic -g \
		-lopendht -lgnutls -pthread -ljsoncpp -I/usr/include/opendht \
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
		-I/usr/local/include/restinio -lfmt -lhttp_parser \
		-o proxy_restinio.out

dhtclient:
	${CXX} ${CXXFLAGS} -std=c++14 dhtclient.cpp -o dhtclient.out

dhtclient_async:
	${CXX} ${CXXFLAGS} -std=c++14 dhtclient_async.cpp -o dhtclient_async.out

proxy_runner:
	${CXX} ${CXXFLAGS} -std=c++14 proxy_runner.cpp -o proxy_runner.out

future_get:
	${CXX} ${CXXFLAGS} -std=c++14 future-get.cpp -o future-get.out

http:
	${CXX} ${CXXFLAGS} -lhttp_parser -std=c++14 http.cpp -o http.out

clean:
	rm -f *.out
