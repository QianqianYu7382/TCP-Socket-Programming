// ringmaster.hpp
#ifndef RINGMASTER_HPP
#define RINGMASTER_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib> // For atoi
#include <netinet/in.h> // For sockaddr_in
#include <sys/socket.h> // For socket functions
#include <unistd.h> // For close
using namespace std;

struct potato {
    int num_hops;
    std::vector<int> id_record;
};

class ringmaster {
public:
    ringmaster(int port, int num_players, int num_hops) 
    : port(port), num_players(num_players), num_hops(num_hops) {}
    
    void print_background()  {
        std::cout << "Potato Ringmaster\n";
        std::cout << "Players = " << num_players << "\n";
        std::cout << "Hops = " << num_hops << std::endl;
    }

    
private:
    int port;
    int num_players;
    int num_hops;
};

#endif // RINGMASTER_HPP
