#! @file
#! @author Vsevolod Ivanov

CXX=g++
CXXFLAGS=-Wall -Wextra -Wfatal-errors -pedantic -g -lgnutls

valid_ca:
	${CXX} ${CXXFLAGS} -std=c++11 valid_ca.cpp -o valid_ca.out

ocsp:
	${CXX} ${CXXFLAGS} -std=c++11 ocsp.cpp -o ocsp.out

clean:
	rm -f *.out
