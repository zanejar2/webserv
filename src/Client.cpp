#include "Client.hpp"

Client::Client(): response(request)
{

}

Client::Client(const Client &other): response(request)
{

}

Client Client::operator=(const Client &other)
{
    return (*this);
}
