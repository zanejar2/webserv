#include "Request.hpp"
#include "Server.hpp"
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <list>

Request::~Request() {}

int	Request::checkMethod(std::string method)
{
	if (method == "GET" || method == "POST" || method == "DELETE")
		return (1);
	else
		return (0);
}

int	Request::checkUriChars(std::string uri)
{
	std::string allowed = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~:/?#[]@!$&'()*+,;=%";
	size_t found = uri.find_first_not_of(allowed);
	if (found != std::string::npos)
		return (0);
	else
		return (1);
}

void Request::firstLineParser(std::string line)
{
	method = line.substr(0, line.find(' '));
	line.erase(0, line.find(' ') + 1);
	uri = line.substr(0, line.find(' '));
	line.erase(0, line.find(' ') + 1);
	version = line.substr(0, line.find('\r'));
}

std::string Request::fileExtentionDefiner()
{
	std::string extension;
	std::string contentType = headers["Content-Type"];
    if (contentType == "text/html") return ".html";
    else if (contentType == "text/css") return ".css";
    else if (contentType == "text/xml") return ".xml";
    else if (contentType == "text/csv") return ".csv";
    else if (contentType == "image/gif") return ".gif";
    else if (contentType == "image/x-icon") return ".ico";
    else if (contentType == "image/jpeg") return ".jpeg";
    else if (contentType == "application/javascript") return ".js";
    else if (contentType == "application/json") return ".json";
    else if (contentType == "image/png") return ".png";
    else if (contentType == "application/pdf") return ".pdf";
    else if (contentType == "image/svg+xml") return ".svg";
    else if (contentType == "text/plain") return ".txt";
    else if (contentType == "application/atom+xml") return ".atom";
    else if (contentType == "application/rss+xml") return ".rss";
    else if (contentType == "image/webp") return ".webp";
    else if (contentType == "video/3gpp") return ".3gpp";
    else if (contentType == "video/mp2t") return ".ts";
    else if (contentType == "video/mp4") return ".mp4";
    else if (contentType == "video/mpeg") return ".mpeg";
    else if (contentType == "video/quicktime") return ".mov";
    else if (contentType == "video/webm") return ".webm";
    else if (contentType == "video/x-flv") return ".flv";
    else if (contentType == "video/x-m4v") return ".m4v";
    else if (contentType == "video/x-mng") return ".mng";
    else if (contentType == "video/x-ms-asf") return ".asf";
    else if (contentType == "video/x-ms-wmv") return ".wmv";
    else if (contentType == "video/x-msvideo") return ".avi";
    else if (contentType == "text/mathml") return ".mml";
    else if (contentType == "text/vnd.sun.j2me.app-descriptor") return ".jad";
    else if (contentType == "text/vnd.wap.wml") return ".wml";
    else if (contentType == "text/x-component") return ".htc";
    else if (contentType == "image/tiff") return ".tif";
    else if (contentType == "image/vnd.wap.wbmp") return ".wbmp";
    else if (contentType == "image/x-jng") return ".jng";
    else if (contentType == "image/x-ms-bmp") return ".bmp";
    else if (contentType == "audio/midi") return ".mid";
    else if (contentType == "audio/mpeg") return ".mp3";
    else if (contentType == "audio/ogg") return ".ogg";
    else if (contentType == "audio/x-m4a") return ".m4a";
    else if (contentType == "audio/x-realaudio") return ".ra";
    else if (contentType == "application/x-perl") return ".pl";
    else if (contentType == "application/x-python") return ".py";
    else if (contentType == "application/x-php") return ".php";
    else if (contentType == "application/x-c++") return ".cpp";
    else if (contentType == "application/x-c") return ".c";
    else if (contentType == "*/*") return ".txt";
    else return ".txt";
}

std::string Request::postedFileNameRandomiser()
{
	int length = 10;
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const int charsetSize = sizeof(charset) - 1;
    std::string result;
    
    std::srand(time(NULL));
    
    for (int i = 0; i < length; ++i) {
        result += charset[std::rand() % charsetSize];
    }
    
    return result;
}

void Request::storeRequestBodyToFile(const std::string& request, int received)
{
    size_t bodyStart = request.find("\r\n\r\n");
    
    if (bodyStart != std::string::npos) 
	{
        bodyStart += 4;
        std::string requestBody = request.substr(bodyStart);
		if (!file.is_open())
		{
			extension = fileExtentionDefiner();			
			postedFileName = postedFileNameRandomiser() + extension;
			file.open(uploadPass + "/" + postedFileName, std::ios::app | std::ios::binary);
			requestSize = received - bodyStart;
		}
		file.write(requestBody.c_str(), received - bodyStart);
    }
}

void Request::storeRequestBodyToString(const std::string& request)
{
    size_t bodyStart = request.find("\r\n\r\n");

    if (bodyStart != std::string::npos)
    {
        bodyStart += 4;
        body = request.substr(bodyStart);
    }
}

void Request::setHeaders(std::string line, std::istringstream &ss)
{
	while (std::getline(ss, line) && line != "\r")
	{
		std::string key = line.substr(0, line.find(':'));
		line.erase(0, line.find(':') + 1);
		std::string value = line.substr(1, line.find('\r') - 1);
		headers[key] = value;
	}
}

int Request::checkUploadDirectory(void)
{
	if (opendir(uploadPass.c_str()) != NULL)
		return 1;
	return 0;
}

int Request::ft_stoi(const std::string str, int base) 
{
    size_t pos = 0;
    int result = 0;
    bool negative = false;

    while (pos < str.length() && std::isspace(str[pos])) {
        pos++;
    }
    if (pos < str.length() && (str[pos] == '-' || str[pos] == '+')) {
        negative = (str[pos] == '-');
        pos++;
    }
    while (pos < str.length()) {
        char ch = std::tolower(str[pos]);

        int digit;
        if (std::isdigit(ch)) {
            digit = ch - '0';
        } else if (base == 16 && std::isxdigit(ch)) {
            digit = (std::islower(ch) ? ch - 'a' + 10 : ch - 'A' + 10);
        } else {
            break;
        }
        result = result * base + digit;
        pos++;
    }
    while (pos < str.length() && std::isspace(str[pos])) {
        pos++;
    }
    return negative ? -result : result;
}

void Request::treatBodyString(std::string requestBody)
{
	size_t pos;
	if (chunkBegin)
	{
		if (!restBody.empty())
		{
			requestBody.insert(0, restBody);
			restBody.clear();
		}
		pos = requestBody.find("\r\n");
		if (pos != std::string::npos)
		{
			std::string hexa = requestBody.substr(0, pos);
			hexaReceived = ft_stoi(hexa, 16);
			requestBody.erase(0, pos + 2);
		}
		chunkBegin = false;
	}
	if (requestBody.length() > hexaReceived)
	{
		appendBody.append(requestBody.substr(0, hexaReceived));
		requestBody.erase(0, hexaReceived + 2);
		restBody.append(requestBody);
		putStringToFile(appendBody);
		appendBody.clear();
		chunkBegin = true;
	} 
	else
	{
		appendBody.append(requestBody);
		hexaReceived -= requestBody.length();
	}
	pos = requestBody.find("0\r\n\r\n");
    if (pos != std::string::npos)
	{
		chunkedRequsestBodyDone = true;
		requestBody.erase(pos, pos + 5);
	}
}

void Request::putStringToFile(std::string treatedBody)
{
	if (!file.is_open())
	{
		extension = fileExtentionDefiner();
		postedFileName = postedFileNameRandomiser() + extension;
		file.open(uploadPass + "/" + postedFileName);
	}
	file.write(treatedBody.c_str(), treatedBody.length());
}


void Request::handleChunkedRequest(std::string requestMsg)
{
	std::string treatedBody;
	if (requestParsed == false)
	{
		chunkBegin = true;
		storeRequestBodyToString(requestMsg);
		treatBodyString(body);
	}
	else
		treatBodyString(requestMsg);
}

void Request::handlePostMethod(std::string requestMsg, int received)
{
	if (checkUploadDirectory())
	{
		if (headers.find("Transfer-Encoding") != headers.end())
			handleChunkedRequest(requestMsg);
		else
			storeRequestBodyToFile(requestMsg, received);
	}
	else
		statusCode = 403;
}

void Request::matchLocation(Server &server)
{
    std::list<std::string>          routes;
    size_t                          pos = 0;
    size_t                          lastSlash = uri.length();

	if (*(uri.end() - 1) != '/')
	{
        routes.push_back(uri + '/');
	}
    while (lastSlash)
    {
        lastSlash = uri.find_last_of("/", lastSlash - 1);
        routes.push_back(uri.substr(0, lastSlash + 1));
    }

    std::list<std::string>::iterator it = routes.begin();

    for(; it != routes.end(); it++)
    {
        if(server.getAllLocations().find(*it) != server.getAllLocations().end())
        {    
            std::map<std::string, Location>::iterator locationIt = server.getAllLocations().find(*it);
            location = &(locationIt->second); 
            break;
        }
    }
	uploadPass = location->upload_pass;
}

void Request::requestParser(std::string requestMsg, size_t bodySize, int received, Server &server)
{
	std::string line;
	std::istringstream ss(requestMsg);
	std::getline(ss, line);

	firstLineParser(line);
	setHeaders(line, ss);

	matchLocation(server);
	setStatusCode(bodySize);
	if (statusCode != 0)
	{
		requestParsed = true;
		return;
	}
	if (method == "POST")
		handlePostMethod(requestMsg, received);
	requestParsed = true;
}

int Request::isTransferEncodingHeaderChunked(void)
{
	if (headers.find("Transfer-Encoding") != headers.end()) 
	{
    	std::string transferEncoding = headers["Transfer-Encoding"];
        if (transferEncoding != "chunked") 
    	    return 0;
		else 
			return 1;
    }
	return 1;
}

int Request::checkPost(void)
{
    if (method == "POST") 
	{
        if (headers.find("Transfer-Encoding") == headers.end() && headers.find("Content-Length") == headers.end())
            return 0;
    }
    return 1;
}

int Request::checkUriLength(std::string uri)
{
	if (uri.length() > 2048)
		return 0;
	else
		return 1;
}

int Request::isRequestBodyLarge(size_t bodySize)
{
	if (method == "POST" && headers.find("Content-Length") != headers.end() && ft_stoi(headers["Content-Length"], 10) > bodySize)
		return 0;
	else
		return 1;
}

bool Request::checkAllowMethods()
{
    if ((method == "GET") && (location->methods == 3 || location->methods == 7 || location->methods == 9 || location->methods == 13))
        return false;
    else if ((method == "POST") && (location->methods == 4 || location->methods == 7 || location->methods == 10 || location->methods == 13))
        return false;
    else if ((method == "DELETE") && (location->methods == 6 || location->methods == 9 || location->methods == 10 || location->methods == 13))
        return false;
    return true;
}

void Request::setStatusCode(size_t bodySize)
{
	if (!isTransferEncodingHeaderChunked())
		statusCode = 501;
	else if (!checkPost())
		statusCode = 400;
	else if (!checkUriChars(uri))
		statusCode = 400;
	else if (!checkUriLength(uri))
		statusCode = 414;
	else if (!isRequestBodyLarge(bodySize))
		statusCode = 413;
	else if(!checkMethod(method))
		statusCode = 405;
	else if (checkAllowMethods())
		statusCode = 405;
	else
		statusCode = 0;
} 