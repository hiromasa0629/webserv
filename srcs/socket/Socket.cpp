/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 00:27:54 by hyap              #+#    #+#             */
/*   Updated: 2023/02/11 16:16:22 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

/***********************************
 * Constructors
***********************************/

Socket::Socket(void) : _addrinfo(NULL), _socketfd(0)
{

}

Socket::Socket(int ai_flags, int ai_family, int ai_socktype, int ai_protocol, const char* hostname, const char* port) : _addrinfo(NULL), _socketfd(0)
{
	try
	{
		this->init_addrinfo(ai_flags, ai_family, ai_socktype, ai_protocol, hostname, port);
		this->init_socket();
	}
	catch (const std::exception& e)
	{
		std::cerr << RED << e.what() << RESET << std::endl;
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

	this->_addrinfo = new addrinfo_t();
	this->get_addrinfo()->ai_flags = rhs.get_addrinfo()->ai_flags;
	this->get_addrinfo()->ai_family = rhs.get_addrinfo()->ai_family;
	this->get_addrinfo()->ai_socktype = rhs.get_addrinfo()->ai_socktype;
	this->get_addrinfo()->ai_protocol = rhs.get_addrinfo()->ai_protocol;
	this->get_addrinfo()->ai_addrlen = rhs.get_addrinfo()->ai_addrlen;

	this->get_addrinfo()->ai_canonname = new char[std::strlen(rhs.get_addrinfo()->ai_canonname)];
	std::memcpy(this->get_addrinfo()->ai_canonname, rhs.get_addrinfo()->ai_canonname, sizeof(char) * std::strlen(rhs.get_addrinfo()->ai_canonname));

	this->get_addrinfo()->ai_addr = new sockaddr_t();
	std::memcpy(this->get_addrinfo()->ai_addr, rhs.get_addrinfo()->ai_addr, sizeof(sockaddr_t));

	this->get_addrinfo()->ai_next = rhs.get_addrinfo()->ai_next;
	return (*this);
}

/***********************************
 * Getters
***********************************/

Socket::addrinfo_t*	Socket::get_addrinfo(void) const
{
	return (this->_addrinfo);
}

/***********************************
 * Destructors
***********************************/

Socket::~Socket(void)
{
	close(this->_socketfd);
	freeaddrinfo(this->_addrinfo);
}

/***********************************
 *	Private Functions
***********************************/

void	Socket::init_addrinfo(int ai_flags, int ai_family, int ai_socktype, int ai_protocol, const char* hostname, const char* port)
{
	addrinfo_t		hint;
	int				gai;

	hint.ai_flags = ai_flags;
	hint.ai_family = ai_family;
	hint.ai_socktype = ai_socktype;
	hint.ai_protocol = ai_protocol;

	gai = getaddrinfo(hostname, port, &hint, &(this->_addrinfo));
	if (gai != 0)
		throw std::runtime_error(gai_strerror(gai));
	std::cout << INFO("Socket::init_addrinfo") << "IP address: " << this->get_ip() << " Port: " << this->get_port() << std::endl;
}

void	Socket::init_socket(void)
{
	this->_socketfd = socket(this->get_addrinfo()->ai_family, this->get_addrinfo()->ai_socktype, this->get_addrinfo()->ai_protocol);
	if (this->_socketfd < 0)
		throw std::runtime_error("[Error] Socket::init_socket()");
	int tmp = 1;
	setsockopt(this->_socketfd, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(tmp) );
	std::cout << INFO("Socket::init_socket (non-blocking)") << std::endl;
	
}

std::string	Socket::get_ip(void) const
{
	std::stringstream		ss;
	struct sockaddr_in*		s;
	unsigned char*	p;
	
	if (this->_addrinfo->ai_addr->sa_family == AF_INET)
	{
		s = (struct sockaddr_in *)this->_addrinfo->ai_addr;
		p = (unsigned char *)&s->sin_addr;
		ss << (unsigned int)p[0] << "." << (unsigned int)p[1] << "." << (unsigned int)p[2] << "." << (unsigned int)p[3];
	}
	return (ss.str());
}

unsigned short	Socket::get_port(void) const
{
	struct sockaddr_in*	s = (struct sockaddr_in *)this->_addrinfo->ai_addr;
	unsigned short	port = ntohs(s->sin_port);
	return (port);
}
