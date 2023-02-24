/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/17 15:03:20 by hyap              #+#    #+#             */
/*   Updated: 2023/02/24 23:48:38 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(void)
{
	
}

Response::~Response(void)
{
	
}

Response::Response(const Request& request, const ServerConfig& sconfig) : _request(request), _status("200")
{
	this->set_directives(sconfig);
	this->set_path();

	if (this->has_handled_error())
		this->handle_error();
	else if (this->is_cgi())
		this->handle_cgi();
	else
		this->handle_normal();
}

void	Response::handle_error(void)
{
	this->_header.set_location("");
	this->_header.set_content_length(0);
	this->_header.construct();
}

void	Response::handle_cgi(void)
{
	
}

void	Response::handle_normal(void)
{
	std::ifstream	infile;
	std::string		line;
	
	if (this->_directives.find("return") != this->_directives.end())
	{
		this->handle_normal_redirect(this->_directives.find("return")->second[0]);
		return ;
	}
	infile.open(this->_path);
	while (std::getline(infile, line))
	{
		line.append("\r\n");
		this->_body.insert(this->_body.end(), line.begin(), line.end());
	}
	this->_body.push_back('\r');
	this->_body.push_back('\n');
	this->_body.push_back('\0');
	
	this->_header.set_content_length(this->_body.size());
	this->_header.set_location("");
	this->_header.set_status(200);
	this->_header.construct();
}

void	Response::handle_normal_redirect(std::string redirect)
{
	std::stringstream	ss;
	
	ss << this->_request.get_host() << ":" << this->_request.get_port();
	if (redirect.front() != '/')
		redirect.insert(redirect.begin(), '/');
	ss << redirect;
	this->_header.set_content_length(0);
	this->_header.set_location(ss.str());
	this->_header.set_status(301);
	this->_header.construct();
}

bool	Response::has_handled_error(void)
{
	utils::StrVec					sv;
	utils::StrToStrVecMap::iterator	it;
	std::ifstream					infile;
	
	// check server_names
	sv = this->_directives.find("server_name")->second;
	if (!std::count(sv.begin(), sv.end(), "_") && !std::count(sv.begin(), sv.end(), this->_request.get_host()) && !this->is_localhost())
	{
		this->_header.set_status(404);
		this->_logger.warn(this->_request.get_host() + ", Invalid server_name");
		return (true);
	}
	// check limit_except
	sv = this->_directives.find("limit_except")->second;
	if (!std::count(sv.begin(), sv.end(), this->_request.get_method()))
	{
		this->_header.set_status(405);
		this->_logger.warn(this->_request.get_method() + ", Method not allowed");
		return (true);
	}
	// Body size
	it = this->_directives.find("client_max_body_size");
	if (it != this->_directives.end() && std::atoi(it->second[0].c_str()) < this->_request.get_body_size())
	{
		this->_header.set_status(413);
		this->_logger.warn("Body size too large");
		return (true);
	}
	// try open path if not (autoindex and redirection)
	if (this->_directives.find("autoindex") == this->_directives.end() || this->_directives.find("return") == this->_directives.end())
		return (false);
	infile.open(this->_path);
	if (!infile.good())
	{
		infile.open(this->_path.append(".html"));
		if (!infile.good())
		{
			this->_header.set_status(404);
			this->_logger.warn(this->_path + ", Path not found");
			return (true);
		}
	}
	this->_logger.debug("Passed error handle");
	return (false);
}

void	Response::set_directives(const ServerConfig& sconfig)
{
	utils::StrToStrVecMap::const_iterator	it;
	utils::StrToStrVecMap					m;
	std::string								trimmed_uri; // eg. /folder/file.html -> /folder
	size_t									second_slash_index = 0;
	std::string								tmp;
	
	it = sconfig.get_directives().begin();
	for (; it != sconfig.get_directives().end(); it++) // Save ServerConfig
		this->_directives.insert(std::make_pair(it->first, it->second));
	tmp = this->_request.get_uri();
	second_slash_index = std::string(tmp.begin() + 1, tmp.end()).find_first_of('/');
	trimmed_uri = this->_request.get_uri().substr(0, second_slash_index);
	if (sconfig.get_lconfig().find(trimmed_uri) != sconfig.get_lconfig().end()) // Overwrite if there is location specified
	{
		m = sconfig.get_lconfig().find(trimmed_uri)->second.get_directives();
		it = m.begin();
		for (; it != m.end(); it++)
			this->_directives[it->first] = it->second;
	}
}

void	Response::set_path(void)
{
	std::string	root;
	
	root = this->_directives.find("root")->second[0];
	if (root.back() == '/')
		root.pop_back();
	this->_path = root.append(this->_request.get_uri());
	if (this->_path.back() == '/')
		this->_path.pop_back();
	if (this->_directives.find("index") != this->_directives.end())
	{
		this->_path.append("/");
		this->_path.append(this->_directives.find("index")->second[0]);
	}
	
}

bool	Response::is_cgi(void)
{
	if (this->_path.substr(this->_path.length() - 3, this->_path.length()) == ".py")
		return (true);
	return (false);
}

utils::CharVec		Response::get_response_header(void) const
{
	utils::CharVec	header;
	
	header = this->_header.get_responds_header();
	return (header);
}

const utils::CharVec&	Response::get_body(void) const
{
	return (this->_body);
}

bool	Response::is_localhost(void) const
{
	return (this->_request.get_host() == "localhost" || this->_request.get_host() == "127.0.0.1");
}
