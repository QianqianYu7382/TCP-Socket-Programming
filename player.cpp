#include "player.hpp"
#include "function.hpp"
#include <iostream>
#include "potato.hpp"
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <algorithm>

using namespace std;

player::player(std::string machine_name, int port_num) 
    : machine_name(machine_name), port_num(port_num) {}

void player::setup_client(Potato & potato) {
    
    int socket_fd = create_client(machine_name, port_num);
    server_fd = create_server(0);
    local_port = get_port(server_fd);
    std::string local_port_str = std::to_string(local_port);
    // cout<<"local port number : "<<local_port_str<<endl;
    std::string message = local_port_str;

    // 

    send(socket_fd, message.c_str(), message.size(), 0);

    // string test = receive_info(socket_fd);
    // cout<<test<<endl;


    std:: string message1 = receive_info(socket_fd);

    get_neighbor_info(message1);
    connect_neighbor(socket_fd);
    // print_info();
    // int revc_potato = recv(socket_fd, &potato, sizeof(potato), 0);
    // cout << "Received message potato from server: " << potato.hops << endl;

    int ringMasterFD = socket_fd;
    int leftPlayerFD = left_fd;
    int rightPlayerFD = right_fd;
    cout<<"Connected as player "<<player_id<<" out of "<<total_player<<" total players"<<endl;
    listen3(ringMasterFD,leftPlayerFD,rightPlayerFD, potato);


    end_game(socket_fd);
    
}





void player::print_info() {
    cout<<"print info"<<endl;
    for (int i = 0; i < neighbor_port.size(); i++) {
        cout<<"port "<<i<<": "<<neighbor_port[i]<<endl;
    }
    for (int i = 0; i < neighbor_ip.size(); i++) {
        cout<<"ip "<<i<<": "<<neighbor_ip[i]<<endl;
    }
    cout<<"left: "<<left_fd<<endl;
    cout<<"right: "<<right_fd<<endl;
}

void player::listen3(int ringMasterFD, int leftPlayerFD, int rightPlayerFD, Potato &potato) {
    std::vector<int> fds = {ringMasterFD, leftPlayerFD, rightPlayerFD};
    
    int maxFD = *max_element(fds.begin(), fds.end()); 
    // for (int i = 0; i < 3; i++) {
    //     cout<<i<<": "<<fds[i]<<endl;
    // }

    int check = 1;
    srand((unsigned int)time(NULL)+player_id);
    while (check == 1) {
        fd_set readfds;
        FD_ZERO(&readfds);
        for (int i = 0; i < 3; i++) {
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
                if (revc_potato >0) {
                    // cout<<"receive from: "<<fd<<endl;
                    // std::cout << "Potato received with hops: " << potato.hops << ", idx: " << potato.idx << std::endl;
                    if (potato.hops == 0) {
                        check = 0;
                    }
                    if (potato.hops == 1) {
                        potato.hops--; 
                        // cout<<"potato.idx: "<<potato.idx<<endl;
                        potato.record[potato.idx] = player_id; 
                        potato.idx++;
                        send(ringMasterFD, &potato, sizeof(potato), 0);
                        cout<<"I'm it"<<endl;
                        // cout<<"list________________"<<endl;
                        // for (int i = 0; i < 10; i++) {
                        //     cout<<potato.record[i]<<endl;
                        // }
                        // cout<<"list________________"<<endl;
                        check = 0;
                    }else if (potato.hops > 1) {
                        potato.hops--; 
                        // cout<<"potato.idx: "<<potato.idx<<endl;
                        potato.record[potato.idx] = player_id; 
                        potato.idx++;
                        int id = rand() % 2;
                        int send_fd = 0;
                        if (id == 0){
                            send_fd = leftPlayerFD;
                            int id_print = -1;
                            if (player_id-1<0) {
                                id_print = total_player-1;
                            }else {
                                id_print = player_id-1;
                            }
                            cout<<"Sending potato to "<<id_print<<endl;
                        } 
                        
                        if (id == 1) {
                            send_fd = rightPlayerFD;
                            int id_print = -1;
                            if (player_id+1>total_player-1) {
                                id_print = 0;
                            }else {
                                id_print = player_id+1;
                            }
                            cout<<"Sending potato to "<<id_print<<endl;
                        } 
                        ssize_t bytes_sent = send(send_fd, &potato, sizeof(potato), 0);

                    }
                }

                // receive_potato(fd, leftPlayerFD, rightPlayerFD, ringMasterFD, potato);
                
            }
        }
    }
}


void player::connect_neighbor(int socket_fd) {
    //left is server
    int socket_client_fd_left = create_client(neighbor_ip[0], neighbor_port[0]);
    left_fd = socket_client_fd_left;
    //accept right client
    struct sockaddr_storage their_addr;
    socklen_t addr_size = sizeof(their_addr);
    int new_fd = accept(server_fd, (struct sockaddr *)&their_addr, &addr_size);
    right_fd = new_fd;
    // cout<<"Connect neighbor success!"<<endl;
    // cout<<"left_fd"<<left_fd<<endl;
    // cout<<"right_fd"<<right_fd<<endl;

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
    // cout<<"left_port_str: "<<left_port_str<<endl;
    // cout<<"left_ip: "<<left_ip<<endl;

    player_id = stoi(player_id_str);
    int left_port = stoi(left_port_str);
    int right_port = stoi(right_port_str);
    neighbor_port.push_back(left_port);
    neighbor_port.push_back(right_port);
    neighbor_ip.push_back(left_ip);
    neighbor_ip.push_back(right_ip);
    // cout<<"Player "<<player_id_str<<" is is ready to play"<<endl;
}


string player:: receive_info(int socket_fd) {


    int len;
    int bytes_received_len = recv(socket_fd, &len, sizeof(len), 0);
    // cout<<"len is : "<<len<<endl;
    
    char buffer[65538];
    memset(buffer,0,65538);
    int bytes_received = recv(socket_fd, buffer, len, 0);
    if (bytes_received == -1) {
        cerr << "Error in receiving data from server" << endl;
        return 0;
    } else if (bytes_received == 0) {
        cout << "Server closed the connection" << endl;
        return 0;
    }

    int players_total;
    int bytes_received_num_player = recv(socket_fd, &players_total, sizeof(players_total), 0);
    total_player = players_total;


    string message(buffer,len);
    // cout << "Received message from server: " << message << endl;
    // cout << "receive finish"<<endl;

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
    // int num_hops = atoi(argv[3]);
    Potato potato;
    player player(machine_name, port_num);
    player.setup_client(potato);
    // cout<<"now potato info: "<<potato.hops<<endl;



    return 0;
}


