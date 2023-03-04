/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TmpRequest.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/03 17:56:35 by hyap              #+#    #+#             */
/*   Updated: 2023/03/04 17:37:19 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TmpRequest.hpp"

const char	TmpRequest::_methods[] = {"GET", "POST", "DELETE"};

TmpRequest::TmpRequest(void) : _is_complete(false), _is_complete_header(false), _req("") {}

TmpRequest::~TmpRequest(void) {} 

std::string	TmpRequest::get_req(void) const
{
	return (this->_req);
}

bool	TmpRequest::get_is_complete(void) const
{
	return (this->_is_complete);
}

void	TmpRequest::append(const std::string& req)
{
	this->_req.append(req);
	if (!_is_complete_header)
	{
		if (!this->check_header_complete())
			return ;
		this->extract_header_info();
		this->_is_complete_header = true;
	}
	if (this->_header_info["method"] == "POST")
		this->handle_post(req);
	else
		this->handle_get();
}

bool	TmpRequest::check_header_complete(void)
{
	if (this->_req.length() < 4)
		throw RequestErrorException(__LINE__, 400, "Invalid request length");
	if (this->_req.find("\r\n\r\n") == std::string::npos)
		return (false);
	return (true);
}

bool	TmpRequest::extract_header_info(void)
{
	size_t			index;
	std::string		header;
	utils::StrVec	header_lines;
	utils::StrVec	split;
	
	index = this->_req.find("\r\n\r\n");
	header = std::string(this->_req.begin(), this->_req.begin() + index);
	while ((index = header.find("\r\n")) != std::string::npos)
	{
		header_lines.push_back(std::string(header.begin(), header.begin() + index));
		header = std::string(header.begin() + index + 2, header.end());
	}
	split = utils::ft_split(header_lines[0]);
	if (!this->check_request_line(split))
		throw RequestErrorException(__LINE__, 400, "Request line error");
	this->_header_info.insert(std::make_pair("method", split[0]));
	this->_header_info.insert(std::make_pair("uri", split[1]));
	this->_header_info.insert(std::make_pair("protocol", split[2]));
	for (size_t i = 1; i < header_lines.size(); i++)
	{
		split = utils::ft_split(header_lines[i]);
		if (split.front() == "Host:")
		{
			this->_header_info.insert(std::make_pair("server_name", split[1].substr(0, split[1].find_first_of(':'))));
			this->_header_info.insert(std::make_pair("port", split[1].substr(split[1].find_first_of(':') + 1)));
		}
		else if (split.front() == "Content-Type:")
			this->handle_content_type(utils::StrVec(split.begin() + 1, split.end()));
		else if (split.front() == "Transfer-Encoding:")
			this->_header_info.insert(std::make_pair("transfer-encoding", split.back()));
		else if (split.front() == "Content-Length:")
			this->_header_info.insert(std::make_pair("content-length", split.back()));
	}
	if (this->_header_info.find("server_nama") == this->_header_info.end())
		throw RequestErrorException(__LINE__, 400, "Missing request host");
}	

bool	TmpRequest::check_request_line(const utils::StrVec& vec) const
{
	int	i;
	
	if (vec.empty())
		return (false);
	if (vec.size() < 3)
		return (false);
	i = 0;
	while (i < 3)
	{
		if (TmpRequest::_methods[i] == vec.front())
			break ;
		i++;
	}
	if (i == 3)
		return (false);
	if (vec.back() != "HTTP/1.1")
		return (false);
	if (vec[1].front() != '/')
		return (false);
	return (true);
}

void	TmpRequest::handle_content_type(const utils::StrVec& val)
{
	std::string	type;
	
	type = val.front();
	if (type.back() == ';')
		type.pop_back();
	this->_header_info.insert(std::make_pair("content-type", type));
	if (type == "mutlipart/form-data")
	{
		if (val.size() != 2)
			throw RequestErrorException(__LINE__, 400, "Invalid form-data");
		else
			this->_header_info.insert(std::make_pair("boundary", val.back().substr(val.back().find_first_of('=') + 1)));
	}
}

void	TmpRequest::handle_get(void)
{
	std::string	uri;
	std::string	query;
	int			index;
	
	uri = this->_header_info["uri"];
	if (uri.find_first_of('?') == std::string::npos)
		return ;
	query = uri.substr(uri.find_first_of('?') + 1);
	while ((index = query.find_first_of('&')) != std::string::npos)
	{
		std::string	single;
		int			index_two;
		
		single = query.substr(0, index);
		index_two = single.find_first_of('=');
		this->_query.insert(std::make_pair(single.substr(0, index_two), single.substr(index_two + 1)));
		query = query.substr(index + 1);
	}
}

void	TmpRequest::handle_post(const std::string& req)
{
	if (this->_header_info.count("transfer-encoding") > 0 && this->_header_info["transfer-encoding"] == "chunked")
	{
		if (this->_chunked_body.empty())
			this->_chunked_body = this->_req.substr(this->_req.find("\r\n\r\n") + 4);
		else
			this->_chunked_body.append(req);
		if (this->_chunked_body.find("\r\n0\r\n\r\n"));
		{
			this->_is_complete = true;
			this->_body = this->tidy_up_chunked_body();
		}
	}
	else if (this->_header_info.count("content-length") > 0)
	{
		int			content_length;
		
		content_length = std::stoi(this->_header_info["content-length"]);
		if (this->_body.empty())
		{
			std::string body;
			
			body = this->_req.substr(this->_req.find("\r\n\r\n") + 4);
			if (body.size() > content_length)
				this->_body.append(body, content_length);
			else
				this->_body.append(body);
		}
		else
		{
			int	remaining;
			
			remaining = content_length - this->_body.size();
			if (req.size() > remaining)
				this->_body.append(req, remaining);
			else
				this->_body.append(req);
		}
		if (this->_body.size() == content_length)
			this->_is_complete = true;
	}
}

std::string	TmpRequest::tidy_up_chunked_body(void)
{
	utils::StrVec	lines;
	std::string		body;
	size_t			index;
	int				size;
	std::string		res;
	
	body = this->_chunked_body;
	while ((index = body.find("\r\n")) != std::string::npos)
	{
		lines.push_back(body.substr(0, index));
		body = body.substr(index + 2);
	}
	for (size_t i = 0; i < lines.size(); i++)
	{
		size = std::stoi(lines[i++]);
		for (int j = 0; j < size; j++)
			res.push_back(lines[i][j]);
	}
	return (res);
}

TmpRequest::RequestErrorException::RequestErrorException(int line, int status, const std::string& msg)
	: _line(line), _status(status), _msg(msg) {}

TmpRequest::RequestErrorException::~RequestErrorException(void) throw() {}

const char*	TmpRequest::RequestErrorException::what(void) const throw()
{
	std::stringstream	ss;
	
	ss << this->_status << " ";
	ss << this->_msg << " ";
	ss << "line: " << this->_line;
	return (ss.str().c_str());
}

int	TmpRequest::RequestErrorException::get_status(void) const throw()
{
	return (this->_status);
}

