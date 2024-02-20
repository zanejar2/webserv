#include "Server.hpp"

//setConfiguration

void Server::setHost(const std::string &host) {this->host = host;}

void Server::setListen(const std::string &listen) {this->listen = listen;}

void Server::setBodySize(const size_t body_size) {this->body_size = body_size;}

void Server::setErrorPage(int Key, std::string Path) {this->error_page[Key] = Path;}

void Server::setServerName(const std::string &server_name) {this->server_name = server_name;}

void Server::addLocation(std::string path, Location loc)
{
    if (path[path.length() - 1] != '/')
        path += '/';
    this->location[path] = loc;

}

//getConfiguration

std::string Server::getHost() const{return this->host;}

std::string Server::getListen() const{return this->listen;}

std::string Server::getServerName() const{return this->server_name;}

size_t	Server::getBodySize() const {return this->body_size;}

std::map<int, std::string>	Server::getErrorPage(){return this->error_page;}

std::string	Server::getKeyErrorPage(const int Key)
{
    std::map<int, std::string>::iterator it = error_page.find(Key);
    if (it != error_page.end()) {
        return it->second;
    }
	return "";
}

std::map<std::string, Location> &Server::getAllLocations() {return location;}

Location &Server::getLocations(std::string path)
{
	std::map<std::string, Location>::iterator it = location.find(path);
    if (it != location.end()) {
        return it->second;
    }
	return it->second;
}