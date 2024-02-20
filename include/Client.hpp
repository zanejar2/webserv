#ifndef _CLIENT_
# define _CLIENT_

#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "Request.hpp"
#include "Response.hpp"

class Client
{

    public:
        Request     request;        
        Response    response;

    public:
        Client();
        Client(const Client&);
        Client operator=(const Client&);

};

#endif