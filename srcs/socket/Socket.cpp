/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 00:27:54 by hyap              #+#    #+#             */
/*   Updated: 2023/02/14 19:08:50 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

/***********************************
 * Constructors
***********************************/

Socket::Socket(void)
{

}

Socket::Socket(int ai_flags, int ai_family, int ai_socktype, int ai_protocol, const Config& config)
{
	addrinfo_t	hint;
	
	this->_config = config;
	
	hint.ai_flags = ai_flags;
	hint.ai_family = ai_family;
	hint.ai_socktype = ai_socktype;
	hint.ai_protocol = ai_protocol;
	try
	{
		this->init_addrinfo(hint);
		this->init_socket();
	}
	catch (const std::exception& e)
	{
		std::cerr << RED << e.what() << RESET << std::endl;
		std::exit(1);
	}
}

Socket::Socket(const Socket &src)
{
	*this = src;
}

Socket	&Socket::operator=(const Socket &rhs)
{
	if (this == &rhs)
		return (*this);
	addrinfo_t*	tmp;
	for (size_t i = 0; i < rhs._addrinfos.size(); i++)
	{
		tmp = new addrinfo_t();
		tmp->ai_flags = rhs._addrinfos[i]->ai_flags;
		tmp->ai_family = rhs._addrinfos[i]->ai_family;
		tmp->ai_socktype = rhs._addrinfos[i]->ai_socktype;
		tmp->ai_protocol = rhs._addrinfos[i]->ai_protocol;
		tmp->ai_addrlen = rhs._addrinfos[i]->ai_addrlen;
		
		tmp->ai_canonname = new char[std::strlen(rhs._addrinfos[i]->ai_canonname)];
		std::memcpy(tmp->ai_canonname, rhs._addrinfos[i]->ai_canonname, sizeof(char) * std::strlen(rhs._addrinfos[i]->ai_canonname));

		tmp->ai_addr = new sockaddr_t();
		std::memcpy(tmp->ai_addr, rhs._addrinfos[i]->ai_addr, sizeof(sockaddr_t));
		
		tmp->ai_next = rhs._addrinfos[i]->ai_next;
		this->_addrinfos.push_back(tmp);
	}
	this->_socketfds = rhs._socketfds;
	return (*this);
}

/***********************************
 * Getters
***********************************/

/***********************************
 * Destructors
***********************************/

Socket::~Socket(void)
{
	SocketFdsMap::iterator	it;
	
	it = this->_socketfds.begin();
	for (; it != this->_socketfds.end(); it++)
	{
		close(it->first);
		freeaddrinfo(it->second.first);
	}
}

/***********************************
 *	Private Functions
***********************************/

void	Socket::init_addrinfo(const addrinfo_t& hint)
{
	int									gai;
	Config::StrToSConfigMap				configs;
	Config::StrToSConfigMap::iterator	it;
	std::string							host;
	std::string							port;
	
	
	configs = this->_config.get_config();
	this->_addrinfos.insert(this->_addrinfos.begin(), configs.size(), NULL);
	it = configs.begin();
	for (size_t i = 0; it != configs.end(); it++, i++)
	{
		host = it->second.get_directives("listen")[0];
		port = it->second.get_directives("listen")[1];
		gai = getaddrinfo(host.c_str(), port.c_str(), &hint, &(this->_addrinfos[i]));
		if (gai != 0)
			throw std::runtime_error(host + " " + port + " " + gai_strerror(gai));
		std::cout << INFO("Socket::init_addrinfo") << "IP address: " << this->get_ip(this->_addrinfos[i]) << " Port: " << this->get_port(this->_addrinfos[i]) << std::endl;
	}
}

void	Socket::init_socket(void)
{
	int	tmpfd;
	int tmp = 1;
	Config::StrToSConfigMap::const_iterator	it;
	
	it = this->_config.get_config().begin();
	for (size_t i = 0; i < this->_addrinfos.size(); i++, it++)
	{
		tmpfd = socket(this->_addrinfos[i]->ai_family, this->_addrinfos[i]->ai_socktype, this->_addrinfos[i]->ai_protocol);
		if (tmpfd < 0)
			throw std::runtime_error("[Error] Socket::init_socket()");
		setsockopt(tmpfd, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(tmpfd));
		this->_socketfds.insert(std::make_pair(tmpfd, std::make_pair(this->_addrinfos[i], it->first)));
		std::cout << INFO("Socket::init_socket") << std::endl;
	}
	
}

std::string	Socket::get_ip(addrinfo_t *a) const
{
	std::stringstream		ss;
	struct sockaddr_in*		s;
	unsigned char*			p;
	
	if (a->ai_addr->sa_family == AF_INET)
	{
		s = (struct sockaddr_in *)a->ai_addr;
		p = (unsigned char *)&s->sin_addr;
		ss << (unsigned int)p[0] << "." << (unsigned int)p[1] << "." << (unsigned int)p[2] << "." << (unsigned int)p[3];
	}
	return (ss.str());
}

unsigned short	Socket::get_port(addrinfo_t *a) const
{
	struct sockaddr_in*	s = (struct sockaddr_in *)a->ai_addr;
	unsigned short	port = ntohs(s->sin_port);
	return (port);
}

bool	Socket::is_socketfd(int fd)
{
	SocketFdsMap::iterator	it;
	
	it = this->_socketfds.begin();
	for (; it != this->_socketfds.end(); it++)
		if (it->first == fd)
			return (true);
	return (false);
}
