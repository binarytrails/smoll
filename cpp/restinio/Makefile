#! @file
#! @author Vsevolod Ivanov

CXX=g++
CXXFLAGS=-Wall -Wextra -Wfatal-errors -pedantic -g

user_controlled_parts:
	${CXX} ${CXXFLAGS} -std=c++14 \
		user_controlled_parts.cpp -I./user_controlled_parts \
		-I/usr/local/include/restinio -lfmt -lpthread -lhttp_parser \
		-o user_controlled_parts.out

clean:
	rm -f *.out
