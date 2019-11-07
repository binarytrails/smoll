// Vsevolod Ivanov

#include <opendht/crypto.h>
#include <typeinfo>

int main()
{
    auto identity = dht::crypto::generateEcIdentity();

    std::string cert_pem = "-----BEGIN CERTIFICATE-----"
"MIICDjCCAZSgAwIBAgIIS90uAKp+u/swCgYIKoZIzj0EAwMwTDEQMA4GA1UEAxMH"
"ZGh0bm9kZTE4MDYGCgmSJomT8ixkAQETKDBlNDQxZTA4YWJmYTQzYTc3ZTVjZDBm"
"Y2QzMzAzMTc4MjYxMTk0MzIwHhcNMTkxMTA3MDA0MTMwWhcNMjkxMTA0MDA0MTMw"
"WjBMMRAwDgYDVQQDEwdkaHRub2RlMTgwNgYKCZImiZPyLGQBARMoMGU0NDFlMDhh"
"YmZhNDNhNzdlNWNkMGZjZDMzMDMxNzgyNjExOTQzMjB2MBAGByqGSM49AgEGBSuB"
"BAAiA2IABCZ7sBp0Pu+b5yIifoNXchU9crv9won0in++COWynvM4GCLF2Gk6QGhh"
"YLDxNGsyQjGR7z5AGibvYhNLU0JA4RbmxYWHw4g3wBrPA1jm9hGZ8y5Y8R97d0Hl"
"VpyreEMjRKNDMEEwHQYDVR0OBBYEFA5EHgir+kOnflzQ/NMwMXgmEZQyMA8GA1Ud"
"EwEB/wQFMAMBAf8wDwYDVR0PAQH/BAUDAwcGADAKBggqhkjOPQQDAwNoADBlAjEA"
"kKF/6WReNytwSrJ8JSTToc7xWS5SvAa23Fnla4mywZUxUFS4VSxCMQTjQCknO3iZ"
"AjBgxXyXYqn0d7vz7S6oAY5TdaD5YFT5MD2c1MAAp8pxQSwdPa9k0ZSoGIEn31Z0"
"GxU="
"-----END CERTIFICATE-----";
    identity.second = std::make_shared<dht::crypto::Certificate>(cert_pem);

    // extract from certificate
    unsigned char serial[64];
    unsigned long size = sizeof(serial);
    std::cout << "size=" << size << " name=" << typeid(size).name() << std::endl;
    gnutls_x509_crt_get_serial(identity.second->cert, &serial, &size);
    // convert to hex
    std::stringstream ss;
    ss << std::hex;
    std::cout << size << std::endl;
    for (size_t i = 0; i < size; i++)
        ss << std::setw(2) << std::setfill('0') << (int)serial[i];
    auto ss_s = ss.str();

    printf("Certificate:\n%s\n\nUID: %s\nSN:  %s %s true=%s\n",
            identity.second->toString().c_str(),
            identity.second->getUID().c_str(),
            ss_s.c_str(), identity.second->getSerialNumber().c_str());

    return 0;
}
