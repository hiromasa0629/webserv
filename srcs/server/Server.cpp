/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 00:27:33 by hyap              #+#    #+#             */
/*   Updated: 2023/02/13 14:54:20 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

const char* Server::_example_res = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 44\n\n<link rel=\"icon\" href=\"data:,\">\nHello world!";

/***********************************
 * Constructors
***********************************/

Server::Server(void)
{

}

Server::Server(int ai_flags, int ai_family, int ai_socktype, int ai_protocol, const char* hostname, const char* port, const char* config_file) : Socket(ai_flags, ai_family, ai_socktype, ai_protocol, hostname, port)//, _config(config_file)
{
	(void)config_file;
	try
	{
		this->init_bind();
		this->init_listen();
	}
	catch (const std::exception& e)
	{
		std::cout << errno << std::endl;
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

void	Server::run(void)
{
	try
	{
		this->main_loop();
	}
	catch (const std::exception& e)
	{
		std::cerr << RED << e.what() << RESET << std::endl;
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
	// close(3);
	if (bind(this->_socketfd, this->_addrinfo->ai_addr, this->_addrinfo->ai_addrlen) < 0)
		throw std::runtime_error("[Error] Server::init_bind()");
	std::cout << INFO("Server::init_bind") << std::endl;
}

void	Server::init_listen(void)
{
	if (listen(this->_socketfd, SOMAXCONN))
		throw std::runtime_error("[Error] Server::init_listen()");
	std::cout << INFO("Server::init_listen") << "Listening..." << std::endl;
}

void	Server::insert_fd_to_fds(int fd, short event)
{
	pollfd_t	tmp;
	
	std::memset(&tmp, 0, sizeof(tmp));
	tmp.fd = fd;
	tmp.events = event;
	this->_fds.push_back(tmp);
	
}

void	Server::print_request_header(int fd)
{
	char buf[3000] = {0};
	if (recv(fd, buf, 3000, 0) < 0)
		throw std::runtime_error("Recv");
	std::cout << buf << std::endl;
}

void	Server::accept_connection(void)
{
	int	newfd;
	
	if ((newfd = accept(this->_socketfd, this->_addrinfo->ai_addr, &this->_addrinfo->ai_addrlen)) < 0)
		throw std::runtime_error("[Error] Server::accept_connection() accept");
	// fcntl(newfd, F_SETFL, O_NONBLOCK);
	this->insert_fd_to_fds(newfd, POLLIN);
}

void	Server::handle_pollin(const pollfd_t& pfd)
{
	std::cout << "POLLIN" << std::endl;
	print_request_header(pfd.fd);
	this->insert_fd_to_fds(pfd.fd, POLLOUT);
	this->_fds.erase(std::find(this->_fds.begin(), this->_fds.end(), pfd));
	
}

void	Server::handle_pollout(const pollfd_t& pfd)
{
	std::cout << "POLLOUT" << std::endl;
	send(pfd.fd, _example_res, std::strlen(_example_res), 0);
	close(pfd.fd);
	this->_fds.erase(std::find(this->_fds.begin(), this->_fds.end(), pfd));
}

void	Server::main_loop(void)
{
	int poll_ready;
	
	this->insert_fd_to_fds(this->_socketfd, POLLIN);
	while (1)
	{
		poll_ready = poll(this->_fds.data(), this->_fds.size(), 100);
		if (poll_ready < 0)
			throw std::runtime_error("[Error] Server::accept_connnection() poll");
		if (poll_ready == 0)
			continue ;
		for (size_t i = 0; i < this->_fds.size(); i++)
		{
			if (!(this->_fds[i].revents))
				continue;
			if (this->_fds[i].fd == this->_socketfd && this->_fds[i].revents & POLLIN) // Listening socket
			{
				std::cout << "accepted connection" << std::endl;
				this->accept_connection();
			}
			else if (this->_fds[i].revents & POLLIN)
				this->handle_pollin(this->_fds[i]);
			else if (this->_fds[i].revents & POLLOUT)
				this->handle_pollout(this->_fds[i]);
		}
	}
}

/***********************************
 *  Non member function
***********************************/

bool	operator==(const struct pollfd& lhs, const struct pollfd& rhs)
{
	return (lhs.fd == rhs.fd && lhs.events == rhs.events && lhs.revents == rhs.revents);
}
