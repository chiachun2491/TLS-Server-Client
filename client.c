#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "mySSLTool.h"
#include "myFileTool.h"

/* client socket setting */
int create_socket(const char *hostname, int port)
{   
    int s;
    struct hostent *host;
    struct sockaddr_in addr;

    bzero(&addr, sizeof(addr));

    if ((host = gethostbyname(hostname)) == NULL) {
        perror("Unable to get host");
        exit(EXIT_FAILURE);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = *(long*)(host->h_addr);

    s = socket(PF_INET, SOCK_STREAM, 0);
    /* connect to server */
    if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(s);
        perror("Unable to connect to host");
        exit(EXIT_FAILURE);
    }
    return s;
}

/* create client context */
SSL_CTX* create_context(void)
{   
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    /* Create new client-method instance */
    method = TLSv1_2_client_method();  
    /* Create new context */
    ctx = SSL_CTX_new(method);   
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    return ctx;
}

int main(int argc, char *argv[])
{   
    char *hostname, *portnum;
    char send[1024];
    char* help = "Command List\n \
    \nRemote File copy\n \
    \tlist \t Show file list\n \
    \tgetf \t Get file from server\n \
    \tdelf \t Delete file from server\n \
    \nRemote shell\n \
    \tcmd \t Send command to server\n \
    \nMessage communication\n \
    \techo \t Send message and recieve echo from server\n \
    \nOther\n \
    \thelp \t Show command list\n";

    if (argc != 3) {
        printf("usage: %s <hostname> <portnum>\n", argv[0]);
        exit(0);
    }

    hostname = argv[1];
    portnum = argv[2];

    /* Load cryptos, bring in and register error messages */
    SSL_library_init(); 

    /* print command list */
    printf("%s", help);
    printf("> ");

    while (gets(send) != NULL) {
        SSL_CTX *ctx;
        SSL *ssl;

        /* connect to server */
        int server = create_socket(hostname, atoi(portnum));

        /* create context and load key, certificate and ca also set verify */
        ctx = create_context();
        configure_context(ctx, "client.crt", "client.key"); /* load certs */

        /* set ssl */
        ssl = SSL_new(ctx);      
        SSL_set_fd(ssl, server);    
        SSL_set_verify_depth(ssl, 1);
        
        if (SSL_connect(ssl) < 0) {
            ERR_print_errors_fp(stderr);
        }
        else {
            char buf[1024];
            int bytes;
            char* recieved;

            /* print connection information */
            printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
            show_certificate(ssl);      

            /* encrypt & send message */
            SSL_write(ssl, send, strlen(send));   

            /* get reply & decrypt & save to file*/
            while ((bytes = SSL_read(ssl, buf, sizeof(buf))) != 0) {
                if (bytes < 1024) {
                    buf[bytes] = 0;
                }
                writeFile(buf, "received", bytes);
            }

            /* read reply message to buffer */
            recieved = readFile("received", &bytes);
            system("rm received");

            /* save remote file */
            if (!strncmp(send, "getf ", strlen("getf "))) {
                char path[1024] = "downloads/";
                strcat(path, send + strlen("getf "));
                writeFile(recieved, path, bytes);
                printf("Downloaded File: \"%s\"\n", path);
            }
            else {
                /* print command list */
                if (!strcmp(send, "help")) {
                    printf("%s", help);
                }
                /* print server reply message */
                else {
                    printf("Received: \"%s\"\n", recieved);
                }
            }

            /* close server connection */
            SSL_shutdown(ssl);
            SSL_free(ssl);
        }

        /* close socket */
        close(server);         
        SSL_CTX_free(ctx);
        printf("> ");
    }

    return 0;
}