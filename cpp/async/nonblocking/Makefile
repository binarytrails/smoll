#! @file
#! @author Vsevolod Ivanov

CXX=g++
CXXFLAGS=-Wall -Wextra -Wfatal-errors -pedantic -g -lpthread

server:
	${CXX} ${CXXFLAGS} -std=c++11 server.cpp -o server.out

client:
	${CXX} ${CXXFLAGS} -std=c++14 client.cpp -o client.out

client2:
	${CXX} ${CXXFLAGS} -std=c++14 client2.cpp -o client2.out

clean:
	rm -f *.out
