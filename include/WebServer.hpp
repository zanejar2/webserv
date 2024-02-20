#ifndef _WEB_SERVER_
# define _WEB_SERVER_

#include "Config.hpp"
#include "Server.hpp"
#include <utility>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <string>
#include <algorithm>
#include <fcntl.h>

#define MAX_FD      1024
#define BUFFER_SIZE 1024
#define BUFFER_MAX 1000000

class WebServer
{
    public:
        WebServer(const Config &config);
        WebServer();

        Config                          config;
        size_t                          max_fd;
        std::pair<const int, Client>    *client;
		Request							*request;

    public:
        void    Build();
        void    Run();

    private:
        void                        setSockets(fd_set*, fd_set*);
        void                        AcceptNewConnection(fd_set*, fd_set*);
        void                        createSocket(Server &server);
        void                        HandleRequest(fd_set*, fd_set*);
        void                        dropClient(const int&);
        void                        sendTheRest(fd_set*);
        
        // bool HandleRequestParsing(int socket, Client& client, size_t bodySize);
        // bool HandleHeadersCompletion(const std::string& buffer, std::string& requestHeaders, Request& request, size_t bodySize);
        // void HandleRequestBody(const std::string& buffer, const std::string& requestHeaders, std::string& requestBody,
        // bool& saveBodyToFile, std::ofstream& bodyFile, Request& request, bool& requestCompleted);
        // void HandleRemainingBody(const std::string& buffer, const std::string& strBuffer, std::ofstream& bodyFile);

};

#endif