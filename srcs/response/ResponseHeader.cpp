/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseHeader.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/23 16:26:53 by hyap              #+#    #+#             */
/*   Updated: 2023/03/07 20:04:59 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseHeader.hpp"

ResponseHeader::ResponseHeader(void)
{
	this->_codes.insert(std::make_pair(E404, "Not Found"));
	this->_codes.insert(std::make_pair(E403, "Forbidden"));
	this->_codes.insert(std::make_pair(E405, "Method Not Allowed"));
	this->_codes.insert(std::make_pair(E500, "Internal Server Error"));
	this->_codes.insert(std::make_pair(S200, "OK"));
	this->_codes.insert(std::make_pair(S301, "Moved Permanently"));
	this->_codes.insert(std::make_pair(E413, "Request Entity Too Large"));
	this->_is_chunked = false;
}

ResponseHeader::~ResponseHeader(void) {}

void	ResponseHeader::set_status(enum StatusCode status)
{
	this->_status = status;
}

void	ResponseHeader::construct(void)
{
	std::stringstream	ss;
	std::string			s;

	ss << "HTTP/1.1 " << this->_status << " " << this->_codes.find(this->_status)->second << "\r\n";
	// ss << "Connection: keep-alive" << "\r\n";
	// ss << "Content-Type: " << (this->_content_type.empty() ? "text/html" : this->_content_type /*+ "; charset=us-ascii"*/) << "\r\n";
	ss << "Content-Type: text/html" << "\r\n";
	if (this->_content_length != 0)
		ss << "Content-Length: " << this->_content_length << "\r\n";
	if (!this->_location.empty())
		ss << "Location: " << this->_location << "\r\n";
	if (this->_is_chunked)
		ss << "Transfer-Encoding: chunked" << "\r\n";
	ss << "\r\n";
	this->_response_header = ss.str();
}

std::string	ResponseHeader::get_response_header(void) const
{
	return (this->_response_header);
}

void	ResponseHeader::set_location(std::string location)
{
	this->_location = location;
}

void	ResponseHeader::set_content_length(int length)
{
	this->_content_length = length;
}

void	ResponseHeader::set_is_chunked(bool is_chunked)
{
	this->_is_chunked = is_chunked;
}

void	ResponseHeader::set_content_type(std::string type)
{
	this->_content_type = type;
}

enum StatusCode	ResponseHeader::get_status(void) const
{
	return (this->_status);
}

