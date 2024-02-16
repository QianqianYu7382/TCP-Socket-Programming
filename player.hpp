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
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

class player {
public:
    player(std::string machine_name, int port_num);
    void setup_client();
    string receive_info(int socket_fd);
    void get_neighbor_info(string message);
    void end_game(struct addrinfo *host_info_list, int socket_fd);

private:
    std::string machine_name;
    int port_num;
    int player_id;
    vector<int> neighbor_port;
    vector<string> neighbor_ip;
};

#endif // PLAYER_HPP