/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 00:27:33 by hyap              #+#    #+#             */
/*   Updated: 2023/03/06 21:46:06 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <cstring>

const char* Server::_example_res = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 44\r\n\r\n<link rel=\"icon\" href=\"data:,\">\nHello world!";

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
		this->_logger.error(std::string(strerror(errno)) + " " + e.what());
		std::exit(errno);
	}
	
}

Server::Server(const Server &src)
{
	*this = src;
}

// Server	&Server::operator=(const Server &rhs)
// {
// 	this->_fds = rhs._fds;
// 	this->_sockets = rhs._sockets;

// 	return (*this);
// }

void	Server::run(void)
{
	try
	{
		// this->main_loop();
		this->main_loop_select();
	}
	catch (const std::exception& e)
	{
		this->_logger.error(e.what());
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
		this->_logger.info("Server::init_bind() " + *it);
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
		this->_logger.info("Server::init_listen() Listening... " + *it);
	}
}

// void	Server::insert_fd_to_fds(int fd, short event)
// {
// 	pollfd_t	tmp;
	

// 	tmp.fd = fd;
// 	tmp.events = event;
// 	this->_fds.push_back(tmp);
	
// }

std::string			Server::read_request(int fd)
{
	char				buf[BUFFER_SIZE];
	std::string			res;
	ssize_t				ret;
	
	std::memset(buf, 0, BUFFER_SIZE);
	ret = recv(fd, buf, BUFFER_SIZE, 0);
	while (ret > 0)
	{
		for (ssize_t i = 0; i < ret; i++)
			res.push_back(buf[i]);
		std::memset(buf, 0, BUFFER_SIZE);
#if DEBUG
		// std::cout << "ret: " << ret << ", size: " << res.size() << std::endl;
#endif
		if (ret < BUFFER_SIZE)
			break ;
		ret = recv(fd, buf, BUFFER_SIZE, 0);
	}
	if (ret == -1)
		throw std::runtime_error("Recv()");
#if DEBUG
	// for (size_t i = 0; i < res.size(); i++)
	// 	std::cout << res[i];
	// std::cout << "ret = " << ret << "res.size = " << res.size() << std::endl;
	// std::cout << res << std::endl;
	// std::cout << std::endl;
	// std::cout << "last 4: " << (int)res[res.size() - 4] << " " << (int)res[res.size() - 3] << " " << (int)res[res.size() - 2] << " " << (int)res[res.size() - 1] << std::endl;
	// std::cout << "last 1: " << (int)res[res.size()] << std::endl; 
#endif
	return (res);
}

// // poll()
// void	Server::accept_connection(int fd)
// {
// 	int								newfd;
// 	std::vector<Socket>::iterator	it;
	
// 	if ((it = this->get_socket_from_fd(fd)) == this->_sockets.end())
// 		throw std::runtime_error("Server::accept_connection() Missing socketfd");
// 	if ((newfd = accept(it->get_socketfd(), it->get_addrinfo()->ai_addr, &(it->get_addrinfo()->ai_addrlen))) < 0)
// 		throw std::runtime_error("Server::accept_connection() accept");
// 	this->_logger.info<std::string>("Server::accept_connection() accepted from " + *it);
// 	fcntl(newfd, F_SETFL, O_NONBLOCK);
// 	this->insert_fd_to_fds(newfd, POLLIN);
// }

// void	Server::handle_pollin(const pollfd_t& pfd)
// {
// 	this->_logger.info<std::string>("POLLIN");
// 	read_request(pfd.fd);
// 	this->insert_fd_to_fds(pfd.fd, POLLOUT);
// 	this->_fds.erase(std::find(this->_fds.begin(), this->_fds.end(), pfd));
	
// }

// void	Server::handle_pollout(const pollfd_t& pfd)
// {
// 	this->_logger.info<std::string>("POLLOUT");
// 	send(pfd.fd, _example_res, std::strlen(_example_res), 0);
// 	close(pfd.fd);
// 	this->_fds.erase(std::find(this->_fds.begin(), this->_fds.end(), pfd));
// }

// void	Server::main_loop(void)
// {
// 	int poll_ready;
// 	std::vector<Socket>::iterator	it;
	
// 	it = this->_sockets.begin();
// 	for (; it != this->_sockets.end(); it++)
// 		this->insert_fd_to_fds(it->get_socketfd(), POLLIN);
// 	while (1)
// 	{
// 		poll_ready = poll(this->_fds.data(), this->_fds.size(), 1000);
// 		if (poll_ready < 0)
// 			throw std::runtime_error("Server::main_loop() poll");
// 		std::cout << "loop" << std::endl;
// 		if (poll_ready == 0)
// 			continue ;
// 		for (size_t i = 0; i < this->_fds.size(); i++)
// 		{
// 			if (!(this->_fds[i].revents))
// 				continue;
// 			if (is_socketfd(this->_fds[i].fd) && this->_fds[i].revents & POLLIN) // is socket fds
// 			{
// 				std::cout << "this is a socketfd ready for accept" << std::endl;
// 				this->accept_connection(this->_fds[i].fd);
// 			}
// 			else if (this->_fds[i].revents & POLLIN)
// 			{
// 				std::cout << "this is a client fd ready to be read" << std::endl;
// 				this->handle_pollin(this->_fds[i]);
// 			}
// 			else if (this->_fds[i].revents & POLLOUT)
// 			{
// 				std::cout << "this is a client fd ready to be write" << std::endl;
// 				this->handle_pollout(this->_fds[i]);
// 			}
// 		}
// 	}
// }


void	Server::accept_connection_select(int fd, int* maxfd)
{
	int	newfd;
	std::vector<Socket>::iterator	it;
	
	if ((it = this->get_socket_from_fd(fd)) == this->_sockets.end())
		throw std::runtime_error("Server::accept_connection_select() Missing socketfd");
	if ((newfd = accept(it->get_socketfd(), it->get_addrinfo()->ai_addr, &(it->get_addrinfo()->ai_addrlen))) < 0)
		throw std::runtime_error("Server::accept_connection_select() accept");
	this->_logger.info("Server::accept_connection_select() accepted from " + *it);
	fcntl(newfd, F_SETFL, O_NONBLOCK);
	if (*maxfd < newfd)
		*maxfd = newfd;
	FD_SET(newfd, &this->_fd_sets.first);
	it = this->_sockets.begin();
	for (; it != this->_sockets.end(); it++)
		if (it->get_socketfd() == fd)
			this->_fd_sconfig.insert(std::make_pair(newfd, it->get_sconfig()));
}

void	Server::handle_pollin_select(int fd)
{
	this->_logger.info("POLLIN (select) fd: " + std::to_string(fd));
	
	try
	{
		std::string	req;
		
		req = this->read_request(fd);
		if (req.empty())	// empty request or closed by client
			throw ServerErrorException(__LINE__, __FILE__, E0, "Empty request or Closed by client");
		if (this->_fd_requests.count(fd) == 0)
			this->_fd_requests.insert(std::make_pair(fd, TmpRequest()));
		this->_fd_requests[fd].append(req);
		if (!this->_fd_requests[fd].get_is_complete())
		{
#if DEBUG
			std::cout << "NOT COMPLETE" << std::endl;
#endif
			return ;
		}
		this->_fd_requests[fd].print_request_header();
	}
	catch (const ServerErrorException& e)
	{
		this->_logger.warn(e.what());
		if (e.get_status() == E0)	// empty request or closed by client
		{
			FD_CLR(fd, &this->_fd_sets.first);
			this->_fd_requests.erase(fd);
			if (close(fd) != 0)
				throw std::runtime_error(std::to_string(__LINE__) + " close error");
			return ;
		}
		else
			this->_fd_requests[fd] = TmpRequest(e.get_status());
	}
	catch (const std::exception& e)
	{
		this->_logger.error(e.what());
		this->_fd_requests[fd] = TmpRequest(E500);
	}
	FD_CLR(fd, &this->_fd_sets.first);
	FD_SET(fd, &this->_fd_sets.second);
}

void	Server::handle_pollout_select(int fd)
{
	this->_logger.info("POLLOUT (select) fd: " + std::to_string(fd));
	Response		response;
	std::string		response_msg;
	std::string		body;
	std::string		header;
	std::string		res;

	try
	{
		if (this->_fd_response.count(fd) == 0)
		{
			this->_fd_response.insert(std::make_pair(fd, Response(this->_fd_requests[fd], this->_fd_sconfig[fd])));
			// header = this->_fd_response[fd].get_response_header();
			// body = this->_fd_response[fd].get_body();
			res.append(this->_fd_response[fd].get_response_header()).append(this->_fd_response[fd].get_body());
		}
		else
			res.append(this->_fd_response[fd].get_body());
		// response = Response(this->_fd_requests[fd], this->_fd_sconfig[fd]);
		// body = response.get_chunked_body();
		// response_msg = this->_fd_response[fd].get_chunked_msg();
		std::cout << "res.size(): " << res.size() << std::endl;
		send(fd, res.c_str(), res.size(), 0);
		if (this->_fd_response[fd].get_is_complete_response())
			if (close(fd) != 0)
				throw std::runtime_error(std::to_string(__LINE__) + " close error");
	}
	catch (const ServerErrorException& e)
	{	
		this->_logger.warn(e.what());
		if (this->_fd_response.count(fd) == 0)
		{
			this->_fd_response.insert(std::make_pair(fd, Response(e.get_status(), this->_fd_sconfig[fd])));
			// header = this->_fd_response[fd].get_response_header();
			// body = this->_fd_response[fd].get_body();
			res.append(this->_fd_response[fd].get_response_header()).append(this->_fd_response[fd].get_body());
		}
		else
			res.append(this->_fd_response[fd].get_body());
		send(fd, res.c_str(), res.size(), 0);
		if (this->_fd_response[fd].get_is_complete_response())
			if (close(fd) != 0)
				throw std::runtime_error(std::to_string(__LINE__) + " close error");
	}
	catch (const std::exception& e)
	{
		this->_logger.warn(e.what());
		if (this->_fd_response.count(fd) == 0)
		{
			this->_fd_response.insert(std::make_pair(fd, Response(this->_fd_requests[fd], this->_fd_sconfig[fd])));
			// header = this->_fd_response[fd].get_response_header();
			// body = this->_fd_response[fd].get_body();
			res.append(this->_fd_response[fd].get_response_header()).append(this->_fd_response[fd].get_body());
		}
		else
			res.append(this->_fd_response[fd].get_body());
		send(fd, res.c_str(), res.size(), 0);
		if (this->_fd_response[fd].get_is_complete_response())
			if (close(fd) != 0)
				throw std::runtime_error(std::to_string(__LINE__) + " close error");
	}
	
	// send(fd, _example_res, std::strlen(_example_res), 0);
	// if (close(fd) != 0)
	// 	throw std::runtime_error("Pollout select close");
	if (this->_fd_response[fd].get_is_complete_response())
	{
		FD_CLR(fd, &this->_fd_sets.second);
		this->_fd_requests.erase(fd);
		this->_fd_response.erase(fd);
	}
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
		// this->_logger.debug("selecting...");
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
				continue ;
			if (this->is_socketfd(i))
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
