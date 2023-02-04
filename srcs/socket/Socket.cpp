/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 00:27:54 by hyap              #+#    #+#             */
/*   Updated: 2023/02/04 01:19:45 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

/***********************************
 * Constructors
***********************************/

Socket::Socket(void)
{
	addrinfo_t		hint;
	int				gai;

	hint.ai_flags = AI_PASSIVE;
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_protocol = 0;

	gai = getaddrinfo(NULL, "http", &hint, &(this->_addrinfo));
	if (gai != 0)
		throw std::runtime_error(gai_strerror(gai));
}

Socket::Socket(const Socket &src)
{
	*this = src;
}

Socket	&Socket::operator=(const Socket &rhs)
{
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
	freeaddrinfo(this->_addrinfo);
}
