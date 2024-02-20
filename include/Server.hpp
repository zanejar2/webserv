#ifndef _SERVER_
# define _SERVER_

#include <map>
#include <string>
#include "Client.hpp"
#include <vector>
#include <utility>
#include <string>

struct Location{
		
	int													methods;
    bool	 											auto_index;
	bool												cgi;
    std::string											index;
    std::string											root;
    std::string											upload_pass;
	std::string											redirect;
    std::vector<std::pair <std::string, std::string> >	cgi_pass;
};

class Server
{
    private:
        size_t		                   		body_size;
        std::string                    		host;
        std::string                    		listen;
		std::string							server_name;
        std::map<int , std::string> 		error_page;
        std::map<std::string, Location>		location;

    public:

    	int                             socket;
    	std::map<int, Client>           client;
       
		Server() {};
		~Server() {};
		Server(const Server& obj):error_page(obj.error_page), location(obj.location), client(obj.client),
			host(obj.host), server_name(obj.server_name), listen(obj.listen), socket(obj.socket), body_size(obj.body_size){
			}

		Server operator=(const Server& obj)
		{
			this->error_page = obj.error_page;
			this->host = obj.host;
			this->listen = obj.listen;
			this->server_name = obj.server_name;
			this->body_size = obj.body_size;
			this->location = obj.location;
			return *this;
		}

	
		void setHost		(const std::string &host);
		void setListen		(const std::string &listen);
		void setServerName	(const std::string &server_name);
		void setBodySize	(const size_t body_size);
		void setErrorPage	(int Key, std::string Path);
		void addLocation	(std::string path, Location loc);


		size_t 								getBodySize() const;
		std::string 						getHost() const;
		std::string 						getListen() const;
		std::string 						getServerName() const;
		std::string 						getKeyErrorPage(const int Key);
		std::map<int, std::string>	 		getErrorPage();
		std::map<std::string, Location>		&getAllLocations();
		Location							&getLocations(std::string path);
};
#endif