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
    ringmaster(int port, int num_players, int num_hops);
    void print_background();
    void setup_server();
    void get_info(vector<int> client_sockets);

private:
    int port;
    int num_players;
    int num_hops;
    vector<int> client_sockets;
    vector<std:: string> client_ips;
    vector<int> client_ports;

    vector<std:: string> server_ips;
    vector<int> server_ports;

};

#endif // RINGMASTER_HPP
