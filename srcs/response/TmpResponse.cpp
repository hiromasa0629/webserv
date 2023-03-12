/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TmpResponse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/07 23:21:21 by hyap              #+#    #+#             */
/*   Updated: 2023/03/12 16:14:20 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TmpResponse.hpp"

TmpResponse::TmpResponse(void) {}

TmpResponse::~TmpResponse(void) {}

TmpResponse::TmpResponse(enum StatusCode status, const ServerConfig& sconfig)
	: _is_complete_response(true)
{

	this->handle_error(status, sconfig);

	this->_header.set_status(status);
	this->_header.set_content_length(this->_body.size());
	this->_header.construct();
}

TmpResponse::TmpResponse(const TmpRequest& req, const ServerConfig& sconfig, char** envp)
	: _req(req), _is_complete_response(true), _response_config(this->_req, sconfig, envp)
{
	if (this->_req.get_status_code() != S200)
		throw ServerErrorException(__LINE__, __FILE__, E400, "Bad request");

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
#if DEBUG
		this->_logger.debug("Sending size " + std::to_string(this->_body.size()) + " in chunked response...");
#endif
		this->_is_complete_response = false;

		this->_header.set_is_chunked(true);
		this->_header.set_content_length(0);
		this->_header.set_status(S200);
		this->_header.construct();
	}
}



std::string	TmpResponse::get_body(void)
{
	if (!this->_is_complete_response)
	{
		std::string	chunked_body;

		if  (this->_body.empty())
		{
			chunked_body.append("0\r\n\r\n");
			this->_is_complete_response = true;
			return (chunked_body);
		}
		if (this->_body.size() > RESPONSE_BUFFER)
		{
			chunked_body.append(utils::to_hex(RESPONSE_BUFFER)).append("\r\n");
			chunked_body.append(this->_body.c_str(), RESPONSE_BUFFER).append("\r\n");
			this->_body = this->_body.substr(RESPONSE_BUFFER);
		}
		else
		{
			chunked_body.append(utils::to_hex(this->_body.size())).append("\r\n");
			chunked_body.append(this->_body.c_str(), this->_body.size()).append("\r\n");
			this->_body = this->_body.substr(this->_body.size());
		}
		return (chunked_body);
	}
	else
		return (this->_body);
}

std::string	TmpResponse::get_header(void) const
{
	return (this->_header.get_response_header());
}

bool	TmpResponse::get_is_complete_response(void) const
{
	return (this->_is_complete_response);
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

	outfile.open(path, std::ios::binary | std::ios::out);
#if DEBUG
	this->_logger.debug("PUT path: " + path);
#endif
	if (!outfile.good())
		throw ServerErrorException(__LINE__, __FILE__, E500, "Bad PUT request");
	if (this->_req.get_request_field(TRANSFER_ENCODING) == "chunked")
	{
		std::ifstream	infile;
		std::string		buf;
		size_t			size;

		infile.open(this->_req.get_unchunked_filename(), std::ios::binary);
		if (!infile.good())
		{
			outfile.close();
			throw ServerErrorException(__LINE__, __FILE__, E500, "Invalid unchunked file");
		}
		infile.seekg(0, infile.end);
		size = infile.tellg();
		infile.seekg(0, infile.beg);
		buf.resize(size, '\0');
		infile.read(const_cast<char*>(buf.data()), size);
		infile.close();
		if (std::remove(this->_req.get_unchunked_filename().c_str()) != 0)
			this->_logger.warn(this->_req.get_unchunked_filename() + " unchunked file remove failed");
		outfile.write(buf.data(), size);
	}
	else
		outfile.write(this->_req.get_body().c_str(), this->_req.get_body().size());
	outfile.close();

	this->_header.set_status(S200);
	this->_header.set_location("http://" + this->_req.get_request_field(SERVER_NAME) + ":" + this->_req.get_request_field(PORT) + "/" + path);
	this->_header.construct();
}

void	TmpResponse::handle_cgi(const std::string& cgi_msg)
{
	this->_body = cgi_msg.substr(cgi_msg.find("\r\n\r\n") + 4);
	this->_header.set_content_length(this->_body.size());
	this->_header.construct(cgi_msg.substr(0, cgi_msg.find("\r\n\r\n") + 4));
	std::cerr << "this->_body.size(): " << this->_body.size() << std::endl;
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
	if (std::remove(path.c_str()) != 0)
		throw ServerErrorException(__LINE__, __FILE__, E500, "std::remove failed");
	this->_header.set_status(S200);
	this->_header.construct();
}

static std::string	read_file(const std::string& path)
{
	std::ifstream		infile;
	std::stringstream	ss;
	std::string			s;

	infile.open(path, std::ios::binary);
	ss << infile.rdbuf();
	s = ss.str();
#if DEBUG
	Logger().debug("Serving file size: " + std::to_string(s.size()));
#endif
	infile.close();
	return (s);
}
void	TmpResponse::handle_normal(const std::string& path)
{
	this->_body = read_file(path);

	this->_header.set_status(S200);
	this->_header.set_content_length(this->_body.size());
	this->_header.construct();
	if (this->_req.get_request_field(METHOD) == "HEAD")
		this->_body.clear();
}

static	std::string	default_error_page(void)
{
	std::stringstream	ss;
	std::string			s;

	ss << "<!DOCTYPE html>";
	ss << "<html>";
	ss << "<h1>Error</h1>";
	ss << "</html>";
	return (ss.str());
}

void	TmpResponse::handle_error(enum StatusCode status, const ServerConfig& sconfig)
{
	const utils::StrToStrVecMap&	s_directives = sconfig.get_directives();
	std::string						error_file;

	if (s_directives.count("error_page") > 0)
	{
		const utils::StrVec&			val = s_directives.at("error_page");
		utils::StrVec::const_iterator	it;

		it = std::find(val.begin(), val.end(), utils::itoa(status));
		if (it != val.end())
			error_file = *(it + 1);
	}

	if (!error_file.empty())
	{
		std::ifstream	infile;

		infile.open(error_file);
		if (!infile.good())
		{
			this->_body = default_error_page();
#if DEBUG
			this->_logger.debug("Serving default error");
#endif
		}
		else
		{
			this->_body = read_file(error_file);
#if DEBUG
			this->_logger.debug("Serving " + error_file);
#endif
		}
		infile.close();
	}
	else
		this->_body = default_error_page();
}
