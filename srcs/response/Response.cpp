/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/17 15:03:20 by hyap              #+#    #+#             */
/*   Updated: 2023/02/18 18:34:17 by hyap             ###   ########.fr       */
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
	utils::StrVec	domains;
	size_t			i;
	
	domains = sconfig.get_directives("server_name");
	i = 0;
	for (; i < domains.size(); i++)
		if (request.get_host() == domains[i] || domains[i] == "_")
			break ;
	if (i == domains.size())
		this->_status == "404";
	
	this->construct_response();
}

void	Response::construct_response(void)
{
	std::string	root;
	std::string	uri;

	root = this->_sconfig.get_directives("root")[0];
	uri = this->_request.get_uri();
	if (this->_sconfig.get_lconfig().find(uri) != this->_sconfig.get_lconfig().end())
		this->_lconfig = this->_sconfig.get_lconfig().find(uri)->second;
	if (root[root.length() - 1] == '/')
		root.pop_back();
	root.append(this->_request.get_uri());
	this->_logger.debug("root: " + root);
}
