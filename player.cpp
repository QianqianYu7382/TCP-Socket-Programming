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

void player::setup_client(Potato potato) {
    
    int socket_fd = create_client(machine_name, port_num);
    server_fd = create_server(0);
    local_port = get_port(server_fd);
    std::string local_port_str = std::to_string(local_port);
    cout<<"local port number : "<<local_port_str<<endl;
    std::string message = local_port_str;
    send(socket_fd, message.c_str(), message.size(), 0);

    // string test = receive_info(socket_fd);
    // cout<<test<<endl;


    std:: string message1 = receive_info(socket_fd);
    get_neighbor_info(message1);
    
    (socket_fd);
    int ringMasterFD = socket_fd;
    int leftPlayerFD = left_fd;
    int rightPlayerFD = right_fd;
    listen3(ringMasterFD,leftPlayerFD,rightPlayerFD, potato);


    end_game(socket_fd);
    
}
void player::listen3(int ringMasterFD,int leftPlayerFD,int rightPlayerFD, Potato potato) {
        std::vector<int> fds = {ringMasterFD, leftPlayerFD, rightPlayerFD};
        fd_set readfds;

        while (true) {
            FD_ZERO(&readfds);
            int maxFD = 0;
            for (int fd : fds) {
                FD_SET(fd, &readfds);
                maxFD = std::max(maxFD, fd);
            }

            int activity = select(maxFD + 1, &readfds, NULL, NULL, NULL);

            if ((activity < 0) && (errno != EINTR)) {
                std::cerr << "Select error: " << strerror(errno) << std::endl;
                break; // Exit or handle error
            }

            for (int fd : fds) {
                if (FD_ISSET(fd, &readfds)) {
                    receive_potato(fd, leftPlayerFD, rightPlayerFD, ringMasterFD, potato);
                    std::cout << "Received potato!" << std::endl;
                    // Depending on your logic, you might want to break out of the loop here.
                }
            }
        }
    }

// void player::listen3(int ringMasterFD,int leftPlayerFD,int rightPlayerFD, Potato potato) {
//     fd_set readfds;
//     int maxFD;
//     maxFD = ringMasterFD > leftPlayerFD ? ringMasterFD : leftPlayerFD;
//     maxFD = maxFD > rightPlayerFD ? maxFD : rightPlayerFD;
//     while (true) {
//         FD_ZERO(&readfds);
//         FD_SET(ringMasterFD, &readfds);
//         FD_SET(leftPlayerFD, &readfds);
//         FD_SET(rightPlayerFD, &readfds);

//         int activity = select(maxFD + 1, &readfds, NULL, NULL, NULL);

//         if ((activity < 0) && (errno != EINTR)) {
//             std::cout << "select error" << std::endl;
//         }

//         if (FD_ISSET(ringMasterFD, &readfds)) {
//             cout << "send from master"<<endl;
//             receive_potato(ringMasterFD,leftPlayerFD, rightPlayerFD, ringMasterFD, potato);
//         }

//         if (FD_ISSET(leftPlayerFD, &readfds)) {
//             receive_potato(leftPlayerFD,leftPlayerFD, rightPlayerFD, ringMasterFD, potato);
//         }

//         if (FD_ISSET(rightPlayerFD, &readfds)) {
//             receive_potato(rightPlayerFD,leftPlayerFD, rightPlayerFD, ringMasterFD, potato);
//         }

//         // 处理数据...
//     }

// }

//fd is original send_fd
void player::receive_potato(int fd, int left, int right, int master, Potato& potato) {
    ssize_t bytesReceived = recv(fd, &potato, sizeof(Potato), 0);
    if (bytesReceived == sizeof(Potato)) {
        std::cout << "Potato received with hops: " << potato.hops << ", idx: " << potato.idx << std::endl;
        if (potato.hops > 0) {
            potato.hops--; 
            potato.record[potato.idx] = player_id; 
            potato.idx++;
            srand((unsigned int)time(NULL)+player_id);
            int id = rand() % 2;
            id = 0;
            int send_fd = 0;
            if (id == 0){
                send_fd = left;
                cout<<"send to left"<<endl;
            } 
            
            if (id == 1) {
                send_fd = right;
                cout<<"send to right"<<endl;
            } 
            ssize_t bytes_sent = send(send_fd, &potato, sizeof(potato), 0);

        }
        //give master
        if (potato.hops == 0) {
            send(master, &potato, sizeof(potato), 0);
            cout<<"I'am it!"<<endl;
        }
        std::cout << "Potato modified to hops: " << potato.hops << std::endl;
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
    int right_fd = new_fd;
    cout<<"Connect neighbor success!"<<endl;

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
    cout<<"left_port_str: "<<left_port_str<<endl;
    cout<<"left_ip: "<<left_ip<<endl;

    player_id = stoi(player_id_str);
    int left_port = stoi(left_port_str);
    int right_port = stoi(right_port_str);
    neighbor_port.push_back(left_port);
    neighbor_port.push_back(right_port);
    neighbor_ip.push_back(left_ip);
    neighbor_ip.push_back(right_ip);
    cout<<"Player "<<player_id_str<<" is is ready to play";
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
    // int num_hops = atoi(argv[3]);
    Potato potato;
    player player(machine_name, port_num);
    player.setup_client(potato);



    return 0;
}
