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

int create_client(string machine_name, int port_num);

int create_server(int port);

int get_port(int server_fd);

string receive_info(int socket_fd);