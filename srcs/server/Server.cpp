/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 00:27:33 by hyap              #+#    #+#             */
/*   Updated: 2023/02/07 20:58:24 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/***********************************
 * Constructors
***********************************/

Server::Server(void)
{

}

Server::Server(int ai_flags, int ai_family, int ai_socktype, int ai_protocol, const char* hostname, const char* port) : Socket(ai_flags, ai_family, ai_socktype, ai_protocol, hostname, port)
{
	try
	{
		this->init_bind();
		this->init_listen();
	}
	catch (const std::exception& e)
	{
		std::cerr << RED << e.what() << RESET << std::endl;
	}

}

Server::Server(const Server &src)
{
	*this = src;
}

Server	&Server::operator=(const Server &rhs)
{
	Socket::operator=(rhs);

	return (*this);
}

void	Server::accept_connection(void)
{
	int				newfd;
	int				maxfd;
	int				val_read;
	

	while (1)
	{
		if ((newfd = accept(this->_socketfd, this->_addrinfo->ai_addr, &this->_addrinfo->ai_addrlen)) < 0)
			return ;
		std::cout << newfd << std::endl; 
		char buf[3000] = {0};
		val_read = recv(newfd, buf, 3000, 0);
		std::cout << buf << std::endl; // print request header
		const char* response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
		send(newfd, response, std::strlen(response), 0);
		close(newfd);
	}
}

/***********************************
 * Destructors
***********************************/

Server::~Server(void)
{

}

/***********************************
 * Private Functions
***********************************/

void	Server::init_bind(void)
{
	if (bind(this->_socketfd, this->_addrinfo->ai_addr, this->_addrinfo->ai_addrlen) < 0)
		throw std::runtime_error("[Error] Server::init_bind()");
	std::cout << INFO("Server::init_bind") << std::endl;
}

void	Server::init_listen(void)
{
	if (listen(this->_socketfd, 10))
		throw std::runtime_error("[Error] Server::init_listen()");
	std::cout << INFO("Server::init_listen") << "Listening..." << std::endl;
}
