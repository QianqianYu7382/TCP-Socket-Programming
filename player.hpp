// ringmaster.hpp
#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <iostream>
#include "potato.hpp"
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
    void setup_client(Potato potato);
    string receive_info(int socket_fd);
    void get_neighbor_info(string message);
    void end_game(int socket_fd);
    void connect_neighbor(int socket_fd);
    void receive_potato(int fd, int left, int right, int master, Potato& potato);
    void listen3(int ringMasterFD,int leftPlayerFD,int rightPlayerFD, Potato& potato);

private:
    std::string machine_name;
    int port_num;
    int player_id;
    int local_port;
    int server_fd;
    int left_fd;
    int right_fd;
    vector<int> neighbor_port;
    vector<string> neighbor_ip;
};

#endif // PLAYER_HPP