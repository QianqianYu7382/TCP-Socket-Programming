#include "player.hpp"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string>

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
    std:: string message = receive_info(socket_fd);
    get_neighbor_info(message);

    


    end_game(host_info_list, socket_fd);
    
}

void player::get_neighbor_info(string message) {
    string player_id_str, left_ip, right_ip, left_port_str, right_port_str;
    
    std::istringstream iss(message);
    iss.ignore(256, ':'); 
    iss >> player_id_str; 
    iss.ignore(256, ':'); 
    iss >> left_ip;       
    iss.ignore(256, ':'); 
    iss >> right_ip;      
    iss.ignore(256, ':'); 
    iss >> left_port_str; 
    iss.ignore(256, ':'); 
    iss >> right_port_str; 

    player_id = stoi(player_id_str);
    int left_port = stoi(left_port_str);
    int right_port = stoi(right_port_str);
    neighbor_port.push_back(left_port);
    neighbor_port.push_back(right_port);
    neighbor_ip.push_back(left_ip);
    neighbor_ip.push_back(right_ip);
}

std:: string player::receive_info(int socket_fd) {
    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];

    int bytes_received = recv(socket_fd, buffer, BUFFER_SIZE, 0);
    if (bytes_received == -1) {
        cerr << "Error in receiving data from server" << endl;
        return;
    } else if (bytes_received == 0) {
        cout << "Server closed the connection" << endl;
        return;
    }

    buffer[bytes_received] = '\0';

    string message(buffer);
    cout << "Received message from server: " << message << endl;
    return message;
}

void player::end_game(struct addrinfo *host_info_list, int socket_fd) {
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
