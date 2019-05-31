#! @file
#! @author Vsevolod Ivanov

CXX=g++
CXXFLAGS=-Wall -Wextra -Wfatal-errors -pedantic -g -lpthread

server:
	${CXX} ${CXXFLAGS} -std=c++11 server.cpp -o server.out

client:
	${CXX} ${CXXFLAGS} -std=c++14 client.cpp -o client.out

cbs_client:
	${CXX} ${CXXFLAGS} -std=c++14 cbs_client.cpp -o cbs_client.out

clean:
	rm -f *.out
