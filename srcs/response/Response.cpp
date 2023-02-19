/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/17 15:03:20 by hyap              #+#    #+#             */
/*   Updated: 2023/02/19 19:02:35 by hyap             ###   ########.fr       */
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
	
	domains = sconfig.get_directives("server_name");
	i = 0;
	for (; i < domains.size(); i++)
		if (request.get_host() == domains[i] || domains[i] == "_")
			break ;
	if (i == domains.size())
		this->_status == "404";
	it = sconfig.get_directives().begin();
	for (; it != sconfig.get_directives().end(); it++) // Save ServerConfig
		this->_directives.insert(std::make_pair(it->first, it->second));
	if (sconfig.get_lconfig().find(uri) != sconfig.get_lconfig().end()) // Overwrite if there is location specified
	{
		m = sconfig.get_lconfig().find(uri)->second.get_directives();
		it = m.begin();
		for (; it != m.end(); it++)
			this->_directives[it->first] = it->second;
	}
	this->construct_response();
}

bool	Response::has_handled_error(void)
{
	// check server_names
	// check limit_except
	// try open path if not cgi
}

void	Response::construct_response(void)
{
	std::string	s;
	
	this->_path = this->_request.get_host();
	if (this->_path[s.length() - 1] == '/')
		this->_path.pop_back();
	this->_path.append(this->_request.get_uri());
}
