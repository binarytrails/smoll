all:
	mkdir -p ./build

aes-ecb: all
	g++ -std=c++11 -g -Wall -Wextra -Wfatal-errors -pedantic -lcryptopp aes-ecb.cpp -o build/aes-ecb

clean:
	rm -rf ./build
