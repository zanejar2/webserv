#ifndef _RESPONSE_
# define _RESPONSE_

#include "Request.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/time.h>

class Server;
class Location;

class Response
{
    private:
        
        std::map<int, std::string>          errorResponse;
        Location                            *location;
        Request                             *request;
        std::string						 	locationName;
		std::ifstream                       error_file;
        std::ifstream                       file;
        std::string                         path;
        struct stat                         st;


        std::string                         response;
        const char                          *sendBytes;
        size_t                              bytesSize;
        char                                buffer[1024];
        bool                                drop;


        char**                              av;
        char**                              env;
        int                                 inputfile;
        int                                 outputfile;
        int                                 cgi_pid;
        std::string                         php_path, py_path;
        std::string                         CgiPath;
        bool                                CgiAccess;
        bool                                Cgiread;


    public:

        Response();
        Response(Request &request);
        ~Response();

        std::string     getContentType(const char*);
        void            getBody();

        void            GetPath(Server &server);

        bool            Serving(int, Server&);

        void            generateHtmlListing(Server &server);
        void            GetRespond(Server &);
        void            PostRespond(Server &);
        void            DeleteRespond(Server &);

        void            serveTheFile(Server &server);
        bool            checkLocation(Server &);

		void            RemoveDirectory(std::string &path);
		void			BuildErrorPage(std::string &path, int statu);
		bool 			CheckStatusCode(int status);
		void 			Headers(int statuscode);

		void			geterrorbody();
		void			SendResponse(int statcode, Server &server);
		std::string 	errorBody(int statuscode);
        
        void            HandleCgi(Server &server);
        void            BuildEnv(Server &server, char**& env);
        void            ResponseCgi(Server &server);
        std::string     ft_countsize(std::string str);
        bool            checkAllowMethods(int method);



};

#endif