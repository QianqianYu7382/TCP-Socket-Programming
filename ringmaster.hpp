#ifndef RINGMASTER_HPP
#define RINGMASTER_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib> // For atoi
#include <cstring> // For strerror
#include <netinet/in.h> // For sockaddr_in
#include <sys/socket.h> // For socket functions
#include <unistd.h> // For close
#include <errno.h> // For errno
using namespace std;
class ringmaster {
public:
    ringmaster(int port, int num_players, int num_hops);s
    void print_background();
    void setup_server();

private:
    int port;
    int num_players;
    int num_hops;
    vector<int> client_sockets;
    void handle_client(int client_sock);
}

#endif // RINGMASTER_HPP
