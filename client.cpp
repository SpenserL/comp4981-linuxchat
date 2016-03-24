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
#define BUFLEN          511

int sd;     // Global Socket Descriptor

void signal_handler(int signum) {
    cout << "\nInterrupt signal (" << signum << ") received.\n";
    close(sd);

   exit(signum);
}

void receive_message() {
    int read;
    int toread;
    char *bp;
    char recbuf[BUFLEN];
    bp = recbuf;
    toread = BUFLEN;

    while (1) {
        read = 0;
        while ((read = recv (sd, bp, toread, 0)) < BUFLEN) {
            bp += read;
            toread -= read;
            if(read==0){
                cout<<"Server ended... exiting client"<<endl;
                exit(1);

            }
        }

        cout << recbuf << endl;
        fflush(stdout);
    }
}

int main(int argc, char const *argv[]) {

    struct hostent *hp;
    struct sockaddr_in server;
    struct sockaddr_in local_addr;

    string host;
    string username;
    string message;
    string address;
    char ip[16];
    char **pptr;
    int port;

    signal(SIGINT, signal_handler);

    switch (argc) {
        case 2:
            host = argv[1];
            port=SERVER_TCP_PORT;
            break;
        case 3:
            host = argv[1];
            port = atoi(argv[2]);
            break;
        case 4:
            host = argv[1];
            port = atoi(argv[2]);
            username = argv[3];
            break;            
        default:
            cout << "Usage: " << argv[0] << " host port [username] " << endl;
            exit(EXIT_FAILURE);
            break;
    }

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        cerr << "Failed to create socket" << endl;
        exit(EXIT_FAILURE);
    }

    memset (&server, 0, sizeof(server));
    server.sin_family   = AF_INET;
    server.sin_port     = htons(port);

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

    thread receive_thread(receive_message);

    socklen_t addr_len = sizeof(local_addr);
    getsockname(sd, (struct sockaddr*)&local_addr, &addr_len);
    address = inet_ntoa( local_addr.sin_addr);

    while (1) {

        // TODO: fix user prompt (maybe ncurses?)
        // cout << address << " (You): ";

        getline(cin, message);

        if (username.empty()) {
            message = address + ": " + message;
        } else {
            message = address + " (" + username + "): " + message;
        }

        if (message.length() <= BUFLEN) {
            send (sd, message.c_str(), BUFLEN, 0);
        }
    }

}