/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/17 15:03:20 by hyap              #+#    #+#             */
/*   Updated: 2023/02/17 18:56:26 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(void)
{
	
}

Response::~Response(void)
{
	
}

Response::Response(const Request& request, const Config& config) : _request(request)
{
	Config::StrToSConfigMap::const_iterator	it;
	if ((it = config.get_sconfig().find(request.get_host())) != config.get_sconfig().end()) // has server_name
		this->_sconfig = it->second;
	else if ()
	{
		if (config.find_sconfig_by_host_port(request.get_host(), request.get_port()) == config.get_sconfig().end())
	}
}
