#! @file
#! @author Vsevolod Ivanov

.PHONY: all ocsp clean

CXX=g++
CXXFLAGS=-Wall -Wextra -Wfatal-errors -pedantic -g -lgnutls

valid_ca:
	${CXX} ${CXXFLAGS} -std=c++11 valid_ca.cpp -o valid_ca.out

ocsp:
	${CXX} ${CXXFLAGS} -std=c++14 ocsp.cpp -o ocsp.out \
		-lopendht -lssl -lcrypto -lpthread -DASIO_STANDALONE

clean:
	rm -f *.out
