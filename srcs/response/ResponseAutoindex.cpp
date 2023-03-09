/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseAutoindex.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/26 21:14:01 by hyap              #+#    #+#             */
/*   Updated: 2023/03/09 14:50:54 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseAutoindex.hpp"

ResponseAutoindex::ResponseAutoindex(void) {}

ResponseAutoindex::~ResponseAutoindex(void) {}

ResponseAutoindex::ResponseAutoindex(const std::string& path, const std::string& uri,  const std::string& host, const std::string& port)
	: _path(path), _host(host), _port(port), _uri(uri)
{
	DIR*				dir;
	struct dirent*		dir_ent;
	std::stringstream	ss;

	if ((dir = opendir(this->_path.c_str())) == NULL)
		throw ServerErrorException(__LINE__, __FILE__, E500, this->_path + " opendir failed");
	while ((dir_ent = readdir(dir)))
	{
		if (std::count(uri.begin(), uri.end(), '/') == 1 && (!std::strcmp(dir_ent->d_name, ".") || !std::strcmp(dir_ent->d_name, "..")))
			continue ;
		this->construct_href(ss, dir_ent->d_name);
	}
	closedir(dir);
	this->construct_body(ss.str());
}

void	ResponseAutoindex::construct_href(std::stringstream& ss, const char* dirname) const
{
	std::string	s(dirname);

	ss << "<p>";
	ss << "<a href=\"http://" << this->_host << ":" << this->_port;
	if (s == ".")
		ss << this->_uri;
	else if (s == "..")
		ss << this->_uri.substr(0, this->_uri.find_last_of('/'));
	else
		ss << this->_uri << "/" << dirname;
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
