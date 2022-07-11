#include "socket.hpp"
#include <sys/epoll.h>
#include <map>
#include "../response/Response.hpp"
#include <stdlib.h>
#include <stdio.h>

# define MAX_EVENT 1024
# define TIMEOUT -1

class Request
{
private:
    int fd;

public:
    Request() {std::cout << "in the map" << std::endl;};
    Request(const Request &other) : fd(other.fd)
    {
        int i = 0;
        std::string buf;
        if(0 >(i = read(fd, &buf, sizeof(buf))))
            buf+=buf;
        write(1, &buf, i);
    };
    Request(int num) : fd(num)
    { 
        std::cout << "[" << this->fd << "] client socket number!" <<std::endl; 
    };
    ~Request() {};
};

class Epoll
{

public:
    typedef int                         clntFd;
    typedef std::vector<Block>          vecBloc;
    typedef std::map<clntFd, Request>   mapClnt;
    typedef struct epoll_event          event;
    Socket                              sock;
    typedef std::pair<int, Request>     mapPair;
 
private:
    vecBloc vecBloc_;
    mapClnt mapClnt_;
    int     epollFd_;

public:
    Epoll();
    Epoll(std::vector<Block> block); // epoll fd create
    Epoll(const Epoll &other);
    ~Epoll();

    void            init_server_socket();
    void            create_epoll_fd();
    int             epoll_add(int fd);
    void            epoll_server_manager();
    void            close_all_serv_socket();

    int             find_server_fd(int fd);
    int             create_clnt_socket(int fd);
};
