#include <iostream>
#include <cstring>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

#define SERVER_TCP_PORT 7000
#define BUFLEN          511
#define LISTENQ         5

int main(int argc, char const *argv[]) {

    int i;
    int read;
    int toread;
    char *bp;
    char recbuf[BUFLEN];

    int listen_sd;
    int accept_sd;
    int sock_fd;
    int maxfd;
    int maxi;
    int nready;
    int client[FD_SETSIZE];
    struct sockaddr_in server;
    struct sockaddr_in client_addr;
    socklen_t client_len;
    fd_set rset;
    fd_set allset;

    if ((listen_sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        cerr << "Error creating listen socket" << endl;
        exit(EXIT_FAILURE);
    }

    int arg = 1; // to make compiler happy

    if (setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, &arg, sizeof(arg)) == -1) {
        cerr << "Error setting socket options" << endl;
        exit(EXIT_FAILURE);
    }

    memset (&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_TCP_PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listen_sd, (struct sockaddr *) &server, sizeof(server)) == -1) {
        cerr << "Error binding socket" << endl;
        exit(EXIT_FAILURE);
    }

    listen(listen_sd, LISTENQ);
    maxfd = listen_sd;
    maxi = -1;

    for (i = 0; i < FD_SETSIZE; i++) {
        client[i] = -1;
    }

    FD_ZERO(&allset);
    FD_SET(listen_sd, &allset);

    while (1) {
        int sender;
        bool disconnect;
        rset = allset;
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);

        // new client connection
        if (FD_ISSET(listen_sd, &rset)) {
            client_len = sizeof(client_addr);

            if ((accept_sd = accept(listen_sd, (struct sockaddr *) &client_addr, &client_len)) == -1) {
                cerr << "Error accepting new connection" << endl;
                exit(EXIT_FAILURE);
            }

            cout << "New Connection: " << inet_ntoa(client_addr.sin_addr) << endl;

            // update client array with newly connected socket
            for (i = 0; i < FD_SETSIZE; i++) {
                if (client[i] < 0) {
                    client[i] = accept_sd;
                    break;
                }
            }

            if (i == FD_SETSIZE) {
                cerr << "Too many clients" << endl;
                exit(EXIT_FAILURE);
            }

            FD_SET(accept_sd, &allset);

            if (accept_sd > maxfd) {
                maxfd = accept_sd;
            }

            if (i > maxi) {
                maxi = i;
            }

            if (--nready <= 0) {
                continue;
            }
        }

        // check all clients for new messages
        for (i = 0; i <= maxi; i++) {
            disconnect = false;
            if ((sock_fd = client[i]) < 0) {
                continue;
            }

            // if the file descriptor is set
            if (FD_ISSET(sock_fd, &rset)) {
                sender = i;

                bp = recbuf;
                toread = BUFLEN;

                // read in the message
                while ((read = recv(sock_fd, bp, toread, 0)) < BUFLEN) {
                    bp += read;
                    toread -= read;
                    // connection closed by client
                    if (read == 0) {
                        cout << "Remote Address: " << inet_ntoa(client_addr.sin_addr) << " closed connection." << endl;
                        close(sock_fd);
                        FD_CLR(sock_fd, &allset);
                        client[i] = -1;
                        disconnect = true;
                        break;
                    }
                }

                if (!disconnect) {
                    cout << recbuf << endl;

                    // echo recieve message to all clients (excluding sender)
                    for (int j = 0; j <= maxi; j++) {
                        if ((sock_fd = client[j]) < 0) {
                            continue;
                        }

                        if (j != sender) {
                            write(sock_fd, recbuf, BUFLEN);
                        }
                    }
                }
            }

            if (--nready <= 0) {
                continue;
            }
        }
    }
}