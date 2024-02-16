#include "ringmaster.hpp"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>  
#include <arpa/inet.h> 

using namespace std;

ringmaster::ringmaster(int port, int num_players, int num_hops)
: port(port), num_players(num_players), num_hops(num_hops) {}

void ringmaster::print_background() {
    cout << "Potato Ringmaster\n";
    cout << "Players = " << num_players << "\n";
    cout << "Hops = " << num_hops << endl;
}


void ringmaster::setup_server() {
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
        return;
    }

    server_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
    if (server_fd == -1) {
        std::cerr << "Error: cannot create socket (" << port_str << ")" << std::endl;
        return;
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
        return;
    }

    status = listen(server_fd, num_players);
    if (status == -1) {
        std::cerr << "Error: cannot listen on socket (" << port_str << ")" << std::endl;
        return;
    }

    std::cout << "Waiting for connection on port " << port << std::endl;

    // 只接受num_players指定数量的客户端连接
    while (client_sockets.size() < static_cast<unsigned int>(num_players)) {
        struct sockaddr_storage their_addr;
        socklen_t addr_size = sizeof(their_addr);
        int new_fd = accept(server_fd, (struct sockaddr *)&their_addr, &addr_size);
        if (new_fd != -1) {
            client_sockets.push_back(new_fd);
            std::cout << "Player " << client_sockets.size() <<" ready to connect!"<< std::endl;
        } 
    }



    if (num_hops == 0) {
        for (int client_fd : client_sockets) {
            close(client_fd); // 关闭客户端套接字
        }
        close(server_fd);
        freeaddrinfo(host_info_list);
    }
}

void ringmaster::get_info(vector<int> client_sockets) {
    for (int new_fd : client_sockets) {
        // 获取客户端的地址信息
        struct sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof(client_addr);
        getpeername(new_fd, (struct sockaddr *)&client_addr, &client_addr_size);
        std::string client_ip = inet_ntoa(client_addr.sin_addr);
        int client_port = ntohs(client_addr.sin_port);
        client_ips.push_back(client_ip);
        client_ports.push_back(client_port);

        // 获取服务器的地址信息
        struct sockaddr_in server_addr;
        socklen_t server_addr_size = sizeof(server_addr);
        getsockname(new_fd, (struct sockaddr *)&server_addr, &server_addr_size);
        std::string server_ip = inet_ntoa(server_addr.sin_addr);
        int server_port = ntohs(server_addr.sin_port);
        server_ips.push_back(server_ip);
        server_ports.push_back(server_port);
    }

}


int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: ./ringmaster <port_num> <num_players> <num_hops>\n";
        return 1;
    }

    int port = atoi(argv[1]);
    int num_players = atoi(argv[2]);
    int num_hops = atoi(argv[3]);

    if (num_hops < 0 || num_players <= 0) {
        cerr << "Error: Invalid number of players or hops.\n";
        return 1;
    }

    ringmaster ringmaster(port, num_players, num_hops);
    ringmaster.print_background();
    ringmaster.setup_server();

    return 0;
}
