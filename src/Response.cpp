#include "Response.hpp"
#include "Server.hpp"
#include <string.h>
#include <dirent.h>
#include <list>

//  nettop -t loopback
//  lsof -i:8080
Response::Response(Request &request): request(&request), bytesSize(0), CgiAccess(true), Cgiread(true)
{
    sendBytes = NULL;
	errorResponse[404] = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: 67\r\nConnection: close\r\nServer: Webserv\r\n\r\n<html><body><h1><center> 404 Not Found </center></h1></body></html>";
	errorResponse[400] = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: 50\r\nConnection: close\r\nServer: Webserv\r\n\r\n<html><body><h1>400 Bad Request</h1></body></html>";
	errorResponse[403] = "HTTP/1.1 403 Forbidden\r\nContent-Type: text/html\r\nContent-Length: 48\r\nConnection: close\r\nServer: Webserv\r\n\r\n<html><body><h1>403 Forbidden</h1></body></html>\r\n";
	errorResponse[405] = "HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/html\r\nContent-Length: 57\r\nConnection: close\r\nServer: Webserv\r\n\r\n<html><body><h1>405 Method Not Allowed</h1></body></html>\r\n";
	errorResponse[408] = "HTTP/1.1 408 Request Timeout\r\nContent-Type: text/html\r\nContent-Length: 54\r\nConnection: close\r\nServer: Webserv\r\n\r\n<html><body><h1>408 Request Timeout</h1></body></html>";
	errorResponse[502] = "HTTP/1.1 502 Bad Gateway\r\nContent-Type: text/html\r\nContent-Length: 50\r\nConnection: close\r\nServer: Webserv\r\n\r\n<html><body><h1>502 Bad Gateway</h1></body></html>";
	errorResponse[501] = "HTTP/1.1 501 Not Implemented\r\nContent-Type: text/html\r\nContent-Length: 54\r\nConnection: close\r\nServer: Webserv\r\n\r\n<html><body><h1>501 Not Implemented</h1></body></html>";
	errorResponse[409] = "HTTP/1.1 409 Conflict\r\nContent-Type: text/html\r\nContent-Length: 47\r\nConnection: close\r\nServer: Webserv\r\n\r\n<html><body><h1>409 Conflict</h1></body></html>";
	errorResponse[204] = "HTTP/1.1 204 No Content\r\nContent-Type: text/html\r\nContent-Length: 49\r\nConnection: close\r\nServer: Webserv\r\n\r\n<html><body><h1>204 No Content</h1></body></html>";
	errorResponse[413] = "HTTP/1.1 413 Request Entity Too Large\r\nContent-Type: text/html\r\nContent-Length: 63\r\nConnection: close\r\nServer: Webserv\r\n\r\n<html><body><h1>413 Request Entity Too Large</h1></body></html>";
    errorResponse[414] = "HTTP/1.1 414 Request-URI-Long\r\nContent-Type: text/html\r\nContent-Length: 59\r\nConnection: close\r\nServer: Webserv\r\n\r\n<html><body><h1>414 Request-URI Too Long</h1></body></html>";
    errorResponse[500] = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\nContent-Length: 60\r\nConnection: close\r\nServer: Webserv\r\n\r\n<html><body><h1>500 Internal Server Error</h1></body></html>";

}

Response::Response() 
{

}

Response::~Response()
{
    file.close();
}

std::string Response::getContentType(const char* path)
{
    const char *last_dot = strrchr(path, '.');
    
    if (last_dot) 
    {
        if (strcmp(last_dot, ".css") == 0) return "Content-Type: text/css\r\n";
        if (strcmp(last_dot, ".csv") == 0) return "Content-Type: text/csv\r\n";
        if (strcmp(last_dot, ".gif") == 0) return "Content-Type: image/gif\r\n";
        if (strcmp(last_dot, ".htm") == 0) return "Content-Type: text/html\r\n";
        if (strcmp(last_dot, ".html") == 0) return "Content-Type: text/html\r\n";
        if (strcmp(last_dot, ".ico") == 0) return "Content-Type: image/x-icon\r\n";
        if (strcmp(last_dot, ".jpeg") == 0) return "Content-Type: image/jpeg\r\n";
        if (strcmp(last_dot, ".jpg") == 0) return "Content-Type: image/jpeg\r\n";
        if (strcmp(last_dot, ".js") == 0) return "Content-Type: application/javascript\r\n";
        if (strcmp(last_dot, ".json") == 0) return "Content-Type: application/json\r\n";
        if (strcmp(last_dot, ".png") == 0) return "Content-Type: image/png\r\n";
        if (strcmp(last_dot, ".pdf") == 0) return "Content-Type: application/pdf\r\n";
        if (strcmp(last_dot, ".svg") == 0) return "Content-Type: image/svg+xml\r\n";
        if (strcmp(last_dot, ".txt") == 0) return "Content-Type: text/plain\r\n";
        if (strcmp(last_dot, ".mp4") == 0) return "Content-Type: video/mp4\r\n";
		if (strcmp(last_dot, ".php") == 0) return "Content-Type: text/html\r\n";
		if (strcmp(last_dot, ".py") == 0) return "Content-Type: text/html\r\n";
    }

    return "Content-Type: application/octet-stream\r\n";
}

void Response::getBody()
{
    bzero(buffer, 1024);
    file.read(buffer, 1024);

    if (file.eof())
    {
        file.close();
        drop = true;
    }
    sendBytes = buffer;
    bytesSize = file.gcount();
}


bool Response::checkAllowMethods(int method)
{
    if ((request->method == "GET") && (method == 3 || method == 7 || method == 9 || method == 13))
        return false;
    else if ((request->method == "POST") && (method == 4 || method == 7 || method == 10 || method == 13))
        return false;
    else if ((request->method == "DELETE") && (method == 6 || method == 9 || method == 10 || method == 13))
        return false;
    return true;
}

bool Response::checkLocation(Server &server)
{
    std::list<std::string>          routes;
    size_t                          pos = 0;
    size_t                          lastSlash = request->uri.length();
	bool					        red = false;
	std::string				        red_path;
    size_t                          findQmark = request->uri.find('?');

    if (findQmark != std::string::npos)
    {
            std::string new_uri = request->uri.substr(0, findQmark);
            request->queryString = request->uri.substr(findQmark + 1);
            request->uri = new_uri;
    }

    if (*(request->uri.end() - 1) != '/')
	{
        routes.push_back(request->uri + '/');
	}
    while (lastSlash)
    {
        lastSlash = request->uri.find_last_of("/", lastSlash - 1);
        routes.push_back(request->uri.substr(0, lastSlash + 1));
    }

    std::list<std::string>::iterator it = routes.begin();

    for(; it != routes.end(); it++)
    {
        if(server.getAllLocations().find(*it) != server.getAllLocations().end())
        {    
            std::map<std::string, Location>::iterator locationIt = server.getAllLocations().find(*it);
            locationName = locationIt->first;
			location = &(locationIt->second);
            if (locationIt != server.getAllLocations().end())
            {

                path = locationIt->second.root;
                if (locationIt->first == request->uri && !locationIt->second.index.empty())
                    path += locationIt->second.index;
                else
                    path += std::string(request->uri).erase(0, it->length());
				if (!(locationIt->second.redirect.empty()))
				{
					red = true;
					red_path = locationIt->second.redirect;
				}
            }
            break;
        }
    }
    if (checkAllowMethods(location->methods))
    {
        SendResponse(405, server);
        return true;
    }
        
	if (red)
	{
		//check location path : it add a / in the front of path
		response = "HTTP/1.1 301 Moved Permanently\r\n";
        response += "location: " +  red_path + "\r\n";
        response += "Connection: close\r\n\r\n";
        sendBytes = response.c_str();
        bytesSize = response.length();

		drop = true;
		return(true);
	}
    if (path.empty())
    {
        SendResponse(404, server);
        return (true);
    }
    else
        return false;
}

std::string Response::errorBody(int statuscode)
{
	switch(statuscode){
		case 400:
			return "<!DOCTYPE html><html><head> <title>400 Bad Request</title></head><body><h1>400 Bad Request</h1><p>Your request could not be understood by the server due to malformed syntax or invalid parameters.</p></body></html>";
		case 204:
			return "<!DOCTYPE html><html><head><title>204 No Content</title></head><body><h1>204 No Content</h1><p>The server successfully processed the request and is not returning any content.</p></body></html>";
		case 401:
			return "<!DOCTYPE html><html><head><title>401 Unauthorized</title></head><body><h1>401 Unauthorized</h1><p>The request requires user authentication. Please provide valid credentials to access the requested resource.</p></body></html>";
		case 403:
			return "<!DOCTYPE html><html><head><title>403 Forbidden</title></head><body><h1>403 Forbidden</h1><p>You do not have permission to access the requested resource.</p></body></html>";
		case 404:
			return "<!DOCTYPE html><html><head><title>404 Not Found</title></head><body><h1>404 Not Found</h1><p>The requested resource could not be found on this server.</p></body></html>";
		case 405:
			return "<!DOCTYPE html><html><head><title>405 Method Not Allowed</title></head><body><h1>405 Method Not Allowed</h1><p>The method specified in the request is not allowed for the requested resource.</p></body></html>";
		case 406:
			return "<!DOCTYPE html><html><head><title>406 Not Acceptable</title></head><body><h1>406 Not Acceptable</h1><p>The server cannot produce a response matching the list of acceptable values defined by the client.</p></body></html>";
		case 407:
			return "<!DOCTYPE html><html><head><title>407 Proxy Authentication Required</title></head><body><h1>407 Proxy Authentication Required</h1><p>The request requires proxy authentication. Please provide valid proxy credentials to access the requested resource.</p></body></html>";
		case 408:
			return "<!DOCTYPE html><html><head><title>408 Request Timeout</title></head><body><h1>408 Request Timeout</h1><p>The server timed out waiting for the request.</p></body></html>";
		case 409:
			return "<!DOCTYPE html><html><head><title>409 Conflict</title></head><body><h1>409 Conflict</h1><p>There was a conflict while processing your request.</p></body></html>";
		case 410:
			return "<!DOCTYPE html><html><head><title>410 Gone</title></head><body><h1>410 Gone</h1><p>The requested resource is no longer available.</p></body></html>";
		case 411:
			return "<!DOCTYPE html><html><head><title>411 Length Required</title></head><body><h1>411 Length Required</h1><p>The server requires a Content-Length header in the request.</p></body></html>";
		case 412:
			return "<!DOCTYPE html><html><head><title>412 Precondition Failed</title></head><body><h1>412 Precondition Failed</h1><p>The server does not meet the conditions specified by the client in the request headers.</p></body></html>";
		case 413:
			return "<!DOCTYPE html><html><head><title>413 Payload Too Large</title></head><body><h1>413 Payload Too Large</h1><p>The request is larger than the server is willing or able to process.</p></body></html>";
		case 414:
			return "<!DOCTYPE html><html><head><title>414 URI Too Long</title></head><body><h1>414 URI Too Long</h1><p>The requested URL is too long for the server to process.</p></body></html>";
		case 500:
			return "<!DOCTYPE html><html><head><title>500 Internal Server Error</title></head><body><h1>500 Internal Server Error</h1><p>The server encountered an unexpected condition that prevented it from fulfilling the request.</p></body></html>";
		case 501:
			return "<!DOCTYPE html><html><head><title>501 Not Implemented</title></head><body><h1>501 Not Implemented</h1><p>The server does not support the functionality required to fulfill the request.</p></body></html>";
		case 502:
			return "<!DOCTYPE html><html><head><title>502 Bad Gateway</title></head><body><h1>502 Bad Gateway</h1><p>The server received an invalid response from an upstream server.</p></body></html>";
		case 503:
			return "<!DOCTYPE html><html><head><title>503 Service Unavailable</title></head><body><h1>503 Service Unavailable</h1><p>The server is currently unable to handle the request due to temporary overloading or maintenance of the server.</p></body></html>";
		case 504:
			return "<!DOCTYPE html><html><head><title>504 Gateway Timeout</title></head><body><h1>504 Gateway Timeout</h1><p>The server, acting as a gateway or proxy, did not receive a timely response from an upstream server.</p></body></html>";
	}
	return NULL;
}

std::string statusCodeString(int statusCode)
{
    switch (statusCode)
    {
	    case 200:
            return "OK";
        case 201:
            return "Created";
        case 202:
            return "Accepted";
        case 204:
            return "No Content";
        case 205:
            return "Reset Content";
        case 206:
            return "Partial Content";
        case 300:
            return "Multiple Choice";
        case 301:
            return "Moved Permanently";
        case 302:
            return "Moved Temporarily";
        case 400:
            return "Bad Request";
        case 401:
            return "Unauthorized";
        case 403:
            return "Forbidden";
        case 404:
            return "Not Found";
        case 405:
            return "Method Not Allowed";
        case 406:
            return "Not Acceptable";
        case 407:
            return "Proxy Authentication Required";
        case 408:
            return "Request Timeout";
        case 409:
            return "Conflict";
        case 410:
            return "Gone";
        case 411:
            return "Length Required";
        case 412:
            return "Precondition Failed";
        case 413:
            return "Payload Too Large";
        case 414:
            return "URI Too Long";
        case 500:
            return "Internal Server Error";
        case 501:
            return "Not Implemented";
        case 502:
            return "Bad Gateway";
        case 503:
            return "Service Unavailable";
        case 504:
            return "Gateway Timeout";
        default:
            return "Undefined";
        }
}

std::string toString(int val)
{
    std::stringstream stream;
    stream << val;
    return stream.str();
}

void Response::Headers(int statuscode)
{
	response = "HTTP/1.1 " + toString(statuscode) + " " + statusCodeString(statuscode) + "\r\n" + "Content-Type: text/html\r\n" + \
			"Content-Length: " + toString(errorBody(statuscode).length()) + "\r\n\r\n";
	response += errorBody(statuscode);
	sendBytes = response.c_str();
    bytesSize = response.length();
	drop = true;
}

void Response::geterrorbody()
{
    bzero(buffer, 1024);
    error_file.read(buffer, 1024);

    if (error_file.eof())
    {
        error_file.close();
        drop = true;
    }
    
    sendBytes = buffer;
    bytesSize = error_file.gcount();
}

int ft_stringtoi(std::string &str)
{
    std::stringstream ss(str);
    if (str.length() > 10)
       throw std::runtime_error("MAX Body Size");
    for (size_t i = 0; i < str.length(); ++i)
        if(!isdigit(str[i]))
             throw std::runtime_error("Body Size");
    int res;
    ss >> res;
    return (res);
}

bool PathErrorValid(std::string& path) 
{
    if (access(path.c_str(), F_OK) != -1)
        return true;
    return false;
}

std::size_t getFileSize(std::string& filename) {
    struct stat st;
    if (stat(filename.c_str(), &st) != 0) {
        return 0;
    }
    return st.st_size;
}

bool Response::CheckStatusCode(int status)
{
	if (status >= 400 && status <= 504)
		return (true);
	return (false);
}


void	Response::BuildErrorPage(std::string &str, int statu)
{
	if (!error_file.is_open())
	{
		error_file.open(str, std::ios::in | std::ios::binary);
		response = "HTTP/1.1 " + toString(statu) + " " + statusCodeString(statu) + "\r\n" + getContentType(str.c_str()) + \
					"Content-Length: " + toString(getFileSize(str)) + "\r\n\r\n";
		sendBytes = response.c_str();
        bytesSize = response.length();
		drop = false;
	}
	else
		geterrorbody();
}

void	Response::SendResponse(int statcode, Server &server)
{
	std::map<int , std::string> error;
	error = server.getErrorPage();
	std::map<int, std::string>::iterator it = error.begin();
	if (!(it->second.empty()) && error.find(statcode) != error.end())
	{
		it = error.find(statcode);
		if (PathErrorValid(it->second))
			BuildErrorPage(it->second, it->first);
		else
			Headers(statcode);
	}
	else
	{
		Headers(statcode);
	}
}

bool Response::Serving(int clientFd, Server &server)
{
    // Location check
	if (request->statusCode && CheckStatusCode(request->statusCode))
	{
		SendResponse(request->statusCode, server);
		goto send;
	}
    if (!file.is_open())
    {
        if (checkLocation(server))
            goto send;
        if (stat(path.c_str(), &st) == -1)
        {
            SendResponse(404, server);
            goto send;
        }
    }
    // respond
    if (!request->method.compare("GET"))
        GetRespond(server);
    else if (!request->method.compare("POST"))
        PostRespond(server);
    else if (!request->method.compare("DELETE"))
        DeleteRespond(server);
send:
    size_t sended = send(clientFd, sendBytes, bytesSize, 0);
	if (sended == -1)
		return (true);
	else if (sended == 0)
		return (false);
    if (drop)
        return (true);
    else
        return (false);
}

std::string parseCgiRequest(char* CgiRequest) {
    std::string body;

    char* headersEnd = std::strstr(CgiRequest, "\r\n\r\n");
    if (headersEnd == nullptr) {
        headersEnd = std::strstr(CgiRequest, "\n\n");
    }

    if (headersEnd != nullptr) {
        int headersLength = headersEnd - CgiRequest;
        std::string headers(CgiRequest, headersLength);

        body = headersEnd + 4;
    }
    return body;
}

std::string Response::ft_countsize(std::string str)
{
    FILE* file;
    std::string tmp;
    file = fopen(str.c_str(), "r");
    if (file != NULL)
    {
        if (fseek(file, 0, SEEK_END) == 0)
        {
           size_t size = ftell(file);
           std::stringstream ptr;
           ptr << size;
           ptr >> tmp;
        }
        fclose(file);
    }
    return tmp;
}

std::string ft_countsizee(std::string str, size_t find)
{
    FILE* file;
    std::string tmp;
    file = fopen(str.c_str(), "r");
    if (file != NULL)
    {
        if (fseek(file, 0, SEEK_END) == 0)
        {
           size_t size = ftell(file);
           size -= find;
           std::stringstream ptr;
           ptr << size;
           ptr >> tmp;
        }
        fclose(file);
    }
    return tmp;
}

size_t readhaedercgi(std::string path)
{
    char buffer[1024];
    std::ifstream file;
    file.open(path, std::ios::in);
    std::string header;
    size_t find = 0;
    while (file.read(buffer, 1024))
    {
        std::string b(buffer, sizeof(buffer));
        header += b;
        if (header.find("\r\n\r\n") != std::string::npos)
        {
            find = header.find("\r\n\r\n");
            find += 4;
            break;
        }

    }
    file.close();
    return (find);
}

void Response::ResponseCgi(Server &server)
{
    size_t find = 0;
    if (!file.is_open())
    {
        file.open(CgiPath, std::ios::in);
        response = "HTTP/1.1 200 OK\r\n";
        if (path.find(".php") != std::string::npos)
            find = readhaedercgi(CgiPath);
        else
            response += "Content-type: text/html\r\n";
        response += "Content-Length: "  + ft_countsizee(CgiPath, find) + "\r\n";
        if (path.find(".py") != std::string::npos)
            response += "\r\n";
        sendBytes = response.c_str();
        bytesSize = response.length();
    }
    else
    {
        bzero(buffer, 1024);
        file.read(buffer, 1024);
        sendBytes = buffer;
        bytesSize = file.gcount();
        if (file.eof())
        {
            file.close();
            CgiAccess = true;
            drop = true;
        }
    }
}

void Response::serveTheFile(Server &server)
{
    if (access(path.c_str(), R_OK) == -1)
    {
        SendResponse(403, server);
    }
	else if (location->cgi && !access(path.c_str(), X_OK) && ((path.find(".php") != std::string::npos || path.find(".py") != std::string::npos)))
    {
        if (CgiAccess)
        {
            HandleCgi(server);
            drop = false;
        }
        if(waitpid(this->cgi_pid, NULL, WNOHANG) != 0 && location->cgi)
            ResponseCgi(server);
    }
    else if (!file.is_open())
    {
        file.open(path, std::ios::in | std::ios::binary);
        response = "HTTP/1.1 200 OK\r\n";
        response += getContentType(path.c_str());
        response += "Content-Length: " + ft_countsize(path) + "\r\n\r\n";
        drop = false;
        sendBytes = response.c_str();
        bytesSize = response.length();
    }
    else
        getBody();
}

void Response::generateHtmlListing(Server &server)
{
    std::string html;
    DIR *hDirectory = opendir(path.c_str());
    
    if (hDirectory == NULL)
    {
		SendResponse(403, server);
        return ;
    }

    html += "<html><head><title>Directory Listing</title></head><body>";
    html += "<h1>Directory Listing</h1>";
    html += "<ul>";

    struct dirent* entry;
    while ((entry = readdir(hDirectory)) != NULL) {
        html += "<li><td> <a href=\"";
        html += entry->d_name;
        
        html += "\">";
        html += entry->d_name;
        html += "</a> </td>";
        html += "</li>";
    }

    html += "</ul>";
    html += "</body></html>";

    html = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(html.length()) + "\r\n\r\n" + html;
    
    sendBytes = html.c_str();
    bytesSize = html.length();
    closedir(hDirectory);
}

void Response::GetRespond(Server &server)
{
	
	if (S_ISREG(st.st_mode))  // File Handling
    {
        serveTheFile(server);
    }
    else if (S_ISDIR(st.st_mode)) // Directory Handling
    {
        if (*(request->uri.end() - 1) != '/')
        {
            // 301 responses
            response = "HTTP/1.1 301 Moved Permanently\r\n";
            response += "Location: " + request->uri + "/\r\n";
            response += "Connection: close\r\n\r\n";
            sendBytes = response.c_str();
            bytesSize = response.length();
            drop = true;
        }
        else if (locationName == request->uri && !location->index.empty())
		    serveTheFile(server);
        else if (locationName == request->uri && location->auto_index)
            generateHtmlListing(server);
        else
		{
			SendResponse(403, server);
		}
	}
    else
        SendResponse(403, server);
}

void Response::PostRespond(Server &server)
{
    if (location->cgi && !access(path.c_str(), X_OK) && ((path.find(".php") != std::string::npos || path.find(".py") != std::string::npos)))
    {
        if (CgiAccess)
        {
            HandleCgi(server);
            drop = false;
        }
        if(waitpid(this->cgi_pid, NULL, WNOHANG) != 0 && location->cgi)
        {
            ResponseCgi(server);
            std::remove(CgiPath.c_str());
        }
    }
	else if (request->statusCode == 201)
	{
		response = "HTTP/1.1 201 Created\r\n\r\n";
    	drop = true;
    	sendBytes = response.c_str();
    	bytesSize = response.length();
		request->file.close();
	}
}

bool	DirectoryAccess(const char *path)
{
	if (access(path, R_OK | X_OK) == 0)
		return (true);
	else
		return (false);
}

bool	IsDirectory(const char *path)
{
	DIR* dir;
	if (dir)
		return (true);
	else
		return (false);
}

void Response::DeleteRespond(Server &server)
{
	if (S_ISREG(st.st_mode))
	{
		if (access(path.c_str(), F_OK | R_OK) == -1)
		{
			SendResponse(403, server);
		}
		else
		{
			std::remove(path.c_str());
			SendResponse(204, server);
		}
	}
	else if (S_ISDIR(st.st_mode))
	{
		if (path[path.size() - 1] != '/')
			SendResponse(409, server);
		else
			SendResponse(403, server);
	}
}
