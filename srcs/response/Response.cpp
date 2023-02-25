/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/17 15:03:20 by hyap              #+#    #+#             */
/*   Updated: 2023/02/25 23:45:20 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(void)
{
	
}

Response::~Response(void)
{
	
}

Response::Response(const Request& request, const ServerConfig& sconfig) : _request(request), _status("200") //, _has_location_root(false)
{
	this->_uri					= this->_request.get_uri();
	this->_host					= this->_request.get_host();
	this->_request_body			= this->_request.get_body();
	this->_request_body_size	= this->_request.get_body_size();
	this->_method				= this->_request.get_method();
	
	this->set_directives(sconfig);
	this->set_path();
	
	std::cout << "set_path: " << this->_path << std::endl;
	
	if (this->has_handled_error())
		this->handle_error();
	else if (this->is_cgi())
		this->handle_cgi();
	else
		this->handle_normal();
}

void	Response::handle_error(void)
{
	utils::StrVec	vec;
	std::string		root;	
	
	if (this->_directives.find("error_page") == this->_directives.end())
		return ;
	vec = this->_directives.find("error_page")->second;
	this->_path.clear();
	root = this->_directives.find("root")->second[0];
	if (root.back() != '/')
		root.append("/");
	for (size_t i = 0; i < vec.size(); i++)
		if (std::atoi(vec[i++].c_str()) == this->_header.get_status())
			this->_path.append(root).append(vec[i]);
	this->read_path();
	
	this->_header.set_location("");
	this->_header.set_content_length(this->_body.size());
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
	this->read_path();
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
	it = this->_directives.find("server_name");
	if (it != this->_directives.end() && !std::count(sv.begin(), sv.end(), "_") && !std::count(sv.begin(), sv.end(), this->_host) && !this->is_localhost())
	{
		this->_header.set_status(404);
		this->_logger.warn(this->_host + ", Invalid server_name");
		return (true);
	}
	// check limit_except
	it = this->_directives.find("limit_except");
	if (it != this->_directives.end() && !std::count(it->second.begin(), it->second.end(), this->_method))
	{
		this->_header.set_status(405);
		this->_logger.warn(this->_method + ", Method not allowed");
		return (true);
	}
	// Body size
	it = this->_directives.find("client_max_body_size");
	if (it != this->_directives.end() && std::atoi(it->second[0].c_str()) < this->_request.get_body_size())
	{
		std::cout << "request_body_size: " << this->_request_body_size << std::endl;
		this->_header.set_status(413);
		this->_logger.warn("Body size too large");
		return (true);
	}
	// try open path if not (autoindex and redirection)
	if (this->_directives.find("autoindex") != this->_directives.end() && this->_directives.find("return") != this->_directives.end())
		return (false);
	infile.open(this->_path);
	if (!infile.good())
	{
		infile.open(this->_path + ".html");
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

// Set server and location directives and combine them
void	Response::set_directives(const ServerConfig& sconfig)
{
	std::string										location;
	size_t											second_slash_index;
	ServerConfig::StrToLConfigMap::const_iterator	it;
	utils::StrToStrVecMap::const_iterator			it2;
	
	this->_s_directives = sconfig.get_directives();
	second_slash_index = std::string(this->_uri.begin() + 1, this->_uri.end()).find_first_of('/');
	location = this->_uri.substr(0, second_slash_index);
	if ((it = sconfig.get_lconfig().find(location)) != sconfig.get_lconfig().end())
		this->_l_directives = it->second.get_directives();
	this->_directives = this->_s_directives;
	it2 = this->_l_directives.begin();
	for (; it2 != this->_l_directives.end(); it2++)
		this->_directives[it2->first] = it2->second;
	if (second_slash_index != std::string::npos)
		this->_rest_of_the_uri = this->_uri.substr(second_slash_index, this->_uri.length());
	this->_location_uri = location;
}

void	Response::set_path(void)
{
	std::string	path;
	
	if (!this->_l_directives.size()) // No matching location block
	{
		path.append(this->_directives.find("root")->second[0]);
		this->remove_trailing_slash(path);
		path.append(this->_request.get_uri());
		this->_path = path;
	}
	else // Has matching location block
	{
		std::cout << "root: " << this->_directives.find("root")->second[0] << std::endl;
		path.append(this->_directives.find("root")->second[0]);
		
		this->remove_trailing_slash(path);
		if (this->_l_directives.find("root") == this->_l_directives.end())
			path.append(this->_location_uri);
		this->remove_trailing_slash(path);
		// if has more uri
		if (!this->_rest_of_the_uri.empty())
			path.append(this->_rest_of_the_uri);
		else
		{
			if (this->_directives.find("index") != this->_directives.end())
				path.append("/").append(this->_directives.find("index")->second[0]);
		}
	}
	this->_path = path;
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

void	Response::print_directives(void) const
{
	utils::StrToStrVecMap::const_iterator	it;

	it = this->_directives.begin();
	while (it != this->_directives.end())
	{
		std::cout << CYAN << it->first << " " << RESET;
		for (size_t i = 0; i < it->second.size(); i++)
			std::cout << (it->second)[i] << " ";
		std::cout << std::endl;
		it++;
	}
}

void	Response::read_path(void)
{
	std::ifstream	infile;
	std::string		line;
	
	infile.open(this->_path);
	std::cout << "this->_path: " << this->_path << std::endl;
	if (!infile.good())
	{
		infile.open(this->_path + ".html");
		if (!infile.good())
		{
			this->_logger.warn("_path opened failed, defaulted to 404.html");
			infile.open("files/error/404.html");
		}
	}
	while (std::getline(infile, line))
	{
		line.append("\r\n");
		this->_body.insert(this->_body.end(), line.begin(), line.end());
	}
	this->_body.push_back('\r');
	this->_body.push_back('\n');
}

void	Response::remove_trailing_slash(std::string& path)
{
	if (path.back() == '/')
		path.pop_back();
}
