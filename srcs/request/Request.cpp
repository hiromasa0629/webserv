/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/15 21:24:22 by hyap              #+#    #+#             */
/*   Updated: 2023/03/02 13:50:42 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Due to the fact that i use std::vector<char> and everytime that i insert or push_back, the capacity will not
// be the same as its size, thus, making the data() function which returns a pointer to the first element isn't
// very accurate. The workaround is using a copy constructor or copy assignment after finish messing with the 
// char vector

#include "Request.hpp"

Request::Request(void) {}

Request::~Request(void) {}

Request::Request(const std::string& req) : _is_complete(false), _method(), _content_length(0), _logger(), _is_empty_request(false), _is_client_side_error(false)
{
	(void)_method;

	// std::cout << req << std::endl;
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
	// if (!this->_is_complete)
	// 	throw std::runtime_error("Request is not completely read yet");
#if DEBUG
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
#else
	this->_logger.info(this->_method + " " + this->_uri + " " + this->_host + ":" + this->_port);
#endif
	// for (size_t i = 0; i < this->_body.size(); i++)
	// {
	// 	utils::CharVec::const_iterator	it = this->_body[i].begin();
	// 	for (;it != this->_body[i].end(); it++)
	// 		ss << *it;
	// 	this->_logger.debug(ss.str());
	// 	ss.str("");
	// }
}

void	Request::append(const std::string& req)
{
	for (size_t i = 0; i < req.size(); i++)
		this->_req.push_back(req[i]);
	// std::cout << "req.size(): " << this->_req.size() << std::endl;
	// for (size_t i = 0; i < this->_req.size(); i++)
	// 	std::cout << (int)this->_req[i] << " ";
	// std::cout << std::endl;
	// if (this->_req.size() < 4)
	// 	return ;
	this->extract_header_n_body();
	this->extract_header_info();
	this->_is_client_side_error = this->is_client_side_error();
	if (this->_is_client_side_error)
		return ;
	// std::cout << "body_size: " << this->get_body_size() << std::endl;
	// std::cout << "content_length: " << this->_content_length << std::endl;
	if (!this->_content_type.empty() && this->_content_type.front() == "multipart/form-data;")
		this->extract_boundary();
	this->_is_complete = true;
	if (this->get_body_size() < this->_content_length)
		this->_is_complete = false;
}

void	Request::check_full_request_header(void)
{
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
	size_t					end_of_header;
	
	it = this->_req.begin();
	start = this->_req.begin();
	
#if DEBUG
	std::cout << "---------------" << std::endl;
	std::cout << this->_req << std::endl;
#endif
	end_of_header = this->_req.find("\r\n\r\n");
	if (end_of_header == std::string::npos)
		return ;
	it += end_of_header;
	this->_header = this->save_header(start, it);
	if ((it + 4) == this->_req.end())
		return ;
	it += 4;
	start = it;
	for (; it != this->_req.end(); it++)
		;
	this->_body = this->save_body(start, it);
}

utils::StrVec	Request::save_header(std::string::iterator start, std::string::iterator end)
{
	utils::StrVec				res;
	std::string					tmp;

	for (; start != end; start++)
	{
		if ((*start == '\r' && (start + 1) != end && *(start + 1) == '\n'))
		{
			start += 1;
			if (!tmp.empty())
				res.push_back(tmp);
			tmp.clear();
		}
		else
			tmp.push_back(*start);
	}
	if (!tmp.empty())
		res.push_back(tmp);
	return (res);
}

std::string	Request::save_body(std::string::iterator start, std::string::iterator end)
{
	std::string	s;
	
	for (; start != end; start++)
		s.push_back(*start);
	return (s);
}

void	Request::extract_header_info(void)
{
	std::string								s;
	utils::StrVec::iterator					it;
	utils::StrVec							split;
	
	// std::cout << "size: " << this->_header.size() << std::endl;
	if (this->_header.empty())
		return ;
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
		if (split[0] == "Content-Length:")
			this->_content_length = std::atoi(split[1].c_str());
		if (split[0] == "Content-Type:")
			this->_content_type = utils::StrVec(split.begin() + 1, split.end());
	}
}

void	Request::extract_boundary(void)
{
	int	equal_index;
	
	equal_index = this->_content_type[1].find_first_of('=');
	// std::cout << this->_content_type[1] << std::endl;
	// std::cout << "equal_index: " << equal_index << std::endl;
	this->_boundary = this->_content_type[1].substr(equal_index + 1, this->_content_type[1].size());
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

std::string	Request::get_body(void) const
{
	return (this->_body);
}

int	Request::get_body_size(void) const
{
	return (this->_body.size());
}

bool	Request::get_is_empty_request(void) const
{
	return (this->_is_empty_request);
}

std::string	Request::get_boundary(void) const
{
	return (this->_boundary);
}

bool	Request::get_is_client_side_error(void) const
{
	return (this->_is_client_side_error);
}

bool	Request::is_client_side_error(void) const
{
	if (this->_host.empty() || this->_port.empty() || this->_uri.empty() || this->_method.empty())
		return (true);
	return (false);
}
