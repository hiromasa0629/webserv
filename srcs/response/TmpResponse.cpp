/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TmpResponse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/07 23:21:21 by hyap              #+#    #+#             */
/*   Updated: 2023/03/15 14:40:12 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TmpResponse.hpp"

static	size_t	get_filesize(std::ifstream*	infile, size_t offset)
{
	size_t	filesize;

	infile->seekg(0, infile->end);
	filesize = infile->tellg();
	filesize -= offset;
	infile->seekg(offset, infile->beg);
	return (filesize);
}

TmpResponse::TmpResponse(void) : _response_infile(NULL, 0) {}

TmpResponse::~TmpResponse(void) {}

TmpResponse::TmpResponse(enum StatusCode status, const ServerConfig& sconfig)
	: _is_complete_response(true), _response_infile(NULL, 0)
{
	this->handle_error(status, sconfig);
}

TmpResponse::TmpResponse(const TmpRequest& req, const ServerConfig& sconfig, char** envp)
	: _req(req), _is_complete_response(true), _response_config(this->_req, sconfig, envp), _response_infile(NULL, 0)
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
		this->handle_autoindex(this->_response_config.get_autoindex().second.get_autoindex_filename());
	}
	else if (this->_response_config.get_cgi().first)
	{
		//	Is CGI
		this->handle_cgi(this->_response_config.get_cgi().second.get_cgi_filename());
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

	if (this->_response_infile.second > RESPONSE_BUFFER && this->_req.get_request_field(METHOD) != "HEAD")
	{
#if DEBUG
		this->_logger.debug("Sending size " + utils::itoa(this->_response_infile.second) + " in chunked response...");
#endif
		this->_is_complete_response = false;

		if (!this->_response_config.get_cgi().first)
		{
			this->_header.set_is_chunked(true);
			this->_header.set_status(S200);
			this->_header.set_content_length(0);
			this->_header.construct();
		}
	}
}

std::string	TmpResponse::get_body(void)
{
	if (!this->_is_complete_response)
	{
		std::string			s;
		size_t				current;

		current = this->_response_infile.first->tellg();
		if (current == this->_response_infile.second)
		{
			this->_chunked_body.append("0\r\n\r\n", 5);
			this->_is_complete_response = true;
			if (this->_response_infile.first != NULL)
			{
				this->_response_infile.first->close();
				delete this->_response_infile.first;
				this->_response_infile.first = NULL;
			}

			if (this->_response_config.get_cgi().first)
			{
				std::string filename = this->_response_config.get_cgi().second.get_cgi_filename();
				filename = filename.substr(0, filename.find_first_of('.')) + ".cgi_body";
				if (std::remove(filename.c_str()) != 0)
					this->_logger.warn(filename + " failed to remove");
			}
			return (this->_chunked_body);
		}
		if ((this->_response_infile.second - current) > RESPONSE_BUFFER)
		{
			this->_chunked_body.resize(RESPONSE_BUFFER);
			this->_response_infile.first->read(const_cast<char*>(this->_chunked_body.c_str()), RESPONSE_BUFFER);
			this->_chunked_body.append("\r\n");
			s.append(utils::to_hex(RESPONSE_BUFFER)).append("\r\n");
			this->_chunked_body.insert(0, s);
		}
		else
		{
			size_t	remaining_filesize = get_filesize(this->_response_infile.first, current);

			this->_chunked_body.resize(remaining_filesize);
			this->_response_infile.first->read(const_cast<char*>(this->_chunked_body.c_str()), remaining_filesize);
			this->_chunked_body.append("\r\n");
			s.append(utils::to_hex(remaining_filesize)).append("\r\n");
			this->_chunked_body.insert(0, s);
		}
		return (this->_chunked_body);
	}
	else if (this->_req.get_request_field(METHOD) == "HEAD")
	{
		return ("");
	}
	else
	{
		std::string	s;

		if (this->_response_infile.first != NULL)
		{
			s.resize(this->_response_infile.second);
			this->_response_infile.first->read(const_cast<char*>(s.c_str()), this->_response_infile.second);
			this->_response_infile.first->close();
			delete this->_response_infile.first;
			this->_response_infile.first = NULL;

			if (this->_response_config.get_cgi().first)
			{
				std::string filename = this->_response_config.get_cgi().second.get_cgi_filename();
				filename = filename.substr(0, filename.find_first_of('.')) + ".cgi_body";
				if (std::remove(filename.c_str()) != 0)
					this->_logger.warn(filename + " failed to remove");
			}

			if (this->_response_config.get_autoindex().first)
			{
				std::string filename = this->_response_config.get_autoindex().second.get_autoindex_filename();
				if (std::remove(filename.c_str()) != 0)
					this->_logger.warn(filename + " failed to remove");
			}
		}
		return (s);
	}
}

void	TmpResponse::truncate_chunk_body(int sent)
{
	this->_chunked_body.erase(0, sent);
}

const std::string&	TmpResponse::get_chunked_body(void) const
{
	return (this->_chunked_body);
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

	outfile.open(path.c_str(), std::ios::binary | std::ios::out);
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

		infile.open(this->_req.get_unchunked_filename().c_str(), std::ios::binary);
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

void	TmpResponse::handle_cgi(const std::string& cgi_filename)
{
	char			c;
	std::string		header;
	size_t			filesize;

	std::ifstream	infile(cgi_filename.c_str(), std::ios::binary);
	if (!infile.good())
		throw ServerErrorException(__LINE__, __FILE__, E500, "handle_cgi infile failed");
	while (infile.get(c))
	{
		header += c;
		if (header.length() > 3 && header.substr(header.length() - 4) == "\r\n\r\n")
			break ;
	}

	filesize = get_filesize(&infile, header.size());
	this->_response_infile.second = filesize;
	this->_header.set_content_length(filesize);

	if (this->_response_infile.second > RESPONSE_BUFFER)
		this->_header.set_is_chunked(true);
	this->_header.construct(header);

	std::string		cgi_body_filname = cgi_filename.substr(0, cgi_filename.find_first_of('.')) + ".cgi_body";
	std::ofstream	outfile(cgi_body_filname.c_str(), std::ios::out | std::ios::binary);
	if (!outfile.good())
		throw ServerErrorException(__LINE__, __FILE__, E500, "handle_cgi outfile failed");

	std::copy(std::istreambuf_iterator<char>(infile),
				std::istreambuf_iterator<char>(),
				std::ostreambuf_iterator<char>(outfile));

	infile.close();
	outfile.close();

	if (std::remove(cgi_filename.c_str()) != 0)
		this->_logger.warn(cgi_filename + " failed to remove");

	this->_response_infile.first = new std::ifstream();
	this->_response_infile.first->open(cgi_body_filname.c_str(), std::ios::binary);
	if (!this->_response_infile.first->good())
		throw ServerErrorException(__LINE__, __FILE__, E500, "handle_cgi response_infile failed");
}

void	TmpResponse::handle_autoindex(const std::string& autoindex_filename)
{
	this->_response_infile.first = new std::ifstream();
	this->_response_infile.first->open(autoindex_filename, std::ios::binary);
	if (!this->_response_infile.first->good())
		throw ServerErrorException(__LINE__, __FILE__, E500, "handle_autoindex infile failed");
	this->_response_infile.second = get_filesize(this->_response_infile.first, 0);

	this->_header.set_status(S200);
	this->_header.set_content_length(this->_response_infile.second);
	this->_header.construct();
}

void	TmpResponse::handle_delete(const std::string& path)
{
	if (std::remove(path.c_str()) != 0)
		throw ServerErrorException(__LINE__, __FILE__, E500, "std::remove failed");
	this->_header.set_status(S200);
	this->_header.construct();
}

void	TmpResponse::handle_normal(const std::string& path)
{
	this->_response_infile.first = new std::ifstream();
	this->_response_infile.first->open(path.c_str(), std::ios::binary);
	if (!this->_response_infile.first->good())
		throw ServerErrorException(__LINE__, __FILE__, E500, "handle_normal infile failed");
	this->_response_infile.second = get_filesize(this->_response_infile.first, 0);

	this->_header.set_status(S200);
	this->_header.set_content_length(this->_response_infile.second);
	this->_header.construct();
}

static	std::string	default_error_page(void)
{
	std::string		filename = "default_error.html";
	struct stat		st;

	if (stat(filename.c_str(), &st) != 0)
	{
		std::ofstream	outfile(filename, std::ios::out | std::ios::binary);
		if (!outfile.good())
			throw ServerErrorException(__LINE__, __FILE__, E500, "default error_page outfile failed");

		std::stringstream	ss;

		ss << "<!DOCTYPE html>";
		ss << "<html>";
		ss << "<h1>Error</h1>";
		ss << "</html>";

		outfile.write(ss.str().c_str(), ss.str().size());
		outfile.close();
	}
	return (filename);
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

	this->_response_infile.first = new std::ifstream();

	if (!error_file.empty())
	{
		this->_response_infile.first->open(error_file.c_str(), std::ios::binary);
		if (!this->_response_infile.first->good())
			this->_response_infile.first->open(default_error_page().c_str(), std::ios::binary);
	}
	else
		this->_response_infile.first->open(default_error_page().c_str(), std::ios::binary);
	this->_response_infile.second = get_filesize(this->_response_infile.first, 0);

	this->_header.set_status(status);
	this->_header.set_content_length(this->_response_infile.second);
	this->_header.construct();
}
