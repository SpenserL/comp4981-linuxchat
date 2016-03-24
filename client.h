#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <ctime>
#include <thread>
#include <netdb.h>
#include <csignal>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
using namespace std;

#define SERVER_TCP_PORT 7000
#define BUFLEN          511

// GLOBALS
int sd;             // socket descriptor
ofstream myfile;    // file stream for logging

string get_time();
void signal_handler(int signum);
bool proccess_args(int *argc, char **argv, string *host, int *port, string *username, bool *logactive);
void receive_message();
int main(int argc, char *argv[]);
bool logactive=false;