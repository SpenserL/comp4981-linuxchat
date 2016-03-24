#!/bin/bash

# ----- runtest.sh -----
# script to launch quicly launch multiple clients for testing.
# One client will have logging enabled.
#
# author:   Spenser Lee
# date:     MAR 23 2016

num_clients=3

# function to run a client program
# press ctrl+c to exit.
create_client() {
    ./client localhost 7000 $1
    read -p "Press any key to exit..." -n1 -s
    exit
}

# export the function so that child bash processess can see it
export -f create_client


# function to launch client terminals
create_clients() {
    name=("" "john" "bob" "joe")
    for i in `seq 1 $num_clients`; do
        gnome-terminal -x bash -c "create_client ${name[i]}"
    done
}

# create the clients then launch the server
gnome-terminal -x bash -c ./server
create_clients
