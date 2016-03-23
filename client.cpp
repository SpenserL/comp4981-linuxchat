#include <cstdlib>
#include <iostream>
#include <string>
#include <cstring>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

#define SERVER_TCP_PORT 7000
#define BUFLEN          255

int main(int argc, char const *argv[]) {

    struct hostent *hp;
    struct sockaddr_in server;

    string host;
    char ip[16];
    int sd;
    char **pptr;

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

    memcpy(hp->h_addr, (char*)&server.sin_addr, hp->h_length);

    if (connect (sd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        cerr << "Couldn't connect to server" << endl;
        exit(EXIT_FAILURE);
    }
    pptr = hp->h_addr_list;
    cout << "Connected: " <<
            hp->h_name << " " <<
            inet_ntop(hp->h_addrtype, *pptr, ip, sizeof(ip)) << endl;


}