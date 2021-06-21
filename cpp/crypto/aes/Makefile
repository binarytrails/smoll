all:
	mkdir -p ./build

aes-ecb_keygen: all
	g++ -std=c++11 -g -Wall -Wextra -Wfatal-errors -pedantic -lcryptopp aes-ecb_keygen.cpp -o build/aes-ecb_keygen

aes-cbc_keygen: all
	g++ -std=c++11 -g -Wall -Wextra -Wfatal-errors -pedantic -lcryptopp aes-cbc_keygen.cpp -o build/aes-cbc_keygen

aes-cbc_keyplain: all
	g++ -std=c++11 -g -Wall -Wextra -Wfatal-errors -pedantic -lcryptopp aes-cbc_keyplain.cpp -o build/aes-cbc_keyplain

clean:
	rm -rf ./build
