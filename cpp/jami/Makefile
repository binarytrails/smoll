#! @file
#! @author Vsevolod Ivanov

CXX=g++
CXXFLAGS=-Wall -Wextra -Wfatal-errors -pedantic -g \
		-lopendht -lgnutls -lpthread -pthread \
		-ljsoncpp -DOPENDHT_JSONCPP \
		-I/usr/include/opendht \
		-I/usr/include/dring -lring \
		-I/home/n0t/jami/ring-daemon/src

namedir:
	${CXX} ${CXXFLAGS} -std=c++11 namedir.cpp -o namedir.out

clean:
	rm -f *.out
