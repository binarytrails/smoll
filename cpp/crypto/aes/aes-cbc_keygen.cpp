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
    std::string cipher, encoded, recovered;

    // key
    // generated key TODO custom plain and pad
    byte key[AES::DEFAULT_KEYLENGTH];
    prng.GenerateBlock(key, sizeof(key));
    // pretty print
    encoded.clear();
    StringSource(key, sizeof(key), true, new HexEncoder(new StringSink(encoded)));
    std::cout << "key: " << sizeof(key) << " " << encoded << std::endl;

    // iv
    byte iv[AES::BLOCKSIZE];
    prng.GenerateBlock(iv, sizeof(iv));
    // pretty print
    encoded.clear();
    StringSource(iv, sizeof(iv), true, new HexEncoder(new StringSink(encoded)));
    std::cout << "iv: " << sizeof(iv) << " " << encoded << std::endl;

    // plain text
    std::string plain = "hello crypto world!";
    std::cout << "plain: " << plain << std::endl;

    try // encrypt
    {
        CBC_Mode<AES>::Encryption e;
        e.SetKeyWithIV(key, sizeof(key), iv);
        StringSource s(plain, true, new StreamTransformationFilter(e, new StringSink(cipher)));
/*
        StreamTransformationFilter filter(e);
        filter.Put((const byte*)plain.data(), plain.size());
        filter.MessageEnd();

        const size_t ret = filter.MaxRetrievable();
        cipher.resize(ret);
        filter.Get((byte*)cipher.data(), cipher.size());
*/
    }
    catch(CryptoPP::Exception& e)
    {
        std::cerr << e.what() << std::endl;
        exit(1);
    }
    // pretty print
    encoded.clear();
    StringSource s(cipher, true, new HexEncoder(new StringSink(encoded)));
    std::cout << "cipher text: " << encoded << std::endl;

    try // decryption
    {
        CBC_Mode<AES>::Decryption d;
        d.SetKeyWithIV(key, sizeof(key), iv);
        StringSource s(cipher, true, new StreamTransformationFilter(d, new StringSink(recovered)));
/*
        StreamTransformationFilter filter(d);
        filter.Put((const byte*)cipher.data(), cipher.size());
        filter.MessageEnd();

        const size_t ret = filter.MaxRetrievable();
        recovered.resize(ret);
        filter.Get((byte*)recovered.data(), recovered.size());
*/
        std::cout << "recovered: " << recovered << std::endl;
    }
    catch(CryptoPP::Exception& e)
    {
        std::cerr << e.what() << std::endl;
        exit(1);
    }
    return 0;
}
