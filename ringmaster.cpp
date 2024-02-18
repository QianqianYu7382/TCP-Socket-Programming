#include "ringmaster.hpp"
#include "function.hpp"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>  
#include <arpa/inet.h> 
#include <sstream>
#include <string>

using namespace std;

ringmaster::ringmaster(int port, int num_players, int num_hops)
: port(port), num_players(num_players), num_hops(num_hops) {}

void ringmaster::print_background() {
    cout << "Potato Ringmaster\n";
    cout << "Players = " << num_players << "\n";
    cout << "Hops = " << num_hops << endl;
}


void ringmaster::setup_server() {
    int server_fd = create_server(port);

    std::cout << "Waiting for connection on port " << port << std::endl;

    // 只接受num_players指定数量的客户端连接
    while (client_sockets.size() < static_cast<unsigned int>(num_players)) {
        struct sockaddr_storage their_addr;
        socklen_t addr_size = sizeof(their_addr);
        int new_fd = accept(server_fd, (struct sockaddr *)&their_addr, &addr_size);
        if (new_fd != -1) {
            client_sockets.push_back(new_fd);
            std::cout << "Player " << client_sockets.size() <<" ready to connect!"<< std::endl;
            string local_port = receive_info( new_fd);
            int local_port_str = stoi(local_port);
            client_ports.push_back(local_port_str);
        } 
    }

    get_info(client_sockets);
    send_info();


    if (num_hops == 0) {
        for (int client_fd : client_sockets) {
            close(client_fd); // 关闭客户端套接字
        }
        close(server_fd);
        
    }
}

void ringmaster::send_info() {
    for (size_t i = 0; i < client_sockets.size(); ++i) {
        int palyer_id = i;
        size_t left_index = (i + client_sockets.size() - 1) % client_sockets.size();
        size_t right_index = (i + 1) % client_sockets.size();

        std::string left_ip = client_ips[left_index];
        std::string right_ip = client_ips[right_index];

        int left_port = client_ports[left_index];
        int right_port = client_ports[right_index];

        std::string left_port_str = std::to_string(left_port);
        std::string right_port_str = std::to_string(right_port);
        std::string player_id_str = std::to_string(palyer_id);

        std::string message = "Player_ID: " + player_id_str +
                      " Left_IP: " + left_ip +
                      " Right_IP: " + right_ip +
                      " Left_Port: " + left_port_str +
                      " Right_Port: " + right_port_str + "\n";

        ssize_t bytes_sent = send(client_sockets[i], message.c_str(), message.size(), 0);
        if (bytes_sent == -1) {
            perror("send");
            // Handle error
        }
    }
}

void ringmaster::get_info(vector<int> client_sockets) {
    for (int new_fd : client_sockets) {
        // 获取客户端的地址信息
        struct sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof(client_addr);
        getpeername(new_fd, (struct sockaddr *)&client_addr, &client_addr_size);
        std::string client_ip = inet_ntoa(client_addr.sin_addr);
        client_ips.push_back(client_ip);



        // // 获取服务器的地址信息
        // struct sockaddr_in server_addr;
        // socklen_t server_addr_size = sizeof(server_addr);
        // getsockname(new_fd, (struct sockaddr *)&server_addr, &server_addr_size);
        // std::string server_ip = inet_ntoa(server_addr.sin_addr);
        // int server_port = ntohs(server_addr.sin_port);
        // server_ips.push_back(server_ip);
        // server_ports.push_back(server_port);
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
