#include <cstdlib>
#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include <stdio.h>
#include <netdb.h>
#include <csignal>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

#define SERVER_TCP_PORT 7000
#define BUFLEN          255

int sd;     // Global Socket Descriptor

void signal_handler(int signum) {
    cout << "\nInterrupt signal (" << signum << ") received.\n";
    close(sd);

   exit(signum);
}

void receive_message(int *sd) {
    int read;
    int toread;
    char *bp;
    char recbuf[BUFLEN];

    bp = recbuf;
    toread = BUFLEN;

    while (1) {
        read = 0;
        while ((read = recv (*sd, bp, toread, 0)) < BUFLEN) {
            bp += read;
            toread -= read;
        }

        cout << "Receive: " <<  recbuf << endl;
        fflush(stdout);
    }
}

int main(int argc, char const *argv[]) {

    struct hostent *hp;
    struct sockaddr_in server;

    string host;
    string message;
    char ip[16];
    char **pptr;
    signal(SIGINT, signal_handler);

    if (argc < 2) {
        cout << "Usage: " << argv[0] << " host" << endl;
        exit(EXIT_FAILURE);
    }

    host = argv[1];

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        cerr << "Failed to create socket" << endl;
        exit(EXIT_FAILURE);
    }


    memset (&server, 0, sizeof(server));
    server.sin_family   = AF_INET;
    server.sin_port     = htons(SERVER_TCP_PORT);

    if ((hp = gethostbyname(host.c_str())) == NULL) {
        cerr << "Unknown server address" << endl;
        exit(EXIT_FAILURE);
    }

    memcpy((char *) &server.sin_addr, hp->h_addr, hp->h_length);

    if (connect (sd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        cerr << "Couldn't connect to server" << endl;
        exit(EXIT_FAILURE);
    }
    pptr = hp->h_addr_list;
    cout << "Connected: " <<
            hp->h_name << " " <<
            inet_ntop(hp->h_addrtype, *pptr, ip, sizeof(ip)) << endl;

    thread receive_thread(receive_message, &sd);

    while (1) {
        getline(cin, message);

        if (message.length() <= BUFLEN) {
            send (sd, message.c_str(), BUFLEN, 0);
        }
    }

}