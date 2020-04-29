#include <openssl/ssl.h>
#include <openssl/err.h>

void configure_context(SSL_CTX* ctx, char* CertFile, char* KeyFile);
void show_certificate(SSL* ssl);