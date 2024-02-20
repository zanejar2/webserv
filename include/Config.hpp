#ifndef _CONFIG_
#define _CONFIG_

#include "Server.hpp"
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <exception>
#include <algorithm>
#include <utility> 
#include <set>


class Config
{
    public:

		int													num_of_server;
        std::vector<Server> 								server;
		std::vector<std::string> 							_str;
	    std::vector<std::pair<std::string, std::string> > 	tokens;


		
		Config(){};
		Config(int ac, char **file);

		int 												ft_strncomp(const std::string& str1, const std::string& str2, size_t count);
		int				 									ft_stoi(std::string str);
		int					 								setMethods(std::vector<std::string> meth);
		void												ft_extention(int ac, char** av);
		void 												split_servers(std::string &content);
		void												remove_comment(std::string &file);
		void												remove_space(std::string &file);
		void												split_server(std::string &file);
		bool												isAllDigits(const std::string &str);
		bool			 									isPathValid(const std::string& path);
		bool 												isValidHost(std::string host);
		bool 												isValidPort(std::string parametr);
		void 												globalCheck(std::vector<Server> &server, int j);
		size_t 												findStartServer (size_t start, std::string &file);
		size_t												findEndServer (size_t start, std::string &file);
		Location											setpartlocation(std::vector<std::pair<std::string, std::string> > &content);
		Server 												ConfigurationServer(std::string& conf);
		std::string											collect_content(std::string file);
		std::string											checkRoot(std::string &root);
		std::vector<std::string>							SplitFile(std::string& conf);
		std::vector<std::pair <std::string, std::string> > 	CgiPath(std::string& inputString);




		Config(const Config &other) : server(other.server), _str(other._str), num_of_server(other.num_of_server){};

		Config operator=(const Config& other){
			if (this == &other)
				return *this;
			this->server = other.server;
			this->_str = other._str;
			this->num_of_server = other.num_of_server;

			return *this;
		}

		class exception_error : public std::exception {
			private :
				std::string Msg;
				
			public :

				exception_error(std::string message) throw(){
					Msg = "Error: " + message;
				}

				virtual const char* what() const throw(){
					return (Msg.c_str());
				};
				virtual ~exception_error() throw() {}
		};

    private:

};

#endif