httpserver_dyad:
	g++ httpserver_dyad.cpp -std=c++11 ../dyad/src/dyad.c -I../dyad/src -I. \
		-o httpserver_dyad.out

httpserver_restinio:
	g++ httpserver_restinio.cpp -std=c++14 \
		-I/usr/local/include/restinio \
		-lfmt -lpthread -lhttp_parser \
		-o httpserver_restinio.out

clean:
	rm -f *.out
