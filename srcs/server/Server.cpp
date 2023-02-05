/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 00:27:33 by hyap              #+#    #+#             */
/*   Updated: 2023/02/06 00:44:54 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/***********************************
 * Constructors
***********************************/

Server::Server(void)
{

}

Server::Server(int ai_flags, int ai_family, int ai_socktype, int ai_protocol) : Socket(ai_flags, ai_family, ai_socktype, ai_protocol)
{
	try
	{
		this->init_bind();
		this->init_listen();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
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
	int	new_sockfd;

	while (1)
	{
		if ((new_sockfd = accept(this->_socketfd, this->_addrinfo->ai_addr, &this->_addrinfo->ai_addrlen)) < 0)
			return ;
		std::cout << new_sockfd << std::endl;
		fcntl(new_sockfd, F_SETFL, O_NONBLOCK);
		const char* response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/plain\r\n\r\n"
			"Content-Length: 13\r\n"
			"Hello world!";
		write(new_sockfd, response, std::strlen(response));
		close(new_sockfd);
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
}

void	Server::init_listen(void)
{
	if (listen(this->_socketfd, 10))
		throw std::runtime_error("[Error] Server::init_listen()");
}
