#include "uweb.h"

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

static const int CONNECTIONS_MAX = 0xff;
static const int PORT = 2342;

uWeb::uWeb()
{

}

bool uWeb::run(const std::string &interface, const std::string &data)
{
    int sockfd = socket(AF_INET6, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
        std::cerr << "ERROR: Can't open socket (" << strerror(errno) << ")" << std::endl;
        return false;
	}

    if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, interface.c_str(), interface.size()) < 0)
    {
        std::cerr << "ERROR: Can't bind to device (" << strerror(errno) << ")" << std::endl;
        return false;
    }

    sockaddr_in6 serv_addr{0};
    serv_addr.sin6_family = AF_INET6;
    serv_addr.sin6_port = htons(PORT);

    if (bind(sockfd, (sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
        std::cerr << "ERROR: Can't bind socket (" << strerror(errno) << ")" << std::endl;
        return false;
	}

    listen(sockfd, CONNECTIONS_MAX);
    sockaddr_in6 cli_addr{0};
    socklen_t cli_len = sizeof(cli_addr);

    while (true)
    {
        int newsockfd = accept(sockfd, (sockaddr *) &cli_addr, &cli_len);
        std::string ip(128, '\0');
        if (inet_ntop(AF_INET6, &cli_addr.sin6_addr, &ip.at(0), ip.length()) == 0)
        {
            ip = "unknown";
        }
        std::cout << "New connection from [" << ip << "]:" << cli_addr.sin6_port << std::endl;

		if (newsockfd < 0)
		{
            std::cerr << "ERROR: Can't accept (" << strerror(errno) << ")" << std::endl;
            continue;
        }

        int pid = fork();
		if (pid < 0)
		{
            std::cerr << "ERROR: Can't fork (" << strerror(errno) << ")" << std::endl;
            close(newsockfd);
		}
        else if (pid == 0)
        {
            // in the child
			close(sockfd);
            sendData(newsockfd, data);
            close(newsockfd);
			exit(0);
		}
		else
        {
            // in the parrent
			close(newsockfd);
		}
	}

	return 0;
}

void uWeb::sendData(const int socket, const std::string &data) const
{
    static const size_t CHUNK = 128U;
    char tmp[CHUNK];

#ifdef DEBUG
    std::cout << "Read: ";
#endif
    ssize_t r = 1;
    while (r > 0)
    {
        r = recv(socket, tmp, CHUNK, MSG_DONTWAIT);
        if ((r < 0) && (errno != EAGAIN))
        {
            std::cerr << "ERROR " << r << " reading to socket (" << strerror(errno) << ")" << std::endl;
            return;
        }
#ifdef DEBUG
        else if (r > 0)
        {
            std::cout << std::string(tmp, tmp+r);
        }
#endif
    }
#ifdef DEBUG
    std::cout << std::endl;
#endif
    const std::string header{"HTTP/1.0 200 OK\r\n"\
                            "Server: uWeb\r\n"\
                            "Connection: close\r\n"\
                            "Content-Type: application/json; charset=UTF-8\r\n"\
                            "Content-Length: "+std::to_string(data.size())+"\r\n"\
                            "\r\n"};
    if (write(socket, header.c_str(), header.size()) < 0)
    {
        std::cerr << "ERROR writing to socket (" << strerror(errno) << ")" << std::endl;
    }

    if (write(socket, data.c_str(), data.size()) < 0)
	{
        std::cerr << "ERROR writing to socket (" << strerror(errno) << ")" << std::endl;
	}
}
