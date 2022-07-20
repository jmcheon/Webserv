#include "epoll.hpp"
//
//Canocial Form
//

//Default Construct
Epoll::Epoll() {}

//Construct for excute
Epoll::Epoll(std::vector<Block> block) : vecBloc_(block)
{
	init_server_socket();
	std::cout << "Webserver Run" << std::endl;
	epoll_server_manager();
	std::cout << "Webserver Close" << std::endl;
}

//Copy Construct
Epoll::Epoll(const Epoll &other) : vecBloc_(other.vecBloc_), mapClnt_(other.mapClnt_), epollFd_(other.epollFd_)
{
	*this = other;
}

//Destruct
Epoll::~Epoll() { close_all_serv_socket(); }

//
//Epoll utility functions
//

//Init Epoll Server Fd list
void    Epoll::init_server_socket()
{
	int numServerFd = vecBloc_.size();

	create_epoll_fd();
	for (int i = 0; i < numServerFd; i++)
	{
		if (OK != (epoll_add(vecBloc_[i].getter_socketFd())))
			std::cout << "epoll add server socket failed" << std::endl;
	}
}

//Create Epoll Fd
void    Epoll::create_epoll_fd()
{
	this->epollFd_ = epoll_create(MAX_EVENT);
	if (epollFd_ < 0)
	{
		close_all_serv_socket();
		// tmp exit error
		std::cout << "Epoll fail" << std::endl;
		exit(1);
	}
}

//Epoll_ctl ADD function
int    Epoll::epoll_add(int fd)
{

	event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = fd;
	if (epoll_ctl(this->epollFd_, EPOLL_CTL_ADD, fd, &ev) < 0)
	{
		close(fd);
		return (ERROR);
	}
	return (OK);
}

//Epoll create client FD && accept()
int   Epoll::create_clnt_socket(int fd)
{
	int                     size = this->vecBloc_.size();
	struct sockaddr_in    clnt_addr;
	int                     clntFd;
	int                     clntLen = sizeof(clnt_addr);

	for (int i = 0; i < size; i++)
	{
		if (fd == this->vecBloc_[i].getter_socketFd())
		{
			clntFd = accept(this->vecBloc_[i].getter_socketFd(),
			(struct sockaddr*)&clnt_addr, (socklen_t *)&clntLen);
			if (0 > (clntFd = sock.socket_nonBlock_setting(clntFd)))
				std::cout << "accept() error" << std::endl;
			else
				return (clntFd);
		}
	}
	return (ERROR);

}

//
//Epoll Main management Function
//
void    Epoll::epoll_server_manager()
{
    int                 evCount;
    int                 clntFd;
    event               epEvent[MAX_EVENT];
    
    while (1)
    {
        evCount = epoll_wait(this->epollFd_, epEvent, MAX_EVENT, TIMEOUT);
        std::cout << "Epoll event count [ " << evCount <<" ]" << std::endl;
        if (evCount < 0)
        {           
            std::cout << "Epoll event count error" << std::endl;
            break ;
        }
        for (int i = 0; i < evCount; i++)
        {
            if ((epEvent[i].events & EPOLLERR) || (epEvent[i].events & EPOLLHUP))
            {    
                std::cout << "Epoll event error" << std::endl;
                close(epEvent[i].data.fd);
                continue ;
            }
            else if ((find_server_fd(epEvent[i].data.fd)) == OK)
            {
                clntFd = create_clnt_socket(epEvent[i].data.fd);
                if (clntFd != ERROR)
                { 
                    epoll_add(clntFd);
                    Block requestBlock = get_location_block(epEvent[i].data.fd);
                    this->mapClnt_.insert(std::make_pair (clntFd, Request(clntFd, requestBlock)));
                }
                else
                {
                    std::cout << "accept() error !" << std::endl;
                    close(clntFd);
                    continue ;
                }
            }
            else if(epEvent[i].events & EPOLLIN)
            { 
                int fd = epEvent[i].data.fd;
                mapClnt::iterator it = this->mapClnt_.find(fd);
                epoll_Ctl_Mode(fd, EPOLLOUT);
                it->second.treat_request(); //treat_request()
            }
            else if(epEvent[i].events & EPOLLOUT)
            {
                int fd = epEvent[i].data.fd;
                mapClnt::iterator it = this->mapClnt_.find(fd);
                it->second.send_string();
                epoll_Ctl_Mode(fd, EPOLLIN);
                // if (epEvent[i].events & EPOLLOUT)
                // {
                //     event ev;
                //     ev.data.fd = fd;
                //     epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &ev);
                //     mapClnt_.erase(it);
                //     close(fd);
                // }
            }

        }
    }
}

void    Epoll::epoll_Ctl_Mode(int fd, int op)
{
    event ev;

    if (op == EPOLLIN)
    {
        ev.data.fd = fd;
        ev.events = EPOLLIN | EPOLLET;
        epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &ev);
    }
    else if (op == EPOLLOUT)
    {
        ev.data.fd = fd;
        ev.events = EPOLLOUT | EPOLLET;
        epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &ev);
    }
    else
        std::cout << "EPOLL CTL FUNC ERROR" << std::endl;
}

//
//Utility Functions for Epoll class
//

int     Epoll::find_server_fd(int fd)
{
	int size = vecBloc_.size();

	for (int i = 0; i < size; i++)
	{
		if (fd == vecBloc_[i].getter_socketFd())
			return (OK);
	}
	return (ERROR);
}


void    Epoll::close_all_serv_socket()
{
	int count = this->vecBloc_.size();

	for(int i = 0; i < count; i++)
		close(this->vecBloc_[i].getter_socketFd());
	close(this->epollFd_);
	std::cout << "all socket closed" << std::endl;
}

// Block class or utile  ????
Block   Epoll::get_location_block(int fd)
{
	Block tmp;
	int   size = this->vecBloc_.size();

	for (int i = 0; i < size; i++)
	{
		if (fd == vecBloc_[i].getter_socketFd())
			return(tmp = vecBloc_[i]);
	}
	return (0);
}
