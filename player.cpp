#include "player.hpp"
#include "function.hpp"
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
    
    int socket_fd = create_client(machine_name, port_num);
    int server_fd = create_server(0);
    local_port = get_port(server_fd);
    std::string local_port_str = std::to_string(local_port);

    std::string message = local_port_str;
    send(socket_fd, message.c_str(), message.size(), 0);
    std:: string message1 = receive_info(socket_fd);
    get_neighbor_info(message1);

    

    end_game(socket_fd);
    
}

void player::connect_neighbor(int socket_fd) {
    int socket_client_fd_left = create_client(neighbor_ip[0], neighbor_port[0]);

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


string player:: receive_info(int socket_fd) {
    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];

    int bytes_received = recv(socket_fd, buffer, BUFFER_SIZE, 0);
    if (bytes_received == -1) {
        cerr << "Error in receiving data from server" << endl;
        return 0;
    } else if (bytes_received == 0) {
        cout << "Server closed the connection" << endl;
        return 0;
    }

    buffer[bytes_received] = '\0';

    string message(buffer);
    cout << "Received message from server: " << message << endl;
    return message;
}

void player::end_game( int socket_fd) {
    
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
