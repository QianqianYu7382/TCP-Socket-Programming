#include "player.hpp"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

using namespace std;

player::player(std::string machine_name, int port_num) 
    : machine_name(machine_name), port_num(port_num) {}

void player::setup_client() {
    int status;
    int socket_fd;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;

    string port_str = to_string(port_num);

    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;

    status = getaddrinfo(machine_name.c_str(), port_str.c_str(), &host_info, &host_info_list);
    if (status != 0) {
        cerr << "Error: cannot get address info for host" << endl;
        cerr << "  (" << machine_name << "," << port_str << ")" << endl;
        return;
    }

    socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
    if (socket_fd == -1) {
        cerr << "Error: cannot create socket" << endl;
        cerr << "  (" << machine_name << "," << port_str << ")" << endl;
        return;
    }

    cout << "Connecting to " << machine_name << " on port " << port_str << "..." << endl;

    status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        cerr << "Error: cannot connect to socket" << endl;
        cerr << "  (" << machine_name << "," << port_str << ")" << endl;
        return;
    }

    const char *message = "hi there!";
    send(socket_fd, message, strlen(message), 0);

    freeaddrinfo(host_info_list);
    close(socket_fd);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: player <machine_name> <port_num>\n";
        return 1;
    }

    std::string machine_name = argv[1];
    int port_num = atoi(argv[2]);

    player player(machine_name, port_num);
    player.setup_client();

    return 0;
}
