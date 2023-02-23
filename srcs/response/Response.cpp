/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/17 15:03:20 by hyap              #+#    #+#             */
/*   Updated: 2023/02/23 19:00:23 by hyap             ###   ########.fr       */
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
	utils::StrVec							domains;
	size_t									i;
	utils::StrToStrVecMap::const_iterator	it;
	utils::StrToStrVecMap					m;
	std::string								uri;
	
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
	if (this->_directives.find("return") != this->_directives.end())
	{
		this->handle_normal_redirect(this->_directives.find("return")->second[0]);
		return ;
	}
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
	utils::StrVec	sv;
	std::ifstream	infile;
	
	// check server_names
	sv = this->_directives.find("server_name")->second;
	if (!std::count(sv.begin(), sv.end(), "_") && !std::count(sv.begin(), sv.end(), this->_request.get_host()))
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
	// try open path
	infile.open(this->_path);
	if (!infile.good())
	{
		this->_header.set_status(404);
		this->_logger.warn(this->_path + ", Path not found");
		return (true);
	}
	return (false);
}

void	Response::set_directives(const ServerConfig& sconfig)
{
	utils::StrToStrVecMap::const_iterator	it;
	utils::StrToStrVecMap					m;
	std::string								uri;
	
	
	for (; it != sconfig.get_directives().end(); it++) // Save ServerConfig
		this->_directives.insert(std::make_pair(it->first, it->second));
	if (sconfig.get_lconfig().find(this->_request.get_uri()) != sconfig.get_lconfig().end()) // Overwrite if there is location specified
	{
		m = sconfig.get_lconfig().find(uri)->second.get_directives();
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
}

bool	Response::is_cgi(void)
{
	if (this->_path.substr(this->_path.length() - 3, this->_path.length()) == ".py")
		return (true);
	return (false);
}
