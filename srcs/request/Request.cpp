/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/15 21:24:22 by hyap              #+#    #+#             */
/*   Updated: 2023/02/27 13:12:47 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Due to the fact that i use std::vector<char> and everytime that i insert or push_back, the capacity will not
// be the same as its size, thus, making the data() function which returns a pointer to the first element isn't
// very accurate. The workaround is using a copy constructor or copy assignment after finish messing with the 
// char vector

#include "Request.hpp"

Request::Request(void) {}

Request::~Request(void) {}

Request::Request(const std::string& req) : _is_complete(false), _method(), _body_size(0), _logger()
{
	(void)_method;

	if (req.empty())
	{
		this->_is_complete = true;
		this->_is_empty_request = true;
		return ;
	}
	this->append(req);
}

bool	Request::get_is_complete(void) const
{
	return (this->_is_complete);
}

void	Request::print_request_header(void) const
{
	std::stringstream	ss;
	if (!this->_is_complete)
		throw std::runtime_error("Request is not completely read yet");
	this->_logger.debug("Method: " + this->_method);
	this->_logger.debug("URI: " + this->_uri);
	this->_logger.debug("Host: " + this->_host);
	this->_logger.debug("Port: " + this->_port);
	this->_logger.debug(" --------------- ");
	for (size_t i = 0; i < this->_header.size(); i++)
	{
		utils::CharVec::const_iterator	it = this->_header[i].begin();
		for (;it != this->_header[i].end(); it++)
			ss << *it;
		this->_logger.debug(ss.str());
		ss.str("");
	}
	for (size_t i = 0; i < this->_body.size(); i++)
	{
		utils::CharVec::const_iterator	it = this->_body[i].begin();
		for (;it != this->_body[i].end(); it++)
			ss << *it;
		this->_logger.debug(ss.str());
		ss.str("");
	}
}

void	Request::append(const std::string& req)
{
	this->_req = req;
	this->check_full_request_header();
	if (!this->_is_complete)
		return ;
	this->extract_header_n_body();
	this->extract_header_info();
}

void	Request::check_full_request_header(void)
{
	if (this->_req.size() < 4)
		return ;
	if (this->is_crlf(this->_req.end() - 4, this->_req.end()))
		this->_is_complete = true;
}

bool	Request::is_crlf(std::string::iterator start, std::string::iterator end) const
{
	std::string			s(start, end);

	if (s == "\r\n\r\n")
		return (true);
	return (false);
}

void	Request::extract_header_n_body(void)
{
	std::string::iterator	it;
	std::string::iterator	start;
	
	it = this->_req.begin();
	start = this->_req.begin();
	for (; (it + 4) != this->_req.end() && !is_crlf(it ,it + 4); it++)
		;
	if (is_crlf(it, it + 4))
		this->_header = this->save_header_n_body(start, it);
	// // null terminate headers
	// for (size_t i = 0; i < this->_header.size(); i++)
	// 	this->_header[i].push_back('\0');
	if ((it + 4) == this->_req.end())
		return ;
	it = it + 4;
	start = it;
	for (; (it + 4) != this->_req.end() && !is_crlf(it ,it + 4); it++)
		;
	this->_body = this->save_header_n_body(start, it);
	for (size_t i = 0; i < this->_body.size(); i++)
		this->_body_size += this->_body[i].size();
	std::cout << "_body_size.size(): " << this->_body.size() << std::endl;
}

std::vector<std::string>	Request::save_header_n_body(std::string::iterator start, std::string::iterator end)
{
	std::vector<std::string>	res;
	std::string					tmp;

	for (; start != end; start++)
	{
		if ((*start == '\r' && (start + 1) != end && *(start + 1) == '\n'))
		{
			start += 1;
			res.push_back(tmp);
			tmp.clear();
		}
		else
			tmp.push_back(*start);
	}
	res.push_back(tmp);
	return (res);
}

void	Request::extract_header_info(void)
{
	std::string								s;
	std::vector<std::string>::iterator		it;
	utils::StrVec							split;
	
	it = this->_header.begin();
	split = utils::ft_split(*it);
	this->_method = split[0];
	this->_uri = split[1];
	if (this->_uri.length() > 1 && this->_uri.back() == '/')	// remove trailing slash except /
		this->_uri.pop_back();
	it++;
	for (; it != this->_header.end(); it++)
	{
		split = utils::ft_split(*it);
		if (split[0] == "Host:")
		{
			this->_host = split[1].substr(0, split[1].find_first_of(':'));
			this->_port = split[1].substr(split[1].find_first_of(':') + 1, split[1].size());
		}
	}
}

std::string	Request::get_method(void) const
{
	return (this->_method);
}

std::string	Request::get_uri(void) const
{
	return (this->_uri);
}

std::string	Request::get_host(void) const
{
	return (this->_host);
}

std::string	Request::get_port(void) const
{
	return (this->_port);
}

std::vector<std::string>	Request::get_body(void) const
{
	return (this->_body);
}

int	Request::get_body_size(void) const
{
	return (this->_body_size);
}

bool	Request::get_is_empty_request(void) const
{
	return (this->_is_empty_request);
}


