// Vsevolod Ivanov

#include <stdlib.h>
#include <cstring>
#include <iostream>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <gnutls/gnutls.h>
#include <gnutls/crypto.h>
#include <gnutls/x509.h>
#include <gnutls/ocsp.h>

#include <assert.h>
#include <vector>
#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <sstream>

#include <opendht.h>
#include <opendht/log.h>
#include <opendht/http.h>

#define CHECK(x) assert((x)>=0)
#define LOOP_CHECK(rval, cmd) \
    do { \
        rval = cmd; \
    } while(rval == GNUTLS_E_AGAIN || rval == GNUTLS_E_INTERRUPTED); \
    assert(rval >= 0)

static gnutls_x509_crt_t load_cert(const std::string& cert_file)
{
    printf("Loading certificate: %s\n", cert_file.c_str());
    gnutls_x509_crt_t crt;
    int ret;
    gnutls_datum_t data;
    ret = gnutls_x509_crt_init(&crt);
    if (ret < 0)
        exit(1);
    std::ifstream in;
    in.open(cert_file);
    std::stringstream buffer;
    buffer << in.rdbuf();
    std::string contents = buffer.str();
    std::cout << contents << std::endl;
    data.data = (unsigned char*) contents.c_str();
    data.size = contents.size();
    if (!data.data) {
        printf("Cannot open file: %s\n", cert_file.c_str());
        exit(1);
    }
    ret = gnutls_x509_crt_import(crt, &data, GNUTLS_X509_FMT_PEM);
    if (ret < 0) {
        printf("Cannot import certificate in %s: %s\n", cert_file.c_str(), gnutls_strerror(ret));
        exit(1);
    }
    return crt;
}

static void ocsp_request(gnutls_datum_t * rdata, gnutls_x509_crt_t cert,
                         gnutls_x509_crt_t issuer, gnutls_datum_t *nonce)
{
    gnutls_ocsp_req_t req;
    int ret = gnutls_ocsp_req_init(&req);
    if (ret < 0){
        printf("ocsp_req_init: %s\n", gnutls_strerror(ret));
        return;
    }
    ret = gnutls_ocsp_req_add_cert(req, GNUTLS_DIG_SHA1, issuer, cert);
    if (ret < 0) {
        printf("ocsp_req_add_cert: %s\n", gnutls_strerror(ret));
        return;
    }
    ret = gnutls_ocsp_req_add_cert(req, GNUTLS_DIG_SHA1, issuer, cert);
    if (ret < 0) {
        printf("ocsp_req_add_cert: %s\n", gnutls_strerror(ret));
        return;
    }
    if (nonce) {
        ret = gnutls_ocsp_req_set_nonce(req, 0, nonce);
        if (ret < 0) {
            printf("ocsp_req_set_nonce: %s\n", gnutls_strerror(ret));
            return;
        }
    }
    ret = gnutls_ocsp_req_export(req, rdata);
    if (ret) {
        printf("ocsp_req_export: %s\n", gnutls_strerror(ret));
        return;
    }
}

static int ocsp_response(gnutls_datum_t * data, gnutls_x509_crt_t cert,
                         gnutls_x509_crt_t signer, gnutls_datum_t *nonce)
{
    gnutls_ocsp_resp_t resp;
    int ret;
    unsigned verify;
    gnutls_datum_t rnonce;
    ret = gnutls_ocsp_resp_init(&resp);
    if (ret < 0)
        exit(1);
    ret = gnutls_ocsp_resp_import(resp, data);
    if (ret < 0)
        exit(1);
    ret = gnutls_ocsp_resp_check_crt(resp, 0, cert);
    if (ret < 0)
        exit(1);
    ret = gnutls_ocsp_resp_get_nonce(resp, NULL, &rnonce);
    if (ret < 0)
        exit(1);
    if (rnonce.size != nonce->size || memcmp(nonce->data, rnonce.data,
        nonce->size) != 0) {
        exit(1);
    }
    ret = gnutls_ocsp_resp_verify_direct(resp, signer, &verify, 0);
    if (ret < 0)
        exit(1);
    printf("Verifying OCSP Response: ");
    if (verify == 0)
        printf("Verification success!\n");
    else
        printf("Verification error!\n");
    if (verify & GNUTLS_OCSP_VERIFY_SIGNER_NOT_FOUND)
        printf("Signer cert not found\n");
    if (verify & GNUTLS_OCSP_VERIFY_SIGNER_KEYUSAGE_ERROR)
        printf("Signer cert keyusage error\n");
    if (verify & GNUTLS_OCSP_VERIFY_UNTRUSTED_SIGNER)
        printf("Signer cert is not trusted\n");
    if (verify & GNUTLS_OCSP_VERIFY_INSECURE_ALGORITHM)
        printf("Insecure algorithm\n");
    if (verify & GNUTLS_OCSP_VERIFY_SIGNATURE_FAILURE)
        printf("Signature failure\n");
    if (verify & GNUTLS_OCSP_VERIFY_CERT_NOT_ACTIVATED)
        printf("Signer cert not yet activated\n");
    if (verify & GNUTLS_OCSP_VERIFY_CERT_EXPIRED)
        printf("Signer cert expired\n");
    gnutls_free(rnonce.data);
    gnutls_ocsp_resp_deinit(resp);
    return verify;
}

int main(int argc, char* argv[])
{
    /* -----------------------------------
     * Online Certificate Service Protocol
     * -----------------------------------
     */
    if (argc < 5){
        printf("./binary <cert_file> <issuer_file> <signer_file> <hostname>\n");
        return 1;
    }
    int ret;
    std::string cert_file = argv[1];
    std::string issuer_file = argv[2];
    std::string signer_file = argv[3];
    std::string hostname;
    if (argc > 4)
        hostname = argv[4];
    //else certificate chain gotta go by sequence in gnutls_x509_crt_get_authority_info_access
    printf("cert: %s issuer: %s signer: %s hostname: %s\n",
            cert_file.c_str(), issuer_file.c_str(), signer_file.c_str(), hostname.c_str());

    // Initialize GnuTLS
    CHECK(gnutls_global_init());

    gnutls_x509_crt_t cert, issuer, signer;
    cert = load_cert(cert_file);
    issuer = load_cert(issuer_file);
    signer = load_cert(signer_file);

    // Extracts the Authority Information Access (AIA) extension, see RFC 5280 section 4.2.2.1
    gnutls_datum_t tmp;
    ret = gnutls_x509_crt_get_authority_info_access(cert, 0, GNUTLS_IA_OCSP_URI, &tmp, NULL);
    switch (ret){
        case 0:
            printf("CA issuers OCSP URI: %s\n", tmp.data);
            //gnutls_free(tmp.data);
            break;
        case GNUTLS_E_UNKNOWN_ALGORITHM:
        case GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE:
            printf("Error: NO URI found in certificate\n");
            return 1;
        default:
            printf("Error: %s\n", gnutls_strerror(ret));
            break;
    }

    // Generate OCSP request
    unsigned char noncebuf[23];
    gnutls_datum_t nonce = { noncebuf, sizeof(noncebuf) };
    ret = gnutls_rnd(GNUTLS_RND_NONCE, nonce.data, nonce.size);
    if (ret < 0) {
        printf("gnutls_rnd: %s\n", gnutls_strerror(ret));
        return -1;
    }
    gnutls_datum_t ocsp_req;
    ocsp_request(&ocsp_req, cert, issuer, &nonce);

    std::string ocsp_req_str = reinterpret_cast<char*>(ocsp_req.data);
    printf("OCSP Request: %s\n", ocsp_req_str.c_str());

    // Send OCSP request
    using namespace dht;
    asio::io_context io_context;
    std::shared_ptr<dht::Logger> logger = dht::log::getStdLogger();
    auto request = std::make_shared<http::Request>(io_context, "http://127.0.0.1:8080", logger);

    request->set_method(restinio::http_method_post());
    request->set_header_field(restinio::http_field_t::host, "127.0.0.1");
    request->set_header_field(restinio::http_field_t::user_agent, "OCSP client");
    request->set_header_field(restinio::http_field_t::accept, "*/*");
    request->set_header_field(restinio::http_field_t::content_type, "application/ocsp-request");

    request->set_body(reinterpret_cast<char*>(ocsp_req.data));

    std::weak_ptr<http::Request> wreq = request;
    request->add_on_state_change_callback([logger, &io_context, wreq]
                                          (const http::Request::State state, const http::Response response){
        logger->w("HTTP Request state=%i status_code=%i", state, response.status_code);
        auto request = wreq.lock();
        logger->w("HTTP Request:\n%s", request->to_string().c_str());
        if (state != http::Request::State::DONE)
            return;
        if (response.status_code != 200)
            logger->e("HTTP Request Failed with status_code=%i", response.status_code);
        else
            logger->w("HTTP Request done!");
        request.reset();
        io_context.stop();
    });
    request->send();
    auto work = asio::make_work_guard(io_context);
    io_context.run();

    // Read OCSP response
    //int v = ocsp_response(&ud, cert, signer, &nonce);
    //printf("= verified: %i\n", v);

    gnutls_x509_crt_deinit(cert);
    gnutls_x509_crt_deinit(issuer);
    gnutls_x509_crt_deinit(signer);
    gnutls_global_deinit();

    printf("= success\n");
    return 0;
}
