#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib> // For atoi
#include <netinet/in.h> // For sockaddr_in
#include <sys/socket.h> // For socket functions
#include <unistd.h> // For close
#include "ringmaster.hpp"
using namespace std;



int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: ./ringmaster <port_num> <num_players> <num_hops>\n";
        return 1;
    }

    int port = std::atoi(argv[1]);
    int num_players = std::atoi(argv[2]);
    int num_hops = std::atoi(argv[3]);

    if (num_hops < 0 || num_players <= 0) {
        std::cerr << "Number of players must be grater than 1.\n";
        std::cerr << "Number of hops and hops must be greater than or equal to zero.\n";
        return 1;
    }

    ringmaster ringmaster(port, num_players, num_hops);
    ringmaster.print_background();

}