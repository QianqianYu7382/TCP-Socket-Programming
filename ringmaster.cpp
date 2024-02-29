#include "ringmaster.hpp"
#include "function.hpp"
// #include "potato.hpp"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>  
#include <arpa/inet.h> 
#include <sstream>
#include <string>
#include <unistd.h>
#include <algorithm>

using namespace std;

ringmaster::ringmaster(int port, int num_players, int num_hops)
: port(port), num_players(num_players), num_hops(num_hops) {}

void ringmaster::print_background() {
    cout << "Potato Ringmaster\n";
    cout << "Players = " << num_players << "\n";
    cout << "Hops = " << num_hops << endl;
}


void ringmaster::setup_server(Potato & potato) {
// void ringmaster::setup_server() {
    int server_fd = create_server(port);

    // std::cout << "Waiting for connection on port " << port << std::endl;

    // 只接受num_players指定数量的客户端连接
    // while (client_sockets.size() < static_cast<unsigned int>(num_players)) {
    for(int i = 0; i<num_players; i++){
        struct sockaddr_storage their_addr;
        socklen_t addr_size = sizeof(their_addr);
        int new_fd = accept(server_fd, (struct sockaddr *)&their_addr, &addr_size);
        if (new_fd != -1) {
            client_sockets.push_back(new_fd);
            // std::cout << "Player " << client_sockets.size() <<" ready to connect!"<< std::endl;
            std::cout << "Player " << i <<" ready to play"<< std::endl;
            string local_port = receive_info( new_fd);
            int local_port_str = stoi(local_port);
            client_ports.push_back(local_port_str);
        } 
    }

    

    // cout<<"print client_sockets"<<endl;
    // for (int i = 0; i < client_sockets.size(); i++) {
    //     cout<<client_sockets[i]<<endl;
    // }

    get_info(client_sockets);
    send_info();

    if (num_hops == 0) {
        cout<<"Trace of potato:"<<endl;
        cout<<""<<endl;
        for (int i = 0; i < num_players; i++) {
            ssize_t bytes_sent_send_final = send(client_sockets[i], &potato, sizeof(potato), 0);
        }
        return;
    }

    srand((unsigned int)time(NULL)+1);
    int first_id = rand() % num_players;
    cout<<"Ready to start the game, sending potato to player "<<first_id<<endl;
    ssize_t bytes_sent = send(client_sockets[first_id], &potato, sizeof(potato), 0);

    listen_player(num_players, potato);

    

    if (num_hops == 0) {
        for (int client_fd : client_sockets) {
            close(client_fd); 
        }
        close(server_fd);
    }
}

void ringmaster::listen_player(int num_players, Potato & potato) {
    std::vector<int> fds;
    for (int i = 0; i < client_sockets.size(); i++) {
        fds.push_back(client_sockets[i]);
    }
    
    int maxFD = *max_element(fds.begin(), fds.end()); 
    // for (int i = 0; i < 3; i++) {
    //     cout<<i<<": "<<fds[i]<<endl;
    // }

    int check = 1;
    while (check == 1) {
        fd_set readfds;
        FD_ZERO(&readfds);
        for (int i = 0; i < client_sockets.size(); i++) {
            FD_SET(fds[i], &readfds); 
        }

        int activity = select(maxFD + 1, &readfds, NULL, NULL, NULL); 

        if ((activity < 0) && (errno != EINTR)) {
            std::cerr << "Select error: " << strerror(errno) << std::endl;
            break; // Exit or handle error
        }

        for (int fd : fds) {
            if (FD_ISSET(fd, &readfds)) {
                int revc_potato = recv(fd, &potato, sizeof(potato), 0);
                cout<<"Trace of potato:"<<endl;
                cout<<potato.record[0];
                for (int i = 1; i < num_hops; i++) {
                    cout<<","<<potato.record[i];
                }
                cout<<endl;
                // receive_potato(fd, leftPlayerFD, rightPlayerFD, ringMasterFD, potato);
                

                check = 0;
            }
        }
    }

    // cout<<"send final potato"<<endl;
    // cout<<"potato hops: "<<potato.hops<<endl;
    for (int i = 0; i < num_players; i++) {
        ssize_t bytes_sent_send_final = send(client_sockets[i], &potato, sizeof(potato), 0);
    }
}

void ringmaster::send_info() {
    Potato test_p;
    test_p.hops = 12;
    test_p.idx = 10;
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
                      " Right_Port: " + right_port_str + + "\n";


        // cout<<"print send mess here +++++++++++++++++"<<endl;
        // cout<<message<<endl;
        // cout<<"print send mess here +++++++++++++++++"<<endl;


        int len = message.length();
        ssize_t bytes_sent1 = send(client_sockets[i], &len, sizeof(len), 0);

        ssize_t bytes_sent = send(client_sockets[i], message.c_str(), len, 0);
        
         ssize_t bytes_sent_num_players = send(client_sockets[i], &num_players, sizeof(num_players), 0);
        

        // int send_test = 8273;
        // ssize_t bytes_sent11 = send(client_sockets[i], &send_test, sizeof(send_test), 0);
        // int send_test1 = 7238;
        // ssize_t bytes_sent1 = send(client_sockets[i], &send_test1, sizeof(send_test), 0);
        // Potato test_potato;
        // test_potato.hops = 999;
        // int test_hop = 999;
        // ssize_t bytes_sent2 = send(client_sockets[i], &test_hop, sizeof(test_hop), 0);

        // int test_hop1 = test_potato.hops;
        // ssize_t bytes_sent3 = send(client_sockets[i], &test_hop1, sizeof(test_hop1), 0);

        // ssize_t bytes_sent4 = send(client_sockets[i], &test_potato, sizeof(test_potato), 0);

        
        // if (bytes_sent == -1) {
        //     perror("send");
        //     // Handle error
        // }
    }
    // for(size_t i = 0; i<client_sockets.size();i++){
    //     int send_potato = send(client_sockets[i], &test_p, sizeof(test_p), 0);
    //     if(send_potato > 0){
    //         cout<<"test potato has successfully send to "<<i<<" hops is: "<<test_p.hops<<endl;
    //     }
    // }
}

void ringmaster::get_info(vector<int> client_sockets) {
    for (int new_fd : client_sockets) {
        // 获取客户端的地址信息
        struct sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof(client_addr);
        getpeername(new_fd, (struct sockaddr *)&client_addr, &client_addr_size);
        std::string client_ip = inet_ntoa(client_addr.sin_addr);
        client_ips.push_back(client_ip);
        // cout<<"client_ip"<<client_ip<<endl;



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
    Potato potato;
    potato.hops = num_hops;
    potato.idx = 0;

    if (num_hops < 0 || num_players <= 1) {
        cerr << "Error: Invalid number of players or hops.\n";
        return 1;
    }

    ringmaster ringmaster(port, num_players, num_hops);
    ringmaster.print_background();
    ringmaster.setup_server(potato);
    // ringmaster.setup_server(potato);

    return 0;
}
