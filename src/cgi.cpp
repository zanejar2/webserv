#include "Response.hpp"
#include "Server.hpp"


std::string generateNumString() 
{
	int length = 10;
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const int charsetSize = sizeof(charset) - 1;
    std::string result;
    
    std::srand(time(NULL));
    
    for (int i = 0; i < length; ++i) {
        result += charset[std::rand() % charsetSize];
    }
	std::string path = "/Users/zanejar/Desktop/webserv/cgiU/" + result;
	return (path);
}

std::string ft_coun(std::string str)
{
    FILE* file;
    file = fopen(str.c_str(), "rb");
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    std::stringstream ptr;
    ptr << size;
    std::string tmp;
    ptr >> tmp;
    fclose(file);
    return tmp;
}

void	Response::HandleCgi(Server &server)
{
	int status = 0;
	std::string pathcgi;
	CgiPath = generateNumString();
	outputfile = open(CgiPath.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0777);
	if (request->method == "POST")
	{
		pathcgi = request->uploadPass + "/" + request->postedFileName;
		inputfile = open(pathcgi.c_str() , O_RDONLY, 0777);
		if (inputfile < 0)
		{
			SendResponse(500, server);
			return ;
		}
	}
	this->cgi_pid = fork();
	if (this->cgi_pid == 0)
	{

		std::vector <std::pair<std::string, std::string> > cgi_loc;
		cgi_loc = location->cgi_pass;
		std::vector<std::pair <std::string, std::string> >::iterator cgi_it = cgi_loc.begin();
		std::vector<std::pair <std::string, std::string> >::iterator cgi_itt = cgi_loc.end();

		while (cgi_it != cgi_itt)
		{
			if (cgi_it->first == "php")
				php_path = cgi_it->second;
			if (cgi_it->first == "py")
				py_path = cgi_it->second;
			cgi_it++;
		}

		this->av = (char **)malloc(sizeof(char *) * 3);
		if(path.find(".php") != std::string::npos)
			av[0] = strdup(php_path.c_str());
		if (path.find(".py") != std::string::npos)
			av[0] = strdup(py_path.c_str());
		av[1] = strdup(path.c_str());
		av[2] = NULL;
		if (request->method == "POST")
			this->env = (char **)malloc(sizeof(char *) * 13);
		else 
			this->env = (char **)malloc(sizeof(char *) * 11);


		std::string tmp;
		tmp = "REQUEST_METHOD=" + request->method;
		env[0] = new char[tmp.size() + 1];
		env[0] = strcpy(env[0], tmp.c_str());
		tmp = "SERVER_PORT=" + server.getListen();
		env[1] = new char[tmp.size() + 1];
		env[1] = strcpy(env[1], tmp.c_str());
		if(path.find(".php") != std::string::npos)
			tmp = "SCRIPT_NAME=" + php_path;
		if (path.find(".py") != std::string::npos)
			tmp = "SCRIPT_NAME=" + py_path;
		env[2] = new char[tmp.size() + 1];
		env[2] = strcpy(env[2], tmp.c_str());
		tmp = "SCRIPT_FILENAME=" + path;
		env[3] = new char[tmp.size() + 1];
		env[3] = strcpy(env[3], tmp.c_str());
		tmp = "PATH_INFO=" + path;
		env[4] = new char[tmp.size() + 1];
		env[4] = strcpy(env[4], tmp.c_str());
		tmp = "QUERY_STRING=" + request->queryString;
		env[5] = new char[tmp.size() + 1];
		env[5] = strcpy(env[5], tmp.c_str());
		tmp = "HTTP_COOKIE=" + request->headers["Cookie"];
		env[6] = new char[tmp.size() + 1];
		env[6] = strcpy(env[6], tmp.c_str());
		tmp = "REDIRECT_STATUS=200";
		env[7] = new char[tmp.size() + 1];
		env[7] = strcpy(env[7], tmp.c_str());
		tmp = "HTTP_USER_AGENT=" + request->headers["User-Agent"];
		env[8] = new char[tmp.size() + 1];
		env[8] = strcpy(env[8], tmp.c_str());
		tmp = "REMOTE_ADDR=127.0.0.1";
		env[9] = new char[tmp.size() + 1];
		env[9] = strcpy(env[9], tmp.c_str());
		tmp = "HTTP_HOST=" + server.getHost();
		env[10] = new char[tmp.size() + 1];
		env[10] = strcpy(env[10], tmp.c_str());
		if (request->method == "POST")
		{
			tmp = "CONTENT_LENGTH=" + ft_countsize(pathcgi);
			env[11] = new char[tmp.size() + 1];
			env[11] = strcpy(env[11], tmp.c_str());
			tmp = "CONTENT_TYPE=" + request->headers["Content-Type"];
			env[12] = new char[tmp.size() + 1];
			env[12] = strcpy(env[12], tmp.c_str());
			env[13] = NULL;
		}
		else 
			env[11] = NULL;
		if (!av[0] || !av[1])
		{
			SendResponse(500, server);
			exit(1);
		}
		if (request->method == "POST")
		{
			dup2(inputfile, 0);
		}
		dup2(outputfile, 1);
		dup2(outputfile, 2);
		execve(av[0], av, env);
		SendResponse(500, server);
		exit(1);
	}
	CgiAccess = false;
	if (request->method == "POST")
		close(inputfile);
	close (outputfile);
}