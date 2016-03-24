#pragma once

#include <iostream>
#include <cstring>
#include <stdio.h>
#include <ctype.h>
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

// thomas
int main(int argc, char const *argv[]);