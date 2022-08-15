#include "connection.hpp"

OperateRequest operateRequest = OperateRequest();

Connection::Connection(int fd, std::vector<ServerBlock> block, Epoll *ep) : clntFd_(fd), block_(block), ep_(ep) {
	// Ctl_mode_flag_ = false;
	phase_msg_ = START_LINE_INCOMPLETE;
	req_status_code_ = NOT_DEFINE;
	buffer_content_length = -1;
	client_max_body_size = 0;
	is_chunk = false;
}

Connection::~Connection() { }

void    Connection::processRequest()
{

    int n = recv(this->clntFd_, &buffer_char, sizeof(buffer_char) - 1, 0); //except \r

	/* protection to disconnect
	if (n < 0 || strchr(buffer_char, 0xff))
	{
		//close connection
		printf("recv Error\n");
		return ;
		//need function to close connection
		//return (Error); or return (-1);
	}*/
	// std::cout << "n: " << n << std::endl;
	// std::cout << "buffer_char: " << buffer_char << std::endl;
	// std::cout << "buffer_char + n: " << buffer_char + n << std::endl;
    // buf_.insert(buf_.end(), buffer_char, buffer_char + n);
	buffer_.insert(buffer_.end(), buffer_char, buffer_char + n);
	// std::cout << "buffer_: " << buffer_ << std::endl;

	if (phase_msg_ == START_LINE_INCOMPLETE
		|| phase_msg_ == START_LINE_COMPLETE
		|| phase_msg_ == HEADER_INCOMPLETE
		|| phase_msg_ == HEADER_COMPLETE)
		operateRequest.checkRequestMessage(this);
	if (phase_msg_ == BODY_CHUNKED)
	{
		operateRequest.checkChunkedMessage(this);
	}
	else if (phase_msg_ == BODY_INCOMPLETE)
		operateRequest.checkRequestBody(this);
	if (phase_msg_ == BODY_COMPLETE)
	{
		std::cout << "************ Message body process **********" << std::endl;
		size_t pos = 0;
		if ((pos = buffer_.find(CRLF)) != std::string::npos) 
			ep_->epoll_Ctl_Mode(clntFd_, EPOLLOUT);
	}


	// std::vector<std::string> ret = getBlock().locationList[0].getReturn();

	// for (size_t i = 0; i < ret.size(); i++)
	// 	std::cout << "///////////////////////////////////////////vector value: " << ret[i] << std::endl;

	////////////
	// if (phase_msg_ == BODY_COMPLETE)
	// {
	// 	std::cout << "CGI / EXECUTE / RESPONSE NEED TO BE DEAL" << std::endl;
	// 	// ep_->epoll_Ctl_Mode(clntFd_, EPOLLOUT);
	// }
	////////////

	//to change Ctl Mode when message is done with CRLFCRLF
	
	// if (n == 0 && buffer_.empty() && phase_msg_ == BODY_COMPLETE)
	memset(&buffer_char, 0, n);
}

void    Connection::processResponse()
{
	std::string	bufForBody_;

	bufForBody_ = this->response_.getBodyStr(this->request_.getFilePath());
	// file open
	// html, ou cgi
	// file save in buf_str;

	// make header with size buf_str

	// add in return buf
	// header and buf_str

	// send all
	// close fd

}

//void	Connection::executeGetMothod(void){

	//std::cout << "get" << std::endl;

	
	//std::cout << this->request_.getBody() << std::endl;
	//std::cout << this->request_.getFilePath() << std::endl;
	////std::cout << this->request_.getHeaderValue() << std::endl;
	//std::cout << this->request_.getHost() << std::endl;
	//std::cout << this->request_.getMethod() << std::endl;
	//std::cout << this->request_.getPath() << std::endl;
	//std::cout << this->request_.getQueryString() << std::endl;
	//std::cout << this->request_.getUri() << std::endl;
//	// std::string	buf;
//
//	//header setting
//	/*	
//		buf += setHeader();
//	*/
//
//	// execute html or cgi
//	/*
//		if (getFormat == "html")
//			buf += getBufHTML();
//		else (getFormat == cgi)
//			buf += getBufCGI();
//
//	*/
//	// envoyer par send
//	/*
//		send(this->clntFd_, &buf, sizeof(buf) - 1, 0); 
//	*/
//	// considerer EPOLLIN ou enlever fd de EPOLL
//	/*
//		if (keep_alive)
//			ep_->epoll_Ctl_Mode(clnFd_, EPOLLIN);
//		else
//			fd out!
//	*/
//}

//getter
std::vector<ServerBlock>		&Connection::getBlock(void) {
	return (block_);
}

Request	&Connection::getRequest(void) {
	return (request_);
}

Response	&Connection::getResponse(void) {
	return (response_);
}

int			&Connection::getReqStatusCode(void) {
	return (req_status_code_);
}

int		&Connection::getPhaseMsg(void) {
	return (phase_msg_);
}

std::string	&Connection::getBuffer(void) {
	return (buffer_);
}

ServerBlock	Connection::getServerConfig(void) {
	return (serverConfig_);
}

LocationBlock	Connection::getLocationConfig(void) {
	return (locationConfig_);
}

std::string		&Connection::getBodyBuf(void) {
	return (body_buf);
}


//setter
void	Connection::setReqStatusCode(int status_code) {
	req_status_code_ = status_code;
}

void	Connection::setPhaseMsg(int new_msg) {
	phase_msg_ = new_msg;
}

//void    Connection::response() {
//    std::cout <<"Response execute" <<std::endl;
//}

void	Connection::setServerBlockConfig(std::string server_name) {
	serverConfig_ = getServerConfigByServerName(server_name);
}

void	Connection::setLocationConfig(LocationBlock locationblock) {
	locationConfig_ = locationblock;
}

void	Connection::setBodyBuf(std::string bodybuf) {
	body_buf = bodybuf;
}


bool		Connection::checkLocationConfigExist(std::string path) {
	std::pair<bool, LocationBlock> location_pair;

	location_pair = serverConfig_.getLocationBlock(path);
	// std::cout << "how many: " << serverConfig_.getLocationBlock().size() << std::endl;
	if (location_pair.first == true)
	{
		setLocationConfig(location_pair.second);
		// std::cout << "$$$$$" << location_pair.second.getUriPath() << std::endl;
		return (true);
	}
	else
		return (false);
}

//tmp
void	Connection::printRequestMsg(void) {
	printf("====Request Parser====\n");
	printf("Request_status_code: %d\n", getReqStatusCode());
	printf("Phase_line: %d\n", getPhaseMsg());
	printf("method_: %s\n", getRequest().getMethod().c_str());
	printf("uri_: %s\n", getRequest().getUri().c_str());
	printf("path_: %s\n", getRequest().getPath().c_str());
	printf("file_path_: %s\n", getRequest().getFilePath().c_str());
	printf("host_: %s\n", getRequest().getHost().c_str());
	printf("port_: %s\n", getRequest().getPort().c_str());

	printf("version_: %s\n", getRequest().getVersion().c_str());
	getRequest().printHeaders();
	std::cout << std::endl;
	printf("=====================\n");
	printf("body:\n");
	printf("%s\n", getRequest().getBody().c_str());
	// printf("%s\n", getBodyBuf().c_str());
	printf("=====================\n");
	std::cout << "content_length: " << getRequest().getHeaderValue("Content-Length") << std::endl;
	std::cout << "client_max_body_size: " << client_max_body_size << std::endl;
	printf("=====================\n");
}

ServerBlock	Connection::getServerConfigByServerName(std::string server_name)
{
	int index = this->block_.size();
	bool ret;

	for (int i = 0; i < index; i++)
	{
		ret = block_[i].checkServerName(server_name);
		if (ret == true)
		{
			return (block_[i]);
		}
	}
	return (block_[0]);
}

