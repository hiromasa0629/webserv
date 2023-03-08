/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/07 23:55:57 by hyap              #+#    #+#             */
/*   Updated: 2023/03/08 17:30:14 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseConfig.hpp"

ResponseConfig::ResponseConfig(void) {}

ResponseConfig::~ResponseConfig(void) {}

ResponseConfig::ResponseConfig(const TmpRequest& req, const ServerConfig& sconfig, char** envp) 
	: _req(req), _envp(envp), _autoindex(std::make_pair(false, ResponseAutoindex())), _cgi(std::make_pair(false, ResponseCgi()))
{
	this->set_directives(sconfig);
	if (this->_directives.count("return") > 0)
		return ;
	this->handle_error();
	this->set_path();
}

utils::StrVec	ResponseConfig::get_directives(const std::string& key) const
{
	if (this->_directives.count(key) > 0)
		return (this->_directives.at(key));
	return (utils::StrVec());
}

std::string	ResponseConfig::get_content_type(void) const
{
	return (this->_content_type);
}

std::string	ResponseConfig::get_path(void) const
{
	return (this->_path);
}

const std::pair<bool, ResponseAutoindex>&	ResponseConfig::get_autoindex(void) const
{
	return (this->_autoindex);
}

const std::pair<bool, ResponseCgi>&	ResponseConfig::get_cgi(void) const
{
	return (this->_cgi);
}

void	ResponseConfig::set_directives(const ServerConfig& sconfig)
{
	size_t											second_slash_index;
	utils::StrToStrVecMap::const_iterator			it2;
	std::string										uri;

	uri = this->_req.get_request_field(URI);
	this->_s_directives = sconfig.get_directives();
	this->_directives = this->_s_directives;
	second_slash_index = std::string(uri.begin() + 1, uri.end()).find_first_of('/');
	if (second_slash_index != std::string::npos)
	{
		this->_location_uri = uri.substr(0, second_slash_index + 1);
		this->_rest_of_the_uri = uri.substr(second_slash_index + 1);
		if (this->_rest_of_the_uri.find_first_of('?') != std::string::npos)
			this->_rest_of_the_uri = this->_rest_of_the_uri.substr(0, this->_rest_of_the_uri.find_first_of('?'));
	}
	else
		this->_location_uri = uri;
	if (sconfig.get_lconfig().count(this->_location_uri) > 0)
		this->_l_directives = sconfig.get_lconfig().at(this->_location_uri).get_directives();
	it2 = this->_l_directives.begin();
	for (; it2 != this->_l_directives.end(); it2++)
		this->_directives[it2->first] = it2->second;
}

void	ResponseConfig::set_path(void)
{
	bool			is_autoindex;
	std::string		method;

	method	= this->_req.get_request_field(METHOD);
	is_autoindex = false;
	if (this->_directives.count("autoindex") > 0 && this->_directives["autoindex"][0] == "on" && \
			(method == "GET" || method == "HEAD"))
		is_autoindex = true;
	
	if (method == "GET" || method == "HEAD" || method == "POST" || method == "DELETE")
	{
		this->_path.append(this->_directives["root"][0]);
		if (this->_l_directives.count("root") == 0)	// If location has no root, append uri
			this->_path.append(this->_req.get_request_field(URI));
		else
			this->_path.append(this->_rest_of_the_uri);

		if (opendir(this->_path.c_str()) != NULL)	// if this is directory
		{
			if (this->_directives.count("index") == 0 && !is_autoindex)
				throw ServerErrorException(__LINE__, __FILE__, E403, this->_path + " is a directory");

			std::ifstream	infile;

			if (is_autoindex)
			{
				this->_autoindex.first = true;
				this->_autoindex.second = ResponseAutoindex(this->_path, this->_rest_of_the_uri, this->_req.get_request_field(SERVER_NAME), this->_req.get_request_field(PORT));
			}
			else
			{
				infile.open(this->_path + "/" + this->_directives["index"][0]);
				if (!infile.good())
					throw ServerErrorException(__LINE__, __FILE__, E404, this->_path + " not found");
				else
					this->_path.append("/").append(this->_directives["index"][0]);
			}
		}
		else
		{
			std::ifstream	infile;

			infile.open(this->_path);
			if (!infile.good())
			{
				infile.open(this->_path + ".html");
				if (!infile.good())
					throw ServerErrorException(__LINE__, __FILE__, E404, this->_path + " not found");
				this->_path.append(".html");
			}
		}
		
		if (method == "GET")
		{
			if (this->is_images(this->_path.substr(this->_path.find_last_of('.'))))
				this->_content_type = "image/*";
			else
				this->_content_type = "text/html";
		}
		
		if (method == "POST" || method == "GET")
		{
			std::string	uri;
			std::pair<std::string, std::string>	cgi;

			uri = this->_req.get_request_field(URI);
			cgi = is_cgi(uri);
			if (!cgi.first.empty())
			{
				this->_cgi.first = true;
				this->_cgi.second = ResponseCgi(this->_path, this->_req.get_body(), cgi.first, this->_envp);
			}
		}
	}
	else if (method == "PUT")
	{
		std::string	filename;
		std::string	uri;

		uri = this->_req.get_request_field(URI);
		filename = uri.substr(uri.find_last_of('/'));
		this->_path = ".";
		if (this->_directives.count("upload") > 0)
			this->_path = this->_directives["upload"][0];
		else
			throw ServerErrorException(__LINE__, __FILE__, E400, " bad PUT request");
		this->_path.append(filename);
	}
}

void	ResponseConfig::handle_error(void) const
{
	if (this->_req.get_status_code() != S200)
		throw ServerErrorException(__LINE__, __FILE__, this->_req.get_status_code(), "Bad request");
	if (this->_directives.count("server_name") > 0)
	{
		utils::StrVec	sv;
		std::string		req_server_name;
		
		sv = this->_directives.at("server_name");
		req_server_name = this->_req.get_request_field(SERVER_NAME);
		if (!std::count(sv.begin(), sv.end(), "_") && !std::count(sv.begin(), sv.end(), req_server_name) && !this->is_localhost(req_server_name))
			throw ServerErrorException(__LINE__, __FILE__, E404, req_server_name + " invalid server_name");
	}
	if (this->_directives.count("limit_except") > 0)
	{
		utils::StrVec	sv;
		
		sv = this->_directives.at("limit_except");
		if (!std::count(sv.begin(), sv.end(), this->_req.get_request_field(METHOD)))
			throw ServerErrorException(__LINE__, __FILE__, E405, "Method not allowed");
	}
	if (this->_directives.count("client_max_body_size") > 0)
	{
		int	client_max_body_size;
		
		client_max_body_size = std::stoi(this->_directives.at("client_max_body_size")[0]);
		if (client_max_body_size < this->_req.get_body().size())
			throw ServerErrorException(__LINE__, __FILE__, E413, "Body size too large");
	}
}

bool	ResponseConfig::is_localhost(const std::string& req_server_name) const
{
	return (req_server_name == "localhost" || req_server_name == "127.0.0.1");
}

bool	ResponseConfig::is_images(const std::string& ext) const
{
	if (ext == ".png" || ext == ".jpeg" || ext == ".jpg")
		return (true);
	return (false);
}

// <command, file>
std::pair<std::string, std::string>	ResponseConfig::is_cgi(const std::string& path) const
{
	std::string	file;
	int			index;
	
	file = path;
	if (file.find_first_of('?') != std::string::npos)
		file = file.substr(file.find_first_of('?'));
	while (file.length() > 0)
	{
		std::string		ext;
		utils::StrVec	cgis;
		
		index = file.find_last_of('/');
		ext = file.substr(index);
		cgis = this->_directives.at("cgi");
		file = file.substr(0, index);
		for (size_t i = 0; i < cgis.size(); i++)
		{
			std::string	ext2;
			
			ext2 = cgis[i++];
			if (ext.length() < ext2.length())
				continue ;
			if (ext2 == ext.substr(ext.length() - ext2.length()))
				return (std::make_pair(cgis[i], file));
		}
	}
	return (std::make_pair("", ""));
}
