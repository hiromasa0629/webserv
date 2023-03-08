/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseAutoindex.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/26 21:14:01 by hyap              #+#    #+#             */
/*   Updated: 2023/03/08 18:36:41 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseAutoindex.hpp"

ResponseAutoindex::ResponseAutoindex(void) {}

ResponseAutoindex::~ResponseAutoindex(void) {}

ResponseAutoindex::ResponseAutoindex(const std::string& path, const std::string& rest_of_the_uri,  const std::string& host, const std::string& port)
	: _path(path), _host(host), _port(port)
{
	DIR*				dir;
	struct dirent*		dir_ent;
	std::stringstream	ss;
	
	if ((dir = opendir(this->_path.c_str())) == NULL)
		throw ServerErrorException(__LINE__, __FILE__, E500, this->_path + " opendir failed");
	while ((dir_ent = readdir(dir)))
	{
		if (rest_of_the_uri.empty() && (!std::strcmp(dir_ent->d_name, ".") || !std::strcmp(dir_ent->d_name, "..")))
			continue ;
		this->construct_href(ss, dir_ent->d_name);
	}
	closedir(dir);
	this->construct_body(ss.str());
}

// ResponseAutoindex::ResponseAutoindex(const std::string& path, const std::string& rest_of_the_uri, const std::string& host, const std::string& port, const std::string& location_uri)
// {
// 	this->_path				= path;
// 	this->_rest_of_the_uri	= rest_of_the_uri;
// 	this->_host 			= host;
// 	this->_port 			= port;
// 	this->_location_uri 	= location_uri;
	
// 	this->_path.append(rest_of_the_uri);
// 	this->_location_uri.append(rest_of_the_uri);
	
// 	DIR*				dir;
// 	struct dirent*		dir_ent;
// 	std::stringstream	ss;
	
// 	if ((dir = opendir(this->_path.c_str())) == NULL)
// 		throw std::runtime_error(this->_path + " Opendir failed");
// 	while ((dir_ent = readdir(dir)))
// 	{
// 		if (this->_rest_of_the_uri.empty() && (!std::strcmp(dir_ent->d_name, ".") || !std::strcmp(dir_ent->d_name, ".."))) // if its root ignore . and ..
// 			continue ;
// 		this->construct_href(ss, dir_ent->d_name);
// 	}
// 	closedir(dir);
// 	this->construct_body(ss.str());
// }

void	ResponseAutoindex::construct_href(std::stringstream& ss, const char* dirname) const
{
	std::string	s(dirname);
	
	ss << "<p>";
	ss << "<a href=\"http://" << this->_host << ":" << this->_port;
	if (s == ".")
		ss << this->_path;
	else if (s == "..")
		ss << this->_path.substr(0, this->_path.find_last_of('/'));
	else
		ss << this->_path << "/" << dirname;
	ss << "\">" << dirname;
	ss << "</a>";
	ss << "</p>";
}

void	ResponseAutoindex::construct_body(const std::string& s)
{
	this->_body = "<!DOCTYPE html>";
	this->_body.append("<html>");
	this->_body.append("<h1>").append(this->_path).append("</h1>");
	this->_body.append("<hr></hr>");
	this->_body.append(s);
	this->_body.append("</html>");
}

std::string	ResponseAutoindex::get_body(void) const
{
	return (this->_body);
}
