/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TmpResponse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/07 23:21:21 by hyap              #+#    #+#             */
/*   Updated: 2023/03/08 13:59:23 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TmpResponse.hpp"

TmpResponse::TmpResponse(void) {}

TmpResponse::~TmpResponse(void) {}

TmpResponse::TmpResponse(const TmpRequest& req, const ServerConfig& sconfig, char** envp)
	: _req(req), _envp(envp), _is_complete_resposne(true)
{
	this->_response_config = ResponseConfig(this->_req, sconfig);
	if (this->_response_config.get_directives("return").size() > 0)
	{
		this->handle_redirection();
		return ;
	}
	
	std::string	method;
	
	method = this->_req.get_request_field(METHOD);
	if (method == "GET" || method == "HEAD" || method == "POST")
	{
		this->_body = this->read_file();
	}
	else if (method == "POST")
		
	
	if (this->_body.size() > RESPONSE_BUFFER)
	{
		this->_is_complete_resposne = false;
		
		this->_header.set_is_chunked(true);
		this->_header.set_status(S200);
		this->_header.set_content_type(this->_response_config.get_content_type());
		this->_header.construct();
	}
}

void	TmpResponse::handle_redirection(void)
{
	std::stringstream	ss;
	std::string			redirect;
	
	redirect = this->_response_config.get_directives("return")[0];
	ss << "http://";
	ss << this->_req.get_request_field(SERVER_NAME) << ":" << this->_req.get_request_field(PORT);
	if (redirect.front() != '/')
		redirect.insert(redirect.begin(), '/');
	ss << redirect;
	
	this->_header.set_content_type(this->_response_config.get_content_type());
	this->_header.set_location(ss.str());
	this->_header.set_status(S301);
	this->_header.construct();
}

std::string	TmpResponse::read_file(void)
{
	std::ifstream	infile;
	std::string		content;
	
	infile.open(this->_response_config.get_path(), std::ios::binary);
	std::copy(std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>(), content.begin());
#if DEBUG
	this->_logger.debug("response body size: " + std::to_string(this->_body.size()));
#endif
	return (content);
}
