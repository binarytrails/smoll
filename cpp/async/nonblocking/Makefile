#! @file
#! @author Vsevolod Ivanov

CXX=g++
CXXFLAGS=-Wall -Wextra -Wfatal-errors -pedantic -g -lpthread

server:
	${CXX} ${CXXFLAGS} -std=c++11 server.cpp -o server.out

client:
	${CXX} ${CXXFLAGS} -std=c++11 client.cpp -o client.out

clean:
	rm -f *.out
