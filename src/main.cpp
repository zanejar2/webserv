#include "WebServer.hpp"
#include <stdio.h>

int main(int ac, char** av)
{
	try{
		Config conf(ac, av);

		WebServer webServer(conf);
		signal(SIGPIPE, SIG_IGN);
    	webServer.Build();
    	webServer.Run();
	}
	catch(std::exception &e){
		std::cout << e.what() << std::endl;
	}
    return (0);
}
