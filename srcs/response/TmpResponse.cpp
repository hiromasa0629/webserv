/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TmpResponse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/07 23:21:21 by hyap              #+#    #+#             */
/*   Updated: 2023/03/09 01:18:59 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TmpResponse.hpp"

TmpResponse::TmpResponse(void) {}

TmpResponse::~TmpResponse(void) {}

TmpResponse::TmpResponse(const TmpRequest& req, const ServerConfig& sconfig, char** envp)
	: _req(req), _is_complete_resposne(true), _response_config(this->_req, sconfig, envp)
{
	std::string	method;

	method = this->_req.get_request_field(METHOD);

	if (this->_response_config.get_redirection().first)
	{
		//	Is redirection
		this->handle_redirection(this->_response_config.get_redirection().second);
	}
	else if (this->_response_config.get_put().first)
	{
		// Is PUT request
		this->handle_put(this->_response_config.get_put().second);
	}
	else if (this->_response_config.get_autoindex().first)
	{
		// Is Directory Listing
		this->handle_autoindex(this->_response_config.get_autoindex().second.get_body());
	}
	else if (this->_response_config.get_cgi().first)
	{
		//	Is CGI
		this->handle_cgi(this->_response_config.get_cgi().second.get_response_msg());
	}
	else if (method == "DELETE")
	{
		// Is DELETE request
		this->handle_delete(this->_response_config.get_path());
	}
	else
	{
		this->handle_normal(this->_response_config.get_path());
	}

	if (this->_body.size() > RESPONSE_BUFFER)
	{
		this->_is_complete_resposne = false;

		this->_header.set_is_chunked(true);
		this->_header.set_status(S200);
		this->_header.construct();
	}
}

void	TmpResponse::handle_redirection(const std::string& uri)
{
	std::stringstream	ss;
	std::string			redirect;

	redirect = this->_response_config.get_directives("return")[0];
	ss << "http://";
	ss << this->_req.get_request_field(SERVER_NAME) << ":" << this->_req.get_request_field(PORT);
	ss << uri;

	this->_header.set_location(ss.str());
	this->_header.set_status(S301);
	this->_header.construct();
}

void	TmpResponse::handle_put(const std::string& path)
{
	std::ofstream	outfile;

	outfile.open(path);
	if (!outfile.good())
		throw ServerErrorException(__LINE__, __FILE__, E500, "Bad PUT request");
	outfile.write(this->_req.get_body().c_str(), this->_req.get_body().size());
	outfile.close();

	this->_header.set_status(S200);
	this->_header.set_location("http://" + this->_req.get_request_field(SERVER_NAME) + ":" + this->_req.get_request_field(PORT) + "/" + path);
	this->_header.construct();
}

void	TmpResponse::handle_cgi(const std::string& cgi_msg)
{
	this->_header.construct(cgi_msg.substr(0, cgi_msg.find("\r\n\r\n")));
	this->_body = cgi_msg.substr(cgi_msg.find("\r\n\r\n") + 4);
}

void	TmpResponse::handle_autoindex(const std::string& body)
{
	this->_body = body;

	this->_header.set_status(S200);
	this->_header.set_content_length(this->_body.size());
	this->_header.construct();
}

void	TmpResponse::handle_delete(const std::string& path)
{
	if (std::remove(this->_response_config.get_path().c_str()) != 0)
		throw ServerErrorException(__LINE__, __FILE__, E500, "std::remove failed");
	this->_header.set_status(S200);
	this->_header.construct();
}

void	TmpResponse::handle_normal(const std::string& path)
{
	this->_body = this->read_file(path);

	this->_header.set_status(S200);
	this->_header.set_content_length(this->_body.size());
	this->_header.construct();
	if (this->_req.get_request_field(METHOD) == "HEAD")
		this->_body.clear();
}

std::string	TmpResponse::read_file(const std::string& path)
{
	std::ifstream	infile;
	std::string		content;

	infile.open(path, std::ios::binary);
	std::copy(std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>(), content.begin());
#if DEBUG
	this->_logger.debug("response body size: " + std::to_string(this->_body.size()));
#endif
	return (content);
}
