/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TmpRequest.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/03 17:56:35 by hyap              #+#    #+#             */
/*   Updated: 2023/03/03 22:51:35 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TmpRequest.hpp"

const char	TmpRequest::_methods[] = {"GET"};

TmpRequest::TmpRequest(void) : _is_complete(false), _req("") {}

TmpRequest::~TmpRequest(void) {} 

std::string	TmpRequest::get_req(void) const
{
	return (this->_req);
}

bool	TmpRequest::get_is_complete(void) const
{
	return (this->_is_complete);
}

void	TmpRequest::append(const std::string& req)
{
	this->_req.append(req);
	if (!this->check_header_complete())
		return ;
	
}

bool	TmpRequest::check_header_complete(void)
{
	if (this->_req.length() < 4)
		return (false);
	if (this->_req.find("\r\n\r\n") == std::string::npos)
		return (false);
	return (true);
}

void	TmpRequest::extract_header_info(void)
{
	size_t			index;
	std::string		header;
	utils::StrVec	header_lines;
	utils::StrVec	split;
	
	index = this->_req.find("\r\n\r\n");
	header = std::string(this->_req.begin(), this->_req.begin() + index);
	while ((index = header.find("\r\n")) != std::string::npos)
	{
		header_lines.push_back(std::string(header.begin(), header.begin() + index));
		header = std::string(header.begin() + index + 2, header.end());
	}
	split = utils::ft_split(header_lines[0]);
	
	for (size_t i = 0; i < header_lines.size(); i++)
	{
		split = utils::ft_split(header_lines[i]);
		
	}
	
}
