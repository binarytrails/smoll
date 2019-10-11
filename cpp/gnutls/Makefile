#! @file
#! @author Vsevolod Ivanov

CXX=g++
CXXFLAGS=-Wall -Wextra -Wfatal-errors -pedantic -g -lgnutls

ocsp:
	${CXX} ${CXXFLAGS} -std=c++11 ocsp.cpp -o ocsp.out

clean:
	rm -f *.out
