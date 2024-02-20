#ifndef _REQUEST_
# define _REQUEST_

#include <map>
#include <string>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <unistd.h>

class Server;
class Location;

class Request
{
    private:
        bool                                nflag;
    public:
        std::string                         method;
        std::string                         uri;
		std::string							version;
        std::string                         queryString;
        std::map<std::string, std::string>  headers;
		int									statusCode;
		bool 								requestParsed;
		std::string							body;
		std::ofstream						file;
		int 								hexaReceived;
		std::string							restBody;
		bool								chunkBegin;
		std::string							appendBody;
		std::string							postedFileName;
		int 								received;
		bool								chunkedRequsestBodyDone;
		int									requestSize;
		std::string							UploadPath;
		std::string							extension;
		std::string							uploadPass;
		Location							*location;


    public:
        Request(/* args */): nflag(false) ,requestParsed(false), restBody(""), appendBody(""), received(0), chunkedRequsestBodyDone(false), requestSize(0), postedFileName("") {}
        Request(const Request &other);
        ~Request();

    public:
		void requestParser(std::string requestMsg, size_t body_size, int received, Server &server);
		void firstLineParser(std::string line);
		int checkMethod(std::string method);
		int checkUriChars(std::string uri);
        void setStatusCode(size_t bodySize);
		void setHeaders(std::string line, std::istringstream &ss);
        int isTransferEncodingHeaderChunked(void);
        int checkPost(void);
        int checkUriLength(std::string uri);
		int isRequestBodyLarge(size_t bodySize);
		void storeRequestBodyToString(const std::string& request);
		void storeRequestBodyToFile(const std::string& request, int received);
		std::string fileExtentionDefiner();
		void handlePostMethod(std::string requestMsg, int received);
		int checkUploadDirectory(void);
		void handleChunkedRequest(std::string requestMsg);
		void treatBodyString(std::string requestBody);
		void putStringToFile(std::string requestBody);
		int ft_stoi(std::string str, int base);
		std::string postedFileNameRandomiser();
		void matchLocation(Server &server);
		bool checkAllowMethods();
};
#endif

