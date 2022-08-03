#pragma once

#include <iostream>
#include <cstdio>
#include "socket.hpp"
// #include "connection.hpp"
#include <sys/epoll.h>
#include <map>
#include <vector>
#include <algorithm>
#include <cctype>
// #include <cstdlib>

typedef std::map<std::string, std::string> 	mapHeader;
// typedef std::vector<unsigned int>			vecByte;

class Request {
	private:
		std::string 	method_;
		std::string 	path_;
		std::string		version_;
		mapHeader		requestHeaders_;
		std::string		body_;

	public:
		Request(void);
		// Request(int fd);
		Request(Request const &x);
		const Request &operator=(const Request &x);
		~Request(void);

		//getter
		const	std::string	&getMethod(void) const;
		const	std::string &getPath(void)	const;
		const	std::string	&getVersion(void)	const;
		const	mapHeader	&getRequestHeaders(void)	const;
		const	std::string	&getHeaderValue(std::string key) const;
		const	std::string	&getBody(void)	const;

		//setter
		void	setMethod(std::string method);
		void	setPath(std::string path);
		void	setVersion(std::string version);
		void	setHeader(std::string key, std::string value);
};
