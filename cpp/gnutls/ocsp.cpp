// Vsevolod Ivanov

#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <gnutls/gnutls.h>
#include <assert.h>

#define LOOP_CHECK(rval, cmd) \
    do { \
        rval = cmd; \
    } while(rval == GNUTLS_E_AGAIN || rval == GNUTLS_E_INTERRUPTED); \
    assert(rval >= 0)

int main()
{
    int port = 8080;
    std::string host = "127.0.0.1";
    std::string server_ca = "ca.crt";

    // Create a socket file descriptor
    int sockfd = 0;
    struct sockaddr_in serv_addr;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("error: socket creation error\n");
        return 1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, host.c_str(), &serv_addr.sin_addr) <= 0){
        printf("error: invalid address not supported\n");
        return 1;
    }
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){ 
        printf("error: connection failed\n"); 
        return 1; 
    }

    // Load the trusted CA certificates.
    gnutls_certificate_credentials_t cred = NULL;
    int ret = gnutls_certificate_allocate_credentials (&cred);
    if (ret != GNUTLS_E_SUCCESS){
        printf("error: gnutls_certificate_allocate_credentials: %s\n",gnutls_strerror(ret));
        return 1;
    }
    // gnutls_certificate_set_x509_system_trust needs GNUTLS version 3.0
    // or newer, so we hard-code the path to the certificate store
    // instead.
    ret = gnutls_certificate_set_x509_trust_file(cred, server_ca.c_str(), GNUTLS_X509_FMT_PEM);
    if (ret == 0){
        printf("error: no certificates found in: %s\n", server_ca.c_str());
        return 1;
    }
    if (ret < 0){
        printf("error: gnutls_certificate_set_x509_trust_files(%s): %s\n",
               server_ca.c_str(), gnutls_strerror(ret));
        return 1;
    }
    // Create the session object.
    gnutls_session_t session;
    ret = gnutls_init(&session, GNUTLS_CLIENT);
    if (ret != GNUTLS_E_SUCCESS){
        printf("error: gnutls_init: %s\n", gnutls_strerror(ret));
        return 1;
    }
    // Configure the cipher preferences.
    const char *errptr = NULL;
    ret = gnutls_priority_set_direct(session, "NORMAL", &errptr);
    if (ret != GNUTLS_E_SUCCESS){
        printf("error: gnutls_priority_set_direct: %s\nerror: at: \"%s\"\n",
        gnutls_strerror(ret), errptr);
        return 1;
    }
    // Install the trusted certificates.
    ret = gnutls_credentials_set(session, GNUTLS_CRD_CERTIFICATE, cred);
    if (ret != GNUTLS_E_SUCCESS){
        printf("error: gnutls_credentials_set: %s\n", gnutls_strerror(ret));
        return 1;
    }
    // Associate the socket with the session object and set the server name.
    gnutls_transport_set_ptr(session, (gnutls_transport_ptr_t)(uintptr_t)sockfd);
    ret = gnutls_server_name_set(session, GNUTLS_NAME_DNS, host.c_str(), host.size());
    if (ret != GNUTLS_E_SUCCESS){
        printf("error: gnutls_server_name_set: %s\n",
        gnutls_strerror(ret));
        return 1;
    }
    // Establish the session.
    ret = gnutls_handshake(session);
    if (ret != GNUTLS_E_SUCCESS){
        printf("error: gnutls_handshake: %s\n", gnutls_strerror(ret));
        return 1;
    }
    // Obtain the server certificate chain.  The server certificate
    // itself is stored in the first element of the array.
    unsigned certslen = 0;
    const gnutls_datum_t *const certs =
    gnutls_certificate_get_peers(session, &certslen);
    if (certs == NULL || certslen == 0){
        printf("error: could not obtain peer certificate\n");
        return 1;
    }
    // Validate the certificate chain.
    unsigned status = (unsigned) -1;
    ret = gnutls_certificate_verify_peers2(session, &status);
    if (ret != GNUTLS_E_SUCCESS){
        printf("error: gnutls_certificate_verify_peers2: %s\n",
        gnutls_strerror(ret));
        return 1;
    }

    // Try sending and receiving some data
    char buf[4096];
    snprintf(buf, sizeof(buf), "GET / HTTP/1.1\r\nHost: %s\r\n\r\n", host.c_str());
    LOOP_CHECK(ret, gnutls_record_send(session, buf, strlen(buf)));
    if (ret < 0){
        printf("send failed: %s\n", gnutls_strerror(ret));
        return 1;
    }
    LOOP_CHECK(ret, gnutls_record_recv(session, buf, sizeof(buf)));
    if (ret < 0){
        printf("recv failed: %s\n", gnutls_strerror(ret));
        return 1;
    }
    else if (ret > 0){
        printf("< received %d bytes:\n", ret);
        for (int ii = 0; ii < ret; ii++) {
            fputc(buf[ii], stdout);
        }
        fputs("\n", stdout);
    }
    // Finish gracefully
    if (session){
        gnutls_bye(session, GNUTLS_SHUT_RDWR);
        gnutls_deinit(session);
    }
    if (cred)
        gnutls_certificate_free_credentials(cred);
    close(sockfd);
    printf("= success\n");
    return 0;
}
