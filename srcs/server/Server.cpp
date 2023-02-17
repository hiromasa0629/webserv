/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 00:27:33 by hyap              #+#    #+#             */
/*   Updated: 2023/02/17 17:00:17 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

const char* Server::_example_res = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 44\n\n<link rel=\"icon\" href=\"data:,\">\nHello world!";

/***********************************
 * Constructors
***********************************/

Server::Server(void)
{
	this->_timeval.tv_sec = TIMEOUT_SEC;
	this->_timeval.tv_usec = TIMEOUT_USEC;
}

Server::Server(int ai_flags, int ai_family, int ai_socktype, int ai_protocol, const Config& config) : _logger()
{
	Config::StrToSConfigMap::const_iterator	it;
	Socket									tmp;

	this->_timeval.tv_sec = TIMEOUT_SEC;
	this->_timeval.tv_usec = TIMEOUT_USEC;

	it = config.get_sconfig().begin();
	for (size_t i = 0; it != config.get_sconfig().end(); it++, i++)
	{
		tmp = Socket(ai_flags, ai_family, ai_socktype, ai_protocol, it->second);
		this->_sockets.push_back(tmp);
	}
	try
	{
		this->init_bind();
		this->init_listen();
	}
	catch (const std::exception& e)
	{
		this->_logger.error<std::string>(e.what());
		std::exit(errno);
	}
	
}

Server::Server(const Server &src)
{
	*this = src;
}

Server	&Server::operator=(const Server &rhs)
{
	this->_fds = rhs._fds;
	this->_sockets = rhs._sockets;

	return (*this);
}

void	Server::run(void)
{
	try
	{
		// this->main_loop();
		this->main_loop_select();
	}
	catch (const std::exception& e)
	{
		this->_logger.error<std::string>(e.what());
		std::exit(errno);
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
	std::vector<Socket>::iterator	it;
	
	it = this->_sockets.begin();
	for (; it != this->_sockets.end(); it++)
	{
		if (bind(it->get_socketfd(), it->get_addrinfo()->ai_addr, it->get_addrinfo()->ai_addrlen) < 0)
			throw std::runtime_error("Server::init_bind() " + *it);
		this->_logger.info<std::string>("Server::init_bind() " + *it);
	}
}

void	Server::init_listen(void)
{
	std::vector<Socket>::iterator	it;
	
	it = this->_sockets.begin();
	for (; it != this->_sockets.end(); it++)
	{
		if (listen(it->get_socketfd(),SOMAXCONN) < 0)
			throw std::runtime_error("Server::init_listen() " + *it);
		this->_logger.info<std::string>("Server::init_listen() Listening... " + *it);
	}
}

void	Server::insert_fd_to_fds(int fd, short event)
{
	pollfd_t	tmp;
	
	std::memset(&tmp, 0, sizeof(tmp));
	tmp.fd = fd;
	tmp.events = event;
	this->_fds.push_back(tmp);
	
}

std::vector<char>	Server::read_request_header(int fd)
{
	std::vector<char>	buf(BUFFER_SIZE);
	std::vector<char>	res;
	int					ret;
	
	ret = recv(fd, buf.data(), BUFFER_SIZE, 0);
	while (ret > 0)
	{
        res.insert(res.end(), buf.begin(), buf.begin() + ret);
		buf.clear();
		buf.resize(BUFFER_SIZE);
		if (ret < BUFFER_SIZE)
			break ;
		ret = recv(fd, buf.data(), BUFFER_SIZE, 0);
	}
	if (ret == -1)
		throw std::runtime_error("Recv()");
	return (res);
}

void	Server::accept_connection(int fd)
{
	int								newfd;
	std::vector<Socket>::iterator	it;
	
	if ((it = this->get_socket_from_fd(fd)) == this->_sockets.end())
		throw std::runtime_error("Server::accept_connection() Missing socketfd");
	if ((newfd = accept(it->get_socketfd(), it->get_addrinfo()->ai_addr, &(it->get_addrinfo()->ai_addrlen))) < 0)
		throw std::runtime_error("Server::accept_connection() accept");
	this->_logger.info<std::string>("Server::accept_connection() accepted from " + *it);
	fcntl(newfd, F_SETFL, O_NONBLOCK);
	this->insert_fd_to_fds(newfd, POLLIN);
}

void	Server::handle_pollin(const pollfd_t& pfd)
{
	this->_logger.info<std::string>("POLLIN");
	read_request_header(pfd.fd);
	this->insert_fd_to_fds(pfd.fd, POLLOUT);
	this->_fds.erase(std::find(this->_fds.begin(), this->_fds.end(), pfd));
	
}

void	Server::handle_pollout(const pollfd_t& pfd)
{
	this->_logger.info<std::string>("POLLOUT");
	send(pfd.fd, _example_res, std::strlen(_example_res), 0);
	close(pfd.fd);
	this->_fds.erase(std::find(this->_fds.begin(), this->_fds.end(), pfd));
}

void	Server::main_loop(void)
{
	int poll_ready;
	std::vector<Socket>::iterator	it;
	
	it = this->_sockets.begin();
	for (; it != this->_sockets.end(); it++)
		this->insert_fd_to_fds(it->get_socketfd(), POLLIN);
	while (1)
	{
		poll_ready = poll(this->_fds.data(), this->_fds.size(), 100);
		if (poll_ready < 0)
			throw std::runtime_error("Server::main_loop() poll");
		if (poll_ready == 0)
			continue ;
		for (size_t i = 0; i < this->_fds.size(); i++)
		{
			if (!(this->_fds[i].revents))
				continue;
			if (is_socketfd(this->_fds[i].fd) && this->_fds[i].revents & POLLIN) // is socket fds
				this->accept_connection(this->_fds[i].fd);
			else if (this->_fds[i].revents & POLLIN)
				this->handle_pollin(this->_fds[i]);
			else if (this->_fds[i].revents & POLLOUT)
				this->handle_pollout(this->_fds[i]);
		}
	}
}

void	Server::accept_connection_select(int fd, int* maxfd)
{
	int	newfd;
	std::vector<Socket>::iterator	it;
	
	if ((it = this->get_socket_from_fd(fd)) == this->_sockets.end())
		throw std::runtime_error("Server::accept_connection_select() Missing socketfd");
	if ((newfd = accept(it->get_socketfd(), it->get_addrinfo()->ai_addr, &(it->get_addrinfo()->ai_addrlen))) < 0)
		throw std::runtime_error("Server::accept_connection_select() accept");
	this->_logger.info<std::string>("Server::accept_connection_select() accepted from " + *it);
	fcntl(newfd, F_SETFL, O_NONBLOCK);
	if (*maxfd < newfd)
		*maxfd = newfd;
	FD_SET(newfd, &this->_fd_sets.first);
}

void	Server::handle_pollin_select(int fd)
{
	this->_logger.info<std::string>("POLLIN (select)");
	
	Request								req;
	std::map<int, Request>::iterator	it;
	
	if ((it = this->_fd_requests.find(fd)) != this->_fd_requests.end())
	{
		it->second.append(read_request_header(fd));
		req = it->second;
	}
	else
	{
		req = Request(read_request_header(fd));
		this->_fd_requests.insert(std::make_pair(fd, req));
	}
	if (!req.get_is_complete())
		return ;
	req.print_request_header();
	FD_CLR(fd, &this->_fd_sets.first);
	FD_SET(fd, &this->_fd_sets.second);
}

void	Server::handle_pollout_select(int fd)
{
	this->_logger.info<std::string>("POLLOUT (select)");
	send(fd, _example_res, std::strlen(_example_res), 0);
	if (close(fd) != 0)
		throw std::runtime_error("Pollout select close");
	FD_CLR(fd, &this->_fd_sets.second);
	this->_fd_requests.erase(fd);
}

void	Server::main_loop_select(void)
{
	fd_set							read_fds;
	fd_set							write_fds;
	int								maxfd;
	int								select_ready;
	std::vector<Socket>::iterator	it;

	FD_ZERO(&this->_fd_sets.first);
	FD_ZERO(&this->_fd_sets.second);
	it = this->_sockets.begin();
	maxfd = it->get_socketfd();
	for (; it != this->_sockets.end(); it++)
	{
		FD_SET(it->get_socketfd(), &this->_fd_sets.first);
		if (maxfd < it->get_socketfd())
			maxfd = it->get_socketfd();
	}
	while (1)
	{
		// this->_logger.listening();
		read_fds = this->_fd_sets.first;
		write_fds = this->_fd_sets.second;
		select_ready = select(maxfd + 1, &read_fds, &write_fds, NULL, &this->_timeval);
		if (select_ready < 0)
			throw std::runtime_error("Server::main_loop_selection() select");
		if (select_ready == 0)
			continue ;
		for (int i = 0; i < maxfd + 1; i++)
		{
			if (!FD_ISSET(i, &read_fds))
				continue;
			if (is_socketfd(i))
				this->accept_connection_select(i, &maxfd);
			else
				this->handle_pollin_select(i);
		}
		for (int i = 0; i < maxfd + 1; i++)
		{
			if (!FD_ISSET(i, &write_fds))
				continue;
			this->handle_pollout_select(i);
		}
	}
	
}

bool	Server::is_socketfd(int fd) const
{
	std::vector<Socket>::const_iterator	it;
	
	it = this->_sockets.begin();
	for (; it != this->_sockets.end(); it++)
		if (it->get_socketfd() == fd)
			return (true);
	return (false);
}

std::vector<Socket>::iterator	Server::get_socket_from_fd(int fd)
{
	std::vector<Socket>::iterator	it;
	
	it = this->_sockets.begin();
	for (; it != this->_sockets.end(); it++)
		if (it->get_socketfd() == fd)
			return (it);
	return (this->_sockets.end());
}

/***********************************
 *  Non member function
***********************************/

bool	operator==(const struct pollfd& lhs, const struct pollfd& rhs)
{
	return (lhs.fd == rhs.fd && lhs.events == rhs.events && lhs.revents == rhs.revents);
}
