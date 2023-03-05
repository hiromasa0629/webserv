/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerErrorException.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/05 19:22:15 by hyap              #+#    #+#             */
/*   Updated: 2023/03/05 19:45:09 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerErrorException.hpp"

ServerErrorException::ServerErrorException(int line, const char* file, enum StatusCode status, const std::string& msg)
	: _line(line), _status(status)
{
	std::stringstream	ss;
	
	ss << this->_status << " " << (this->_status == E400 ? "Bad Request: " : "");
	ss << msg << " ";
	ss << "(" << file << ": " << this->_line << ")";
	this->_msg = ss.str();
}

ServerErrorException::~ServerErrorException(void) throw() {}

const char*	ServerErrorException::what(void) const throw()
{
	return (this->_msg.c_str());
}

enum StatusCode	ServerErrorException::get_status(void) const throw()
{
	return (this->_status);
}
