#include "client.h"

/*---------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: client.cpp
--      The client program - This file contains the functionality for the chat client. A client connects to a server
--      and can send messages and receive message from other clients connected to the same server.
--
-- PROGRAM: client
--
-- FUNCTIONS:
--      int main(int argc, char *argv[]);
--      void receive_message();
--      bool proccess_args(int *argc, char **argv, string *host, int *port, string *username, bool *logactive);
--      string get_time();
--      void signal_handler(int signum);
--
-- DATE: MAR 23 2016
--
-- REVISIONS: MAR 23 2016 - Version 1
--
-- DESIGNER: Spenser Lee
--
-- PROGRAMMER: Spenser Lee & Thomas Yu
--
-- NOTES:
-- This program accepts command arguments for a host ip address
---------------------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: main
--
-- DATE: MAR 23 2016
--
-- REVISIONS: MAR 23 2016 - Version 1
--
-- DESIGNER: Spenser Lee
--
-- PROGRAMMER: Spenser Lee
--
-- INTERFACE: int main(int argc, char const *argv[])
--              argc:   number of commandl line arguments
--              argv:   array of command line arguments
--
-- RETURNS: int: success
--
-- NOTES:
-- The entry point for the client program. This function handles accepts and assigns the command line argument for
-- priority. If valid arguments are received, it will attempt to create a message queue, and override default signaling.
-- Then it will prompt the user for a filename through standard input, and send a request message to message queue.
-- It will then create a thread that waits to file messages from the queue.
----------------------------------------------------------------------------------------------------------------------*/
int main(int argc, char *argv[]) {

    myfile.open ("log.txt",ios::trunc);
    myfile.close();
    myfile.open ("log.txt",ios::app);
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

    if (!proccess_args(&argc, argv, &host, &port, &username, &logactive)) {
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, signal_handler);

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
            message = "[" + get_time() + "] " + address + ": " + message;
        } else {
            message = "[" + get_time() + "] " + address + " (" + username + "): " + message;
        }
        if(logactive){
            myfile<<message.c_str()<<endl;
        }
        if (message.length() <= BUFLEN) {
            send (sd, message.c_str(), BUFLEN, 0);
        }
    }

}
// spenser
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
            if (read == 0) {
                cout << "Server exited... exiting client" << endl;
                exit(EXIT_FAILURE);
            }
        }
        if(logactive){
            myfile << bp << endl;
        }

        cout << recbuf << endl;
        fflush(stdout);
    }
}

// thomas
bool proccess_args(int *argc, char **argv, string *host, int *port, string *username, bool *logactive) {
    string logswitch = "-l";

    switch (*argc) {
        case 2:
            *host = argv[1];
            *port=SERVER_TCP_PORT;
            break;
        case 3:
            *host = argv[1];
            if(argv[2] == logswitch){
                break;
            }
            if(isdigit(*argv[2])){
                *port = atoi(argv[2]);
            } else{
                cerr<<"invalid port number"<<endl;
                return false;
            }

            break;
        case 4:
            *host = argv[1];
            if(isdigit(*argv[2])){
                *port = atoi(argv[2]);
            } else{
                cerr<<"invalid port number"<<endl;
                return false;
            }

            if(argv[3] == logswitch){
                *logactive=true;
                cout<<"Log active"<<endl;
                break;
            }
            *username = argv[3];
            break;
        case 5:
            *host = argv[1];
            if(isdigit(*argv[2])){
                *port = atoi(argv[2]);
            } else{
                cerr<<"invalid port number"<<endl;
                return false;
            }

            *username = argv[3];
            if(argv[4] == logswitch){
                *logactive = true;
                cout<<"Log active"<<endl;
                break;
            }
        default:
            cout << "Usage: " << *argv[0] << " host port [username] [-l] " << endl;
            return false;
    }
    return true;
}

// spenser
string get_time() {

    ostringstream s;
    time_t cur_time;
    struct tm *local_time;

    time( &cur_time );                      // Get the current time
    local_time = localtime( &cur_time );    // Convert the current time to the local time

    s << local_time->tm_hour << ":" << local_time->tm_min << ":" << local_time->tm_sec;
    return s.str();
}

// spenser
void signal_handler(int signum) {
    cout << "\nInterrupt signal (" << signum << ") received.\n";
    close(sd);

   exit(signum);
}
