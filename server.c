#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <dirent.h>

#include "mySSLTool.h"
#include "myFileTool.h"

#define DEFAULT_PORT 4433

/* server socket setting */
int create_socket(int port)
{
    int s;
    struct sockaddr_in addr;

    bzero(&addr, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }
    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Unable to bind");
        exit(EXIT_FAILURE);
    }
    if (listen(s, 1) < 0) {
        perror("Unable to listen");
        exit(EXIT_FAILURE);
    }
    return s;
}

/* create server context */
SSL_CTX *create_context()
{
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    /* Create new server-method instance */
    method = TLSv1_2_server_method();
    /* Create new context */
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    return ctx;
}

int main(int argc, char **argv)
{
    int sock;
    SSL_CTX *ctx;

    /* Load cryptos, bring in and register error messages */
    SSL_library_init();
    ctx = create_context();
    /* load certificate, key and load ca also set verify */
    configure_context(ctx, "host.crt", "host.key");

    /* create server socket */
    sock = create_socket(DEFAULT_PORT);

    /* Handle connections */
    while(1) {
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        SSL *ssl;

        /* accept client connection */
        int client = accept(sock, (struct sockaddr*)&addr, &len);
        if (client < 0) {
            perror("Unable to accept");
            exit(EXIT_FAILURE);
        }

        /* set ssl */
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client);
        SSL_set_verify_depth(ssl, 1);

        if (SSL_accept(ssl) < 0) {
            ERR_print_errors_fp(stderr);
        }
        else {
            char buf[1024];
            int bytes;
            char* reply;

            /* print connection information */
            printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
            show_certificate(ssl);

            /* get message & decrypt & print */
            bytes = SSL_read(ssl, buf, sizeof(buf)); 
            buf[bytes] = 0;
            printf("Received: \"%s\"\n", buf);

            /* reply storage file list */
            if (!strcmp(buf, "list")) {
                reply = getFileList();
            }
            /* reply file */
            else if (!strncmp(buf, "getf ", strlen("getf "))) {
                char path[1024] = "storage/";
                strcat(path, buf + strlen("getf "));
                reply = readFile(path, &bytes);
            }
            /* reply remove file status */
            else if (!strncmp(buf, "delf ", strlen("delf "))) {
                reply = deleteFile(buf + strlen("delf "));
            }
            /* reply server command output */
            else if (!strncmp(buf, "cmd  ", strlen("cmd "))) {
                strcat(buf, " > output.txt 2>&1");
                system(buf + strlen("cmd "));
                reply = readFile("output.txt", &bytes);
                system("rm output.txt");
            }
            /* reply echo from client */
            else if (!strncmp(buf, "echo  ", strlen("echo "))) {
                reply = buf + strlen("echo ");
            } 
            /* reply error message */
            else {
                char* notFoundMsg = "Command Not Found.\n";
                reply = notFoundMsg;
            }
            /* reply to client */
            SSL_write(ssl, reply, strlen(reply));
        }

        /* close client connection */
        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(client);
    }

    /* close server */
    close(sock);
    SSL_CTX_free(ctx);

    return 0;
}