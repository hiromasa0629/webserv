/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 00:27:33 by hyap              #+#    #+#             */
/*   Updated: 2023/02/08 21:33:33 by hyap             ###   ########.fr       */
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

Server::Server(int ai_flags, int ai_family, int ai_socktype, int ai_protocol, const char* hostname, const char* port) : Socket(ai_flags, ai_family, ai_socktype, ai_protocol, hostname, port)
{
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

void	Server::accept_connection(void)
{
	int					newfd;
	int					val_read;
	pollfd_t			rfds[1];
	pollfd_t			wfds[1];
	int					poll_rready;
	int					poll_wready;
	std::stringstream	content_ss;
	int					content_length;
	std::stringstream	http_header_ss;
	
	content_ss << "<link rel=\"icon\" href=\"data:,\">\nHello world!";
	content_length = std::strlen(content_ss.str().c_str());
	http_header_ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " << content_length << "\n\n" << content_ss.str();
	rfds[0].fd = this->_socketfd;
	rfds[0].events = POLLIN;
	while (1)
	{
		poll_rready = poll(rfds, 1, 100);
		if (poll_rready == -1)
			throw std::runtime_error("[Error] Server::accept_connnection() poll in");
		if (rfds[0].revents & POLLIN)
		{
			if ((newfd = accept(this->_socketfd, this->_addrinfo->ai_addr, &this->_addrinfo->ai_addrlen)) < 0)
				throw std::runtime_error("[Error] Server::accept_connection() accept");
			char buf[3000] = {0};
			val_read = recv(newfd, buf, 3000, 0);
			std::cout << buf << std::endl; // print request header
			wfds[0].fd = newfd;
			wfds[0].events = POLLOUT;
			fcntl(newfd, F_SETFL, O_NONBLOCK);
			poll_wready = poll(wfds, 1, -1);
			if (poll_wready == -1)
				throw std::runtime_error("[Error] Server:accept_connection() poll out");
			if (wfds[0].revents & POLLOUT)
			{
				std::cout << "wfds pollout" << std::endl;
				send(newfd, http_header_ss.str().c_str(), std::strlen(http_header_ss.str().c_str()), 0);
			}
			close(newfd);
		}
	}
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
	if (event == POLLIN)
	{
		this->_rfds.push_back(tmp);
		return ;
	}
	else if (event == POLLOUT)
	{
		this->_wfds.push_back(tmp);
		return ;
	}
}

void	Server::print_request_header(int fd)
{
	char buf[3000] = {0};
	recv(fd, buf, 3000, 0);
	std::cout << buf << std::endl;
}


void	Server::poll_read(void)
{
	int poll_ready;
	int	newfd;

	poll_ready = poll(this->_rfds.data(), this->_rfds.size(), 100);
	if (poll_ready < 0)
		throw std::runtime_error("[Error] Server::accept_connnection() poll 1");
	for (size_t i = 0; i < this->_rfds.size(); i++)
	{
		if (!(this->_rfds[i].revents))
			continue;
		if (this->_rfds[i].fd == this->_socketfd) // Listening socket
		{
			if (this->_rfds[i].revents & POLLIN) // New connection
			{
				if ((newfd = accept(this->_socketfd, this->_addrinfo->ai_addr, &this->_addrinfo->ai_addrlen)) < 0)
					throw std::runtime_error("[Error] Server::accept_connection() accept");
				fcntl(newfd, F_SETFL, O_NONBLOCK);
				this->insert_fd_to_fds(newfd, POLLIN);
			}
			continue ;
		}
		if (this->_rfds[i].revents & POLLIN)
		{
			std::cout << "POLLIN" << std::endl;
			print_request_header(this->_rfds[i].fd);
			this->insert_fd_to_fds(this->_rfds[i].fd, POLLOUT);
			this->_rfds.erase(this->_rfds.begin() + i);
		}
	}
}

void	Server::poll_write(void)
{
	int	poll_ready;
	
	this->poll_read();
	poll_ready = poll(this->_wfds.data(), this->_wfds.size(), 100);
	if (poll_ready < 0)
		throw std::runtime_error("[Error] Server::accept_connection() poll 2");
	for (size_t i = 0; i < this->_wfds.size(); i++)
	{
		if (!(this->_wfds[i].revents))
			continue;
		if (this->_wfds[i].revents & POLLOUT)
		{
			std::cout << "POLLOUT" << std::endl;
			send(this->_wfds[i].fd, _example_res, std::strlen(_example_res), 0);
			close(this->_wfds[i].fd);
			this->_wfds.erase(this->_wfds.begin() + i);
		}
	}
}

void	Server::main_loop(void)
{
	this->insert_fd_to_fds(this->_socketfd, POLLIN);
	while (1)
	{
		this->poll_read();
		this->poll_write();
	}
}
