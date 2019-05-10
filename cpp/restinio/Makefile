#! @file
#! @author Vsevolod Ivanov

CXX=g++
CXXFLAGS=-Wall -Wextra -Wfatal-errors -pedantic -g

server_reply_by_parts:
	${CXX} ${CXXFLAGS} -std=c++14 server_reply_by_parts.cpp \
		-I/usr/local/include/restinio -lfmt -lpthread -lhttp_parser \
		-o server_reply_by_parts.out

client:
	${CXX} ${CXXFLAGS} -std=c++14 client.cpp \
		-I/usr/local/include/restinio -lfmt -lpthread -lhttp_parser \
		-o client.out
clean:
	rm -f *.out
