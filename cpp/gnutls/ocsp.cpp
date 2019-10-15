// Vsevolod Ivanov

#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <gnutls/gnutls.h>
#include <gnutls/x509.h>
#include <assert.h>

#define CHECK(x) assert((x)>=0)
#define LOOP_CHECK(rval, cmd) \
    do { \
        rval = cmd; \
    } while(rval == GNUTLS_E_AGAIN || rval == GNUTLS_E_INTERRUPTED); \
    assert(rval >= 0)

int main(int argc, char* argv[])
{
    if (argc < 5){
        printf("./binary <host> <port> <ca.crt> <ca_name>\n");
        return 1;
    }
    int port = atoi(argv[2]);
    std::string host = argv[1];
    std::string ca_name = argv[4];
    std::string server_ca = argv[3];
    printf("host: %s  port: %u  ca: %s ca_name: %s\n", host.c_str(), port,
                                                       server_ca.c_str(), ca_name.c_str());
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
    // Initialize GnuTLS
    CHECK(gnutls_global_init());

    int ret;
    gnutls_datum_t out;
    gnutls_session_t session;

    // Load the trusted CA certificates.
    gnutls_certificate_credentials_t cred = NULL;
    CHECK(gnutls_certificate_allocate_credentials (&cred));
    CHECK(gnutls_certificate_set_x509_system_trust(cred));
    CHECK(gnutls_certificate_set_x509_trust_file(cred, server_ca.c_str(), GNUTLS_X509_FMT_PEM));

    // Create the session object.
    CHECK(gnutls_init(&session, GNUTLS_CLIENT));
    CHECK(gnutls_server_name_set(session, GNUTLS_NAME_DNS, host.c_str(), host.size()));
    CHECK(gnutls_set_default_priority(session));

    // Install the trusted certificates.
    CHECK(gnutls_credentials_set(session, GNUTLS_CRD_CERTIFICATE, cred));
    // NOTE deal breaker
    gnutls_session_set_verify_cert(session, ca_name.c_str(), 0);

    // Establish the session.
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        printf("error: connection failed\n");
        return 1;
    }
    // Associate the socket with the session object and set the server name.
    gnutls_transport_set_ptr(session, (gnutls_transport_ptr_t)(uintptr_t)sockfd);
    //gnutls_transport_set_int(session, sockfd);
    gnutls_handshake_set_timeout(session, GNUTLS_DEFAULT_HANDSHAKE_TIMEOUT);

    // Perform the handshake
    do {
        ret = gnutls_handshake(session);
    }
    while (ret < 0 && gnutls_error_is_fatal(ret) == 0);
    if (ret < 0) {
        if (ret == GNUTLS_E_CERTIFICATE_VERIFICATION_ERROR){
            /* check certificate verification status */
            auto type = gnutls_certificate_type_get(session);
            int status = gnutls_session_get_verify_cert_status(session);
            CHECK(gnutls_certificate_verification_status_print(status, type, &out, 0));
            printf("cert verify output: %s\n", out.data);
            gnutls_free(out.data);
        }
        printf("error: handshake failed: %s\n", gnutls_strerror(ret));
    }
    else {
        // Display session description
        auto desc = gnutls_session_get_desc(session);
        printf("gnutls session info: %s\n", desc);
        gnutls_free(desc);
        // Safety net
        printf("tls false start: %s\n", (gnutls_session_get_flags(session) &
                                         GNUTLS_SFLAGS_FALSE_START) ? "on" : "off");
    }
    gnutls_credentials_type_t c = gnutls_auth_get_type(session);
    switch (c) {
    case GNUTLS_CRD_CERTIFICATE:
        //print_x509_certificate_info(session);
        printf("todo print x509 info\n");
        break;
    default:
		if ((int) c == -1)
			printf(("Transport authentication failure\n"));
		else
			printf(("Unsupported credential type %d.\n"), (int) c);
        break;
    }

    /* TODO
     *  Online Certificate Service Protocol
     */

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
