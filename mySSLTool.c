#include <stdio.h>
#include "mySSLTool.h"

void configure_context(SSL_CTX* ctx, char* CertFile, char* KeyFile) {
    SSL_CTX_set_ecdh_auto(ctx, 1);

    /* Set certificate */
    if (SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
	    exit(EXIT_FAILURE);
    }
    /* Set private key */
    if (SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0 ) {
        ERR_print_errors_fp(stderr);
	    exit(EXIT_FAILURE);
    }
    /* Verify private key */
    if (!SSL_CTX_check_private_key(ctx)) {
        perror("Private key does not match the public certificate");
        exit(EXIT_FAILURE);
    }
    /* Load CA certificate */
    SSL_CTX_load_verify_locations(ctx, "ca.crt", NULL);
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
}

void show_certificate(SSL* ssl) {
    X509 *cert;
    char *line;

    /* Get the remote's certificate */
    cert = SSL_get_peer_certificate(ssl); 
    if (cert != NULL) {
        printf("Remote certificates:\n");

        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);

        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);

        X509_free(cert);
    }
    else {
        printf("Info: No remote certificates configured.\n");
    }
}