/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseHeader.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/23 16:26:53 by hyap              #+#    #+#             */
/*   Updated: 2023/02/23 18:35:58 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseHeader.hpp"

ResponseHeader::ResponseHeader(void)
{
	this->_codes.insert(std::make_pair(404, "Not Found"));
	this->_codes.insert(std::make_pair(405, "Method Not Allowed"));
	this->_codes.insert(std::make_pair(500, "Internal Server Error"));
	this->_codes.insert(std::make_pair(200, "OK"));
	this->_codes.insert(std::make_pair(301, "Moved Permanently"));
}

ResponseHeader::~ResponseHeader(void) {}

void	ResponseHeader::set_status(int status)
{
	this->_status = status;
}

void	ResponseHeader::construct(void)
{
	std::stringstream	ss;
	
	ss << "HTTP/1.1 " << this->_status << " " << this->_codes.find(this->_status)->second << "\r\n";
	ss << "Content-Type: text/html;\r\n";
	ss << "Content-Length: " << this->_content_length << "\r\n";
	if (this->_status == 301)
		ss << "Location: " << this->_location << "\r\n";
	ss << "\r\n";
	this->_res.insert(this->_res.begin(), ss.str().begin(), ss.str().end());
}

utils::CharVec	ResponseHeader::get_res(void) const
{
	return (this->_res);
}

void	ResponseHeader::set_location(std::string location)
{
	this->_location = location;
}

void	ResponseHeader::set_content_length(int length)
{
	this->_content_length = length;
}