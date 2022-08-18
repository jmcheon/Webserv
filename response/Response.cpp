#include "Response.hpp"
namespace ft
{

void		Response::initialMapHeaders(void)
{
  this->headers_["Allow"] = "";
  this->headers_["Content-Location"] = "";
  this->headers_["Last-Modified"] = "";
  this->headers_["Location"] = "";
  this->headers_["Retry-After"] = "";
  this->headers_["Server"] = "";
  this->headers_["Transfer-Encoding"] = "";
  this->headers_["WWW-Authenticate"] = "";
  this->headers_["Content-Language"] = "";
  this->headers_["Content-Length"] = "";
  this->headers_["Content-Type"] = "";
}

void		Response::initialMapStatusCode()
{
	this->mapStatus_.insert(std::make_pair("100", "Continue"));
	this->mapStatus_.insert(std::make_pair("101", "Switching Protocols"));
	this->mapStatus_.insert(std::make_pair("103", "Early Hints"));
	this->mapStatus_.insert(std::make_pair("200", "OK"));
	this->mapStatus_.insert(std::make_pair("201", "Created"));
	this->mapStatus_.insert(std::make_pair("202", "Accepted"));
	this->mapStatus_.insert(std::make_pair("203", "Non-Authoritative Information"));
	this->mapStatus_.insert(std::make_pair("204", "No Content"));
	this->mapStatus_.insert(std::make_pair("205", "Reset Content"));
	this->mapStatus_.insert(std::make_pair("206", "Partial Content"));
	this->mapStatus_.insert(std::make_pair("300", "Multiple Choices"));
	this->mapStatus_.insert(std::make_pair("301", "Moved Permanently"));
	this->mapStatus_.insert(std::make_pair("302", "Found"));
	this->mapStatus_.insert(std::make_pair("303", "See Other"));
	this->mapStatus_.insert(std::make_pair("304", "Not Modified"));
	this->mapStatus_.insert(std::make_pair("305", "Use Proxy"));
	this->mapStatus_.insert(std::make_pair("306", "unsued"));
	this->mapStatus_.insert(std::make_pair("307", "Temporary Redirect"));
	this->mapStatus_.insert(std::make_pair("308", "Permanent Redirect"));
	this->mapStatus_.insert(std::make_pair("400", "Bad Request"));
	this->mapStatus_.insert(std::make_pair("401", "Unauthorized"));
	this->mapStatus_.insert(std::make_pair("402", "Payment Required"));
	this->mapStatus_.insert(std::make_pair("403", "Forbidden"));
	this->mapStatus_.insert(std::make_pair("404", "Not Found"));
	this->mapStatus_.insert(std::make_pair("405", "Method Not Allowed"));	
	this->mapStatus_.insert(std::make_pair("408", "Request Timeout"));	
	this->mapStatus_.insert(std::make_pair("411", "Length Required"));	
	this->mapStatus_.insert(std::make_pair("413", "Payload Too Large"));	
	this->mapStatus_.insert(std::make_pair("414", "URI Too Long"));	
	this->mapStatus_.insert(std::make_pair("500", "Internal Server Error"));	
	this->mapStatus_.insert(std::make_pair("502", "Bad Gateway"));	
	this->mapStatus_.insert(std::make_pair("505", "HTTP Version Not Supported"));	
}
		

Response::Response()
{
	initialMapStatusCode();
	initialMapHeaders();
}

Response::Response(Response const & copy)
{
	(*this) = copy;
}

Response & Response::operator=(Response const & copy)
{
	(void) copy;
	return (*this);
}

Response::~Response()
{
}

/* ********************************************************************************* */
/* ****************************************** getter ******************************* */
/* ********************************************************************************* */
std::string		Response::makeErrorPage(int	status_code) {
	std::string	errorCode_ = toString(status_code);
	std::string	errorMessage_(this->mapStatus_[errorCode_]);
	std::string	errorBody_;

	errorBody_ += "<!DOCTYPE html>\r\n";
	errorBody_ += "<html lang=\"en\">\r\n";
	errorBody_ += "<head>\r\n";
	errorBody_ += "	<meta charset=\"UTF-8\">\r\n";
	errorBody_ += "	<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\r\n";
	errorBody_ += "	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n";
	errorBody_ += "	<title>";
	errorBody_ += errorCode_;
	errorBody_ += "</title>\r\n";
	errorBody_ += "</head>\r\n";
	errorBody_ += "<body>\r\n";
	errorBody_ += "	<h1>";
	errorBody_ += errorMessage_;
	errorBody_ += "</h1>\r\n";
	errorBody_ += "</body>\r\n";
	errorBody_ += "</html>";
	
	return errorBody_;
}

std::string		Response::makeBodyHtml(std::string const &filePath) {
	std::string		ret;
	std::string		filePath_;
	std::ifstream 	ifs;
	
	filePath_ = "./" + filePath;

	ifs.open(const_cast<char*>(filePath_.c_str()));
	if (ifs.fail())
	{
		// std::cout << "error for open" << std::endl;
		return makeErrorPage(404);
	}

	std::string	str;
	while (std::getline(ifs, str))
	{
		ret += "\r\n";
		ret += str;
	}

	return (ret);
}

/*
Referrer-Policy: no-referrer
Content-Length: 1555
Date: Thu, 18 Aug 2022 02:34:57 GMT

*/
std::string		Response::makeHeader(int bodySize, int statusCode) {
	std::string	header_;

	// body size -> Content-Length
	// append "Content-Lengh" to headers_
	setContentLengh(bodySize);
	// make start line
	header_ += makeStartLine(statusCode);
	// append headers value!
	header_ += appendMapHeaders();
	// time
	header_ += makeTimeLine();

	return header_;
}

std::string		Response::appendMapHeaders(void) {
	std::string	mapHeader_;

	for (ft::mapHeader::iterator it=headers_.begin(); it!=headers_.end(); ++it) {
		if ( !(it->second.empty()) ) {
			mapHeader_ += it->first;
			mapHeader_ += ": ";
			mapHeader_ += it->second;
			mapHeader_ += "\r\n";
		}
	}

	return mapHeader_;
}

/* Date: Thu, 18 Aug 2022 11:02:41 GMT */
std::string		Response::makeTimeLine(void) {
	std::string	timeLine;
	timeLine += "Date: ";
	
  	time_t rawtime;
  	struct tm* timeinfo;
  	char buffer[80];
  	time(&rawtime);
  	timeinfo = localtime(&rawtime);

  	strftime(buffer, 80, "%a, %d %b %Y %T GMT", timeinfo);

	timeLine += buffer;
	timeLine += "\r\n";
	return (timeLine);
}

/* ********************************************************************************* */
/* ****************************************** setter ******************************* */
/* ********************************************************************************* */
void			Response::setRequest(Request const &request) {
	this->request_ = request;
	this->request_.setFilePath(request.getFilePath());
}

void			Response::setRequestValue(void){
	// set value from request class
	this->setValueFromRequest();
	// content-type : mime
	this->setContentType(); 
	// printMapHeader(headers_);
}

/* ********************************************************************************* */
/* ****************************************** utils ******************************** */
/* ********************************************************************************* */

/* HTTP/1.0 400 Bad Request */
std::string		Response::makeStartLine(int statusCode) {
	std::string	startLine;
	std::string	statusCode_ = toString(statusCode);
	std::string	statusMessage_ = mapStatus_[statusCode_];

	startLine += request_.getVersion();
	startLine += " ";
	startLine += statusCode_;
	startLine += " ";
	startLine += statusMessage_;
	startLine += "\r\n";	
	return startLine;
}

void			Response::setContentLengh(int bodySize) {
	this->headers_["Content-Length"] = toString(bodySize);
}

void		Response::setContentType(void) {

	// there is no file path
	std::string	file_path(request_.getFilePath());
	std::string	ExtFile(getExt(file_path));
	this->headers_["Content-Type"] = mimeType_.getMIMEType(ExtFile);
	this->headers_["Content-Type"] += "; charset=UTF-8";
}

std::string 	Response::toString(const int& v)
{
	std::ostringstream ss;
	ss << v;
	return (ss.str());
}

std::string	Response::getExt(std::string const &filename) const
{
	std::string	ext;
	std::string::size_type	idx;
	idx = filename.rfind(".");
	if (idx != std::string::npos) {
		ext = filename.substr(idx + 1);
	}
	//std::cout << ext << " is ext in getExit" << std::endl;
	//std::cout << filename << " is filename in getExt"  << std::endl;
    return ext;
}

void			Response::setValueFromRequest(void){
	ft::mapHeader mapRequest(request_.getRequestHeaders());

	ft::mapHeader::iterator	itForHeader;
	for (ft::mapHeader::iterator it=mapRequest.begin(); it!=mapRequest.end(); ++it) {
		itForHeader = headers_.find(it->first);
		if (itForHeader != headers_.end()
			&& itForHeader->first != "Content-Length") {
 				headers_[itForHeader->first] = it->second;
		}
	}
}


/* ********************************************************************************* */
/* ****************************************** tester ******************************* */
/* ********************************************************************************* */
void				Response::printMapHeader(ft::mapHeader	mapHeader_) const{
	typedef std::map<std::string, std::string>::iterator	it_;

	it_		iter_begin	= mapHeader_.begin();
	it_		iter_end 	= mapHeader_.end();

	for(;iter_begin != iter_end;iter_begin++)
	{
		std::cout << "key == [" << iter_begin->first << "], value == [" \
		<< iter_begin->second << "]" << std::endl;
	}
}

} // namespace ft