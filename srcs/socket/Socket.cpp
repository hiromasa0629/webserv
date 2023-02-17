/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 00:27:54 by hyap              #+#    #+#             */
/*   Updated: 2023/02/17 17:31:09 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

/***********************************
 * Constructors
***********************************/

Socket::Socket(void) : _logger()
{

}

Socket::Socket(int ai_flags, int ai_family, int ai_socktype, int ai_protocol, const ServerConfig& sconfig) : _logger()
{
	addrinfo_t	hint;
	
	this->_config = sconfig;
	
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
		this->_logger.error<std::string>(e.what());
		std::exit(errno);
	}
}

Socket::Socket(const Socket &src)
{
	*this = src;
}

Socket&	Socket::operator=(const Socket &rhs)
{
	if (this == &rhs)
		return (*this);
		
	addrinfo_t*	tmp;
	addrinfo_t*	tmpaddr;
	addrinfo_t* rhsaddr;

	tmp = new addrinfo_t();
	tmp->ai_flags = rhs._addrinfo->ai_flags;
	tmp->ai_family = rhs._addrinfo->ai_family;
	tmp->ai_socktype = rhs._addrinfo->ai_socktype;
	tmp->ai_protocol = rhs._addrinfo->ai_protocol;
	tmp->ai_addrlen = rhs._addrinfo->ai_addrlen;
	
	if (rhs._addrinfo->ai_canonname != NULL)
	{
		tmp->ai_canonname = new char[std::strlen(rhs._addrinfo->ai_canonname)];
		std::memcpy(tmp->ai_canonname, rhs._addrinfo->ai_canonname, sizeof(char) * std::strlen(rhs._addrinfo->ai_canonname));
	}
	else
		tmp->ai_canonname = NULL;
	rhsaddr = rhs._addrinfo;
	tmpaddr = tmp;
	while (rhsaddr)
	{
		tmpaddr->ai_addr = new sockaddr_t();
		std::memcpy(tmpaddr->ai_addr, rhs._addrinfo->ai_addr, sizeof(sockaddr_t));
		tmpaddr = tmpaddr->ai_next;
		rhsaddr = rhsaddr->ai_next;
	}
	this->_addrinfo = tmp;
	this->_socketfd = rhs.get_socketfd();
	this->_config = rhs._config;
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
	freeaddrinfo(this->_addrinfo);
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
	
	
	host = this->_config.get_directives("listen")[0];
	port = this->_config.get_directives("listen")[1];
	gai = getaddrinfo(host.c_str(), port.c_str(), &hint, &(this->_addrinfo));
	if (gai != 0)
		throw std::runtime_error(host + ":" + port + " " + gai_strerror(gai));
	this->_logger.info<std::string>("Socket::init_addrinfo " + *this);
}

void	Socket::init_socket(void)
{
	int tmp = 1;

	this->_socketfd = socket(this->_addrinfo->ai_family, this->_addrinfo->ai_socktype, this->_addrinfo->ai_protocol);
	if (this->_socketfd < 0)
		throw std::runtime_error("Socket::init_socket()");
	setsockopt(this->_socketfd, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(tmp));
	this->_logger.info<std::string>("Socket::init_socket " + *this);
}

std::string	Socket::get_ip(void) const
{
	std::stringstream		ss;
	struct sockaddr_in*		s;
	unsigned char*			p;
	
	if (this->_addrinfo->ai_addr->sa_family == AF_INET)
	{
		s = (struct sockaddr_in *)this->_addrinfo->ai_addr;
		p = (unsigned char *)&s->sin_addr;
		ss << (unsigned int)p[0] << "." << (unsigned int)p[1] << "." << (unsigned int)p[2] << "." << (unsigned int)p[3];
	}
	return (ss.str());
}

std::string	Socket::get_port(void) const
{
	std::stringstream	ss;

	struct sockaddr_in*	s = (struct sockaddr_in *)this->_addrinfo->ai_addr;
	unsigned short	port = ntohs(s->sin_port);
	ss << port;
	return (ss.str());
}

int	Socket::get_socketfd(void) const
{
	return (this->_socketfd);
}

Socket::addrinfo_t*	Socket::get_addrinfo(void) const
{
	return (this->_addrinfo);
}

std::string	operator+(const char* s, const Socket& socket)
{
	std::stringstream	ss;
	
	ss << s << socket.get_ip() << ":" << socket.get_port();
	return (ss.str());
}

std::ostream&	operator<<(std::ostream& o, const Socket& socket)
{
	o << socket.get_ip() << ":" << socket.get_port();
	return (o);
}
