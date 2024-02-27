#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>  
#include <arpa/inet.h> 
#include <sstream>
#include <string>
#include "function.hpp"


using namespace std;

int create_client(string machine_name, int port_num) {
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
        return 0;
    }

    socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
    if (socket_fd == -1) {
        cerr << "Error: cannot create socket" << endl;
        cerr << "  (" << machine_name << "," << port_str << ")" << endl;
        return 0;
    }

    // cout << "Connecting to " << machine_name << " on port " << port_str << "..." << endl;

    status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        cerr << "Error: cannot connect to socket" << endl;
        cerr << "  (" << machine_name << "," << port_str << ")" << endl;
        return 0;
    }
    freeaddrinfo(host_info_list);
    return socket_fd;
    
}

int create_server(int port) {
    int status;
    int server_fd;
    struct addrinfo host_info, *host_info_list;
    const char *port_str = std::to_string(port).c_str();

    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family = AF_UNSPEC; 
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags = AI_PASSIVE;

//hostname
    status = getaddrinfo(NULL, port_str, &host_info, &host_info_list);
    if (status != 0) {
        std::cerr << "Error: cannot get addhost_info_lists info for host (" << port_str << ")" << std::endl;
        return 0;
    }

    server_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
    if (server_fd == -1) {
        std::cerr << "Error: cannot create socket (" << port_str << ")" << std::endl;
        return 0;
    }

    int yes = 1;
    status = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    if (status == -1) {
        perror("Error: cannot bind socket");
        // 考虑更优雅的错误处理方式
    }

    status = bind(server_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        std::cerr << "Error: cannot bind socket (" << port_str << ")" << std::endl;
        return 0;
    }

    status = listen(server_fd, 200);
    if (status == -1) {
        std::cerr << "Error: cannot listen on socket (" << port_str << ")" << std::endl;
        return 0;
    }
    freeaddrinfo(host_info_list);
    return server_fd;

}

int get_port(int server_fd) {
    if (server_fd == -1) {
            std::cerr << "Failed to create server socket." << std::endl;
            return -1;
        }

    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(server_fd, (struct sockaddr *)&sin, &len) == -1) {
        std::cerr << "getsockname failed." << std::endl;
        return -1;
    }

    int local_port = ntohs(sin.sin_port); 
    return local_port;

}

std:: string receive_info(int socket_fd) {
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
    // cout << "Received message from server: " << message << endl;
    return message;
}