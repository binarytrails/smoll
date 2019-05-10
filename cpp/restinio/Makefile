#! @file
#! @author Vsevolod Ivanov

CXX=g++
CXXFLAGS=-Wall -Wextra -Wfatal-errors -pedantic -g

user_controlled_parts:
	${CXX} ${CXXFLAGS} -std=c++14 user_controlled_parts.cpp \
		-I/usr/local/include/restinio -lfmt -lpthread -lhttp_parser \
		-o user_controlled_parts.out

client:
	${CXX} ${CXXFLAGS} -std=c++14 client.cpp \
		-I/usr/local/include/restinio -lfmt -lpthread -lhttp_parser \
		-o client.out
clean:
	rm -f *.out
