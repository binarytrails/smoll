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

client_custom_http_methods:
	${CXX} ${CXXFLAGS} -std=c++14 client_custom_http_methods.cpp \
		-I/usr/include/restinio -lfmt -lpthread -lhttp_parser \
		-o client_custom_http_methods.out

clean:
	rm -f *.out
