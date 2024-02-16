#include "ringmaster.hpp"

using namespace std;

ringmaster::ringmaster(int port, int num_players, int num_hops)
: port(port), num_players(num_players), num_hops(num_hops) {}

void ringmaster::print_background() {
    cout << "Potato Ringmaster\n";
    cout << "Players = " << num_players << "\n";
    cout << "Hops = " << num_hops << endl;
}

void ringmaster::setup_server() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //maybe
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) { // Max 3 pending connections
        perror("listen");
        exit(EXIT_FAILURE);
    }

while (client_sockets.size() < num_players) {
    int new_socket;
    sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    client_sockets.push_back(new_socket);
    cout << "Player connected," << client_sockets.size() << " socket fd: " << new_socket << endl;

}

for (int sock : client_sockets) {
    close(sock);
}
    close(server_fd);
}

void ringmaster::handle_client(int /* client_sock */) {
    // This is where you would communicate with the client
    // For now, just a placeholder showing the connection is established
    cout << "Client connected successfully" << endl;
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

    ringmaster ring(port, num_players, num_hops);
    ring.print_background();
    ring.setup_server();

    return 0;
}
