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
    player(std::string machine_name, int port_num);
    void setup_client();

private:
    std::string machine_name;
    int port_num;
    int player_id;
};

#endif // PLAYER_HPP