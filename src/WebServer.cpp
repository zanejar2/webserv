 #include "WebServer.hpp"

WebServer::WebServer(const Config &config) : max_fd (0){
	this->config = config;
}

WebServer::WebServer(): client(NULL)
{
}

void  WebServer::createSocket(Server &server)
{
    struct addrinfo hints;
    struct addrinfo *bind_address;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(server.getHost().c_str(), server.getListen().c_str(), &hints, &bind_address);

    int socket_listen;

    socket_listen = socket(bind_address->ai_family,
    bind_address->ai_socktype, bind_address->ai_protocol);
    
    if (socket_listen == -1) {
        exit(1);
    }


    int yes = 1;
    setsockopt(socket_listen, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)); 
    
    // fcntl(socket_listen, F_SETFL, O_NONBLOCK);
    
    if (bind(socket_listen,
        bind_address->ai_addr, bind_address->ai_addrlen)) {
        
        std::cout << "Address: " << server.getHost().c_str() <<  ", Port: " << server.getListen().c_str() << std::endl;
        exit(1);
    }
    freeaddrinfo(bind_address);
    
    if (listen(socket_listen, MAX_FD) < 0) {
        exit(1);
    }
    server.socket = socket_listen;
}

void WebServer::Build()
{
    std::vector<Server>::iterator it = config.server.begin();
    for(;it != config.server.end(); it++)
    {
        createSocket(*it);
    }
}

void WebServer::Run()
{
    while (1)
    {
        fd_set          readFds;
        fd_set          writeFds;
        struct timeval  timeout;
        
        FD_ZERO(&readFds);
        FD_ZERO(&writeFds);
        
        timeout.tv_sec = 0;
        timeout.tv_usec = 1;
        setSockets(&readFds, &writeFds);

        if (select(max_fd + 1, &readFds, &writeFds, NULL, &timeout) == -1)
        {
			exit(1);
        }
        AcceptNewConnection(&readFds, &writeFds);

        HandleRequest(&readFds, &writeFds);
    }
}

void WebServer::HandleRequest(fd_set *readFds, fd_set *writeFds)
{
    std::vector<Server>::iterator                           it = config.server.begin();
    std::vector<int>                                        droppedClients;

	
    for(;it != config.server.end(); it++)
    {
        std::map<int, Client>::iterator it1 = it->client.begin();
        for(;it1 != it->client.end(); it1++)
        {
            if (FD_ISSET(it1->first, readFds))
            {
                char buffer[BUFFER_SIZE + 1];
                
                bzero(buffer, BUFFER_SIZE);
                it1->second.request.received = recv(it1->first, buffer, BUFFER_SIZE, 0);
				it1->second.request.requestSize += it1->second.request.received;
                if (it1->second.request.received > 0)
                {
                    std::string strBuffer(buffer, it1->second.request.received);
					if (!it1->second.request.requestParsed)
				  		it1->second.request.requestParser(strBuffer, it->getBodySize(), it1->second.request.received, *it);
					else if (it1->second.request.method == "POST" \
					&& it1->second.request.headers.find("Transfer-Encoding") == it1->second.request.headers.end() \
					&& it1->second.request.statusCode == 0)
					{
						it1->second.request.file.write(strBuffer.c_str(), it1->second.request.received);
						if (it1->second.request.requestSize == it1->second.request.ft_stoi(it1->second.request.headers["Content-Length"], 10))
						{
							it1->second.request.statusCode = 201;
                            it1->second.request.file.close();
						}
					}
					else if (it1->second.request.method == "POST" \
					&& it1->second.request.headers.find("Transfer-Encoding") != it1->second.request.headers.end() \
					&& it1->second.request.statusCode == 0)
					{
						it1->second.request.handleChunkedRequest(strBuffer);
						if (it1->second.request.chunkedRequsestBodyDone)
						{	
                            it1->second.request.statusCode = 201;
                            it1->second.request.file.close();
                        }
					}
                }
                else if (it1->second.request.received <= 0)
                {
                    FD_CLR(it1->first, writeFds);
                    FD_CLR(it1->first, readFds);
                    droppedClients.push_back(it1->first);
                }
            }
            else if (FD_ISSET(it1->first, writeFds))
            {
                if (it1->second.response.Serving(it1->first, *it))
                {
                    FD_CLR(it1->first, writeFds);
                    FD_CLR(it1->first, readFds);
                    droppedClients.push_back(it1->first);
                }
            }
        }
    }

    std::vector<int>::iterator itr = droppedClients.begin();
    for(;itr != droppedClients.end(); itr++)
        dropClient(*itr);
}


void WebServer::dropClient(const int &clientFd)
{
    std::vector<Server>::iterator it = config.server.begin();
    close(clientFd);
    for(;it != config.server.end(); it++)
        it->client.erase(clientFd);
}

void WebServer::setSockets(fd_set *readFds, fd_set *writeFds)
{
    std::vector<Server>::iterator it = config.server.begin();

    for(;it != config.server.end(); it++)
    {
        FD_SET(it->socket, readFds);
        FD_SET(it->socket, writeFds);
        
        if (max_fd < it->socket)
            max_fd = it->socket;


        std::map<int, Client>::iterator it1 = it->client.begin();
        for(;it1 != it->client.end(); it1++)
        {
            FD_SET(it1->first, readFds);
            FD_SET(it1->first, writeFds);
            if (it1->first > max_fd)
                max_fd = it1->first;
        }
    } 
}

void    WebServer::AcceptNewConnection(fd_set *readFds, fd_set *writeFds)
{
    std::vector<Server>::iterator it = config.server.begin();

    sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);

    for(;it != config.server.end(); it++)
    {

        if (FD_ISSET(it->socket, readFds))
        {
            int client_fd;

            client_fd = accept(it->socket, (struct sockaddr*)&clientAddress, (socklen_t *)&clientAddressLength);
            if (client_fd == -1)
            {
                exit(1);
            }
            FD_SET(client_fd, readFds);
            FD_SET(client_fd, writeFds);
            if (client_fd > max_fd)
                max_fd = client_fd;
            it->client.insert(std::make_pair(client_fd, Client()));
            return ;
        }
    }
}