#include "Config.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>

void printServers(std::vector<Server> &server, int j)
{
	std::cerr << "\n\n--------------------------- print config file ----------------------------------\n\n";
	
	for(int i = 0; i < j; i++)
	{
		std::cerr << "lesten " << server[i].getListen() << std::endl;
		std::cerr << "host " << server[i].getHost() << std::endl;
		std::cerr << "ServerName " << server[i].getServerName() << std::endl;
		std::cerr << "body size " << server[i].getBodySize() << std::endl;


		std::cerr << "\n\n==========================\n\n";
		std::map<int, std::string> erre ; 
		erre = server[i].getErrorPage();
		std::map<int, std::string>::iterator it = erre.begin(); 
		std::map<int, std::string>::iterator itt = erre.end(); 

		std::cerr << "Error Pages\n";
		while (it != itt)
		{ 
			std::cerr << it->first << " :: " << it->second << std::endl; 
			it++; 
		}

		std::map<std::string, Location>	 lo ;
		lo = server[i].getAllLocations();
		std::map<std::string, Location>::iterator it2 = lo.begin();
		std::map<std::string, Location>::iterator itt2 = lo.end();

		while (it2 != itt2)
		{
			std::vector<std::pair <std::string, std::string> > cgi = it2->second.cgi_pass;
			std::vector<std::pair <std::string, std::string> >::iterator cgi_it = cgi.begin();
			std::vector<std::pair <std::string, std::string> >::iterator cgi_itt = cgi.end();

			std::cerr << "\n\n*****************************\n\n";
			std::cerr << "location path : " << it2->first << " \n" << "auto_index : " << it2->second.auto_index << "\n" \
				 << "methods : " << it2->second.methods << "\n"  << "root : " << it2->second.root << "\n" << "index : " << it2->second.index  << "\n"\
				  << "upload_pass : " << it2->second.upload_pass << "\n"\
				  << "redirection : " << it2->second.redirect <<std::endl;
			while (cgi_it != cgi.end())
			{
				std::cerr << "cgi : " << cgi_it->first << " => " << cgi_it->second << "\n";
				cgi_it++;
			}
			it2++;
		}
		std::cerr << "\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n";

	}

	std::cerr << "\n\n-------------------------------------------------------------\n\n";
}

bool Config::isValidHost(std::string host)
{
	struct sockaddr_in sockaddr;
  	return (inet_pton(AF_INET, host.c_str(), &(sockaddr.sin_addr)) ? true : false);
}

bool Config::isValidPort(std::string parametr)
{
	unsigned int port;
	
	port = 0;
	for (size_t i = 0; i < parametr.length(); i++)
	{
		if (!std::isdigit(parametr[i]))
			return false;
	}
	port = ft_stoi((parametr));
	if (port < 1 || port > 65636)
		return false;
	return true;
}


void Config::globalCheck(std::vector<Server> &server, int j)
{
	
	for(int i = 0; i < j; i++)
	{

		if (server[i].getListen().empty())
			throw exception_error ("Invalid Port");
		if (!isValidPort(server[i].getListen()))
			throw exception_error ("Invalid Port");

		if (server[i].getHost().empty())
			throw exception_error ("Invalid Host");
		if (!isValidHost(server[i].getHost()))
			throw exception_error ("Invalid Host");
		
		if (!server[i].getBodySize())
			throw exception_error ("Invalid Body Size");
		
		if (server[i].getServerName().empty())
			throw exception_error ("Invalid Server Name");
	
		std::map<std::string, Location>	 lo ;
		lo = server[i].getAllLocations();
		std::map<std::string, Location>::iterator it1 = lo.begin();
		std::map<std::string, Location>::iterator it2 = lo.end();

		while (it1 != it2)
		{
			std::vector<std::pair <std::string, std::string> > cgi = it1->second.cgi_pass;
			std::vector<std::pair <std::string, std::string> >::iterator cgi_it = cgi.begin();

			if (it1->second.root.empty())
    		    throw exception_error("Invalid Root");

			if (access(it1->second.root.c_str(), F_OK) == -1)
    		    throw exception_error("Invalid Root");
			while (cgi_it != cgi.end())
			{
				if (cgi_it->first.compare("php") && cgi_it->first.compare("py"))
					throw exception_error("Invalid CGI Pass");
				if (access(cgi_it->second.c_str(), F_OK) == -1)
					throw exception_error("Invalid CGI Execution Path");
				cgi_it++;
			}
			it1++;
		}
	}
}

void	Config::ft_extention(int ac, char** av)
{
	if (ac > 2)
		throw exception_error("Wrong number of argument");

	char str[6] = "fnoc.";
	int i = 0;
	int j = 0;
	if (av[1])
	{
		while (av[1][i])
			i++;
		i--;
		while (str[j])
		{
			if (str[j++] != av[1][i--])
				throw exception_error("Extantion error");
		}
	}	
}

std::string	Config::collect_content(std::string file)
{
	std::fstream conf(file);
	
	if (!conf.is_open())
		throw exception_error("can not open the file");
	std::string line;
	std::string tmp;
	while (std::getline(conf, tmp))
		line = line + tmp + '\n';
	return(line);
}

void	Config::remove_comment(std::string &file)
{
	size_t	i;

	i = file.find("#");
	while(i != std::string::npos)
	{
		size_t	j = file.find("\n", i);
		file.erase(i, j - i);
		i = file.find("#");
	}
}

void	Config::remove_space(std::string &file)
{
	size_t	i = 0;

	while (file[i] && isspace(file[i]))
		i++;
	file = file.substr(i);
	i = file.length() - 1;
	while (i > 0 && isspace(file[i]))
		i--;
	file = file.substr(0, i + 1);
}

int Config::ft_strncomp(const std::string& str1, const std::string& str2, size_t count) 
{
    std::string substr1 = str1.substr(0, count);
    std::string substr2 = str2.substr(0, count);

    return substr1.compare(substr2);
}


void Config::split_servers(std::string &file)
{
	size_t start = 0;
	size_t end = 1;

	while (start != end && start < file.length())
	{
		start = findStartServer(start, file);
		end = findEndServer(start, file);
		if (start == end)
			throw exception_error("problem with scope");
		this->_str.push_back(file.substr(start, end - start + 1));
		this->num_of_server++;
		start = end + 1;
	}
}

size_t Config::findStartServer (size_t start, std::string &file)
{
	size_t i;

	for (i = start; file[i]; i++)
	{
		if (file[i] == 's')
			break ;
		if (!isspace(file[i]))
			throw  exception_error("Server did not find");
	}
	if (!file[i])
		return (start);
	if (file.compare(i, 6, "server") != 0)
		throw exception_error("Server did not find");
	i += 6;
	while (file[i] && isspace(file[i]))
		i++;
	if (file[i] == '{')
		return (i);
	else 
		throw  exception_error("problem with scope");

}

size_t Config::findEndServer (size_t start, std::string &file)
{
	size_t	i;
	size_t	scope;
	
	scope = 0;
	for (i = start + 1; file[i]; i++)
	{
		if (file[i] == '{')
			scope++;
		if (file[i] == '}')
		{
			if (!scope)
				return (i);
			scope--;
		}
	}
	return (start);
}

bool Config::isAllDigits(const std::string& str)
{
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
        if (!isdigit(*it))
            return false;
    return true;
}

bool Config::isPathValid(const std::string& path) 
{
    if (access(path.c_str(), F_OK) != -1)
        return true;
    return false;
}

std::vector<std::string> Config::SplitFile(std::string& input)
{
    std::vector<std::string> lines;
    std::istringstream iss(input);
    std::string line;

    while (std::getline(iss, line))
    {
        std::string delimiter = " \t\n";
        size_t start = line.find_first_not_of(delimiter);
        size_t end = line.find_last_not_of(delimiter);

        if (start != std::string::npos && end != std::string::npos)
        {
            std::string trimmedLine = line.substr(start, end - start + 1);
            std::istringstream lineStream(trimmedLine);
            std::string word;
            while (lineStream >> word)
                lines.push_back(word);
        }
    }
    return lines;
}

std::vector<std::pair<std::string, std::string> > Split_File(std::string& input) 
{

    std::vector<std::pair<std::string, std::string> > pairs;
    std::istringstream iss(input);
    std::string line;


    while (std::getline(iss, line)) {
        std::string delimiter = " \t\n";
        size_t start = line.find_first_not_of(delimiter);
        size_t end = line.find_last_not_of(delimiter);

        if (start != std::string::npos && end != std::string::npos) {
            std::string trimmedLine = line.substr(start, end - start + 1);
            std::istringstream lineStream(trimmedLine);
            std::string firstWord;
            lineStream >> firstWord;

            std::string restOfLine;
            std::getline(lineStream, restOfLine);

            pairs.push_back(std::make_pair(firstWord, restOfLine));
        }
    }
    return pairs;
}

int Config::ft_stoi(std::string str)
{
    std::stringstream ss(str);
    if (str.length() > 10)
        throw exception_error("MAX Body Size");
    for (size_t i = 0; i < str.length(); ++i)
        if(!isdigit(str[i]))
            throw exception_error("Body Size");
    int res;
    ss >> res;
    return (res);
}

std::string RemoveSpaces(std::string& input) {
    std::string result;

    for (size_t i = 0; i < input.length(); ++i) {
        if (input[i] != ' ' && input[i] != '\t') {
            result += input[i];
        }
    }

    return result;
}

int Config::setMethods(std::vector<std::string> method) 
{
	int	methods = 0;
	try{
		std::set<std::string> validMethod;
		validMethod.insert("GET");
		validMethod.insert("POST");
		validMethod.insert("DELETE");
		for (int i = 0; i < method.size(); i++)
		{
			if (!validMethod.count(method[i]))
				throw exception_error("'" + method[i] +"` Invalid method");
			methods += method[i].size();
		}
	}
	catch(const std::exception& e) {std::cerr << e.what() << std::endl;}
	return methods;
}

std::vector<std::pair <std::string, std::string> > Config::CgiPath(std::string& inputString)
{
	std::string newStr = inputString.substr(1);
    std::istringstream iss(newStr);
    std::string token;

    if (std::getline(iss, token, ' ')) 
	{
        std::string firstToken = token;
        if (std::getline(iss, token)) 
		{
            std::string secondToken = token;
            tokens.push_back(std::make_pair(firstToken, secondToken));
        }
    }
    return tokens;
}

std::string	Config::checkRoot(std::string &root)
{
	if (root[root.size() - 1] != '/')
		root += '/';
	return root;
}

Location Config::setpartlocation(std::vector<std::pair<std::string, std::string> > &content)
{
	Location location;
	for (size_t i = 0; i < content.size(); i++) 
	{
		if (content[i].first == "allow_methods" && i < content.size())
		{
			std::vector<std::string> _methods;
			std::istringstream iss(content[i].second);
   			std::string word;
   			while (iss >> word) {
        		_methods.push_back(word);
    		}
			location.methods = setMethods(_methods);
		}
		else if (content[i].first == "index" && i < content.size())
		{
			location.index = RemoveSpaces(content[i].second);
		}
		else if (content[i].first == "root" && i < content.size())
		{
			std::string root = checkRoot(content[i].second);
			location.root = RemoveSpaces(root);
		}
		else if (content[i].first == "autoindex" && i < content.size())
		{
			content[i].second = RemoveSpaces(content[i].second);
			if (content[i].second == "1")
				location.auto_index = true;
			else if (content[i].second == "0")
				location.auto_index = false;
			else
				location.auto_index = false;
		}
		else if (content[i].first == "cgi_pass" && i < content.size())
		{

 			location.cgi_pass = CgiPath(content[i].second);
			location.cgi = true;
		}
		else if (content[i].first == "upload_pass" && i < content.size())
		{
			location.upload_pass = RemoveSpaces(content[i].second);
		}
		else if (content[i].first == "redirection" && i < content.size())
		{
			location.redirect = RemoveSpaces(content[i].second);
		}
    }
	return(location);
}

Server	Config::ConfigurationServer(std::string& conf)
{
	Server server;
	std::istringstream iss(conf);
	std::string line;
	std::vector<std::pair<std::string, std::string> > tmp;

	tmp = Split_File(conf);
	for(int j = 0; j < tmp.size(); j++)
	{
		if (tmp[j].first == "host" && (j + 1) < tmp.size())
		{
			server.setHost(RemoveSpaces(tmp[j].second));
		}
		else if (tmp[j].first == "listen" && (j + 1) < tmp.size())
		{
			server.setListen (RemoveSpaces(tmp[j].second));
		}
		else if (tmp[j].first == "body_size" && (j + 1) < tmp.size())
		{
			server.setBodySize(ft_stoi(RemoveSpaces(tmp[j].second)));
		}
		else if (tmp[j].first == "server_name" && (j + 1) < tmp.size())
		{
			server.setServerName(RemoveSpaces(tmp[j].second));
		}
		else if (tmp[j].first == "error_page" && (j + 1) < tmp.size())
		{
			j++;
			while (tmp[++j].first != "}")
			{
				if (!isAllDigits(tmp[j].first))
					throw exception_error(tmp[j].first + " is not valide");
				int errorCode;
        		errorCode = ft_stoi(tmp[j].first);
				server.setErrorPage(errorCode, RemoveSpaces(tmp[j].second));
			}
		}
		else if (tmp[j].first == "location" && (j + 1) < tmp.size())
		{
			std::string path;
			Location location;
			if (tmp[j].first == "{" || tmp[j].first == "}")
				throw exception_error("Location Scope error");
			path = RemoveSpaces(tmp[j].second);
			if (tmp[++j].first != "{")
				throw exception_error("Scope error");
			j++;
			std::vector<std::pair<std::string, std::string> > LocationCnontent;
			while(j < tmp.size() && tmp[j].first != "}")
			{
				LocationCnontent.push_back(tmp[j]);
				j++;
			}
			location = setpartlocation(LocationCnontent);
			server.addLocation(path, location);
		}
	}
	return server;
}

Config::Config(int ac, char **av)
{
	
	std::string file;
	std::string content;

	server.reserve(3);
	
	ft_extention(ac, av);
	if (ac == 2)
		file = av[1];
	else
		file = "conf/Default.conf";
	content = collect_content(file);
	remove_space(content);
	if (content.empty())
		throw exception_error("Empty file");
	remove_comment(content);
	split_servers(content);
	size_t j = this->num_of_server;
	Server serv;
	for(size_t i = 0; i < j; i++)
	{
		serv = ConfigurationServer(this->_str[i]);
		this->server.push_back(serv);
	}
	globalCheck(server, j);
	for (int i = 0; i < j; i++)
	{
		for(int k = 0; k < j ; k++)
		{
			if (i != k && this->server[i].getListen() == this->server[k].getListen() \
			&& this->server[i].getHost() == this->server[k].getHost())
			{
				throw exception_error("Duplicate server");
				break;
			}
		}
	}
	// printServers(server, j);
}