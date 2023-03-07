/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/07 23:55:57 by hyap              #+#    #+#             */
/*   Updated: 2023/03/08 01:14:56 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseConfig.hpp"

ResponseConfig::ResponseConfig(void) {}

ResponseConfig::~ResponseConfig(void) {}

ResponseConfig::ResponseConfig(const TmpRequest& req, const ServerConfig& sconfig) : _req(req)
{
	this->set_directives(sconfig);
	this->set_path();
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
	std::string	method;

	method = this->_req.get_request_field(METHOD);
	if (method == "GET" || method == "HEAD" || method == "POST" || method == "DELETE")
	{
		this->_path.append(this->_directives["root"][0]);
		if (this->_l_directives.count("root") == 0)	// If location has no root, append uri
			this->_path.append(this->_req.get_request_field(URI));
		else
			this->_path.append(this->_rest_of_the_uri);

		if (opendir(this->_path.c_str()) != NULL)
		{
			if (this->_directives.count("autoindex") > 0 && this->_directives["autoindex"][0] == "on")
			{
				// this->_autoindex = ResponseAutoindex(this->_path, )
			}
			else
			{
				if (this->_directives.count("index") == 0)
					throw ServerErrorException(__LINE__, __FILE__, E403, this->_path + " is a directory");

				std::ifstream	infile;

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
			if (infile.good())
				return ;
			infile.open(this->_path + ".html");
			if (infile.good())
				this->_path.append(".html");
			else
				throw ServerErrorException(__LINE__, __FILE__, E404, this->_path + " not found");
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
		this->_path.append(filename);
	}
}

