// ringmaster.hpp
#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib> // For atoi
#include <netinet/in.h> // For sockaddr_in
#include <sys/socket.h> // For socket functions
#include <unistd.h> // For close
using namespace std;

class player {
public:
    player(string machine_name, int port_num) 
    : machine_name(machine_name), port_num(port_num) {}


private:
    string machine_name;
    int port_num;
};

#endif // PLAYER_HPP