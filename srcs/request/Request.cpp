/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/15 21:24:22 by hyap              #+#    #+#             */
/*   Updated: 2023/02/16 13:53:33 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(void) {}

Request::~Request(void) {}

Request::Request(const utils::CharVec& req) : _req(req), _is_complete(false)
{
	this->check_full_request_header();
	if (!this->_is_complete)
		return ;
	this->extract_header_n_body();
}

bool	Request::get_is_complete(void) const
{
	return (this->_is_complete);
}

void	Request::check_full_request_header(void)
{
	if (this->is_crlf(this->_req.end() - 4, this->_req.end()))
		this->_is_complete = true;
}

bool	Request::is_crlf(utils::CharVec::iterator start, utils::CharVec::iterator end) const
{
	utils::CharVec		str(start, end);
	std::string			s(str.data());
	
	if (s == "\r\n\r\n")
		return (true);
	return (false);
}

void	Request::extract_header_n_body(void)
{
	utils::CharVec::iterator	it;
	utils::CharVec::iterator	start;
	
	it = this->_req.begin();
	start = this->_req.begin();
	for (; (it + 4) != this->_req.end() && !is_crlf(it ,it + 4); it++)
		;
	if ((it + 4) == this->_req.end() && is_crlf(it, it + 4))
	{
		this->_header = this->save_header_n_body(start, it);
		return ;
	}
	it = it + 4;
	start = it;
	for (; (it + 4) != this->_req.end() && !is_crlf(it ,it + 4); it++)
		;
	this->_body = this->save_header_n_body(start, it);
}

std::vector<utils::CharVec>	Request::save_header_n_body(utils::CharVec::iterator start, utils::CharVec::iterator end)
{
	std::vector<utils::CharVec>	res;
	utils::CharVec				tmp;
	
	for (; (start + 1) != end; start++)
	{
		if (*start == '\r' && *(start + 1) == '\n')
		{
			start += 2;
			res.push_back(tmp);
			tmp.clear();
		}
		else
			tmp.push_back(*start);
	}
	return (res);
}

