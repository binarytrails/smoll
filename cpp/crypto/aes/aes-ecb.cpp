// @author Vsevolod Ivanov <seva@binarytrails.net>

#include <iostream>

#include <cryptopp/aes.h>
#include <cryptopp/hex.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>

using namespace CryptoPP;

int main(int argc, char *argv[])
{
    AutoSeededRandomPool prng;
    SecByteBlock key(AES::DEFAULT_KEYLENGTH);
    prng.GenerateBlock(key, key.size());

    std::string plain = "hello crypto world!";
    std::string cipher, encrypted, decrypted;

    try // encrypt plain text with aes ecb
    {
        std::cout << "plain text: " << plain << std::endl;
        ECB_Mode<AES>::Encryption e;
        e.SetKey(key, key.size());
        // STF adds padding as required: ecb & cbc mode must be padded to block size of cipher
        StringSource ss1(plain, true, new StreamTransformationFilter(e, new StringSink(cipher)));
    }
    catch(CryptoPP::Exception& e)
    {
        std::cerr << e.what() << std::endl;
        exit(1);
    }
    // pretty print cipher text in hex
    StringSource ss2(cipher, true, new HexEncoder(new StringSink(encrypted)));
    std::cout << "cipher text: " << encrypted << std::endl;

    try // decrypt aes ecb cipher text
    {
        ECB_Mode<AES>::Decryption d;
        d.SetKey(key, key.size());  // ecb mode does not use an iv
        // STF removes padding as required
        StringSource ss3(cipher, true, new StreamTransformationFilter(d, new StringSink(decrypted)));
        std::cout << "decrypted text: " << decrypted << std::endl;
    }
    catch(CryptoPP::Exception& e)
    {
        std::cerr << e.what() << std::endl;
        exit(1);
    }
    return 0;
}
