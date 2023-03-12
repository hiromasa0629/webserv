/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TmpRequest.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/03 17:56:35 by hyap              #+#    #+#             */
/*   Updated: 2023/03/12 15:37:21 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TmpRequest.hpp"

const char*	TmpRequest::_methods[] = {"GET", "POST", "DELETE", "HEAD", "PUT"};

const char*	TmpRequest::_fields_string[] = {"method", "uri", "query", "server_name", "port", "protocol", "content-length", "content-type", "transfer-encoding", "boundary", "X-Secret-Header"};

TmpRequest::TmpRequest(void)
	: _is_complete(false), _is_complete_header(false), _req(""), _status_code(S200), _logger(), _chunked_debug_size(0), _debugged_index(0) {}

TmpRequest::~TmpRequest(void) {}

TmpRequest::TmpRequest(enum StatusCode status) : _status_code(status) {}

std::string	TmpRequest::get_request_field(enum RequestFields name) const
{
	if (this->_header_info.count(name) == 0)
		return ("");
	return (this->_header_info.at(name));
}

enum StatusCode	TmpRequest::get_status_code(void) const
{
	return (this->_status_code);
}

std::string	TmpRequest::get_body(void) const
{
	return (this->_body);
}

bool	TmpRequest::get_is_complete(void) const
{
	return (this->_is_complete);
}

std::string	TmpRequest::get_unchunked_filename(void) const
{
	return (this->_unchunked_filename);
}

void	TmpRequest::append(const std::string& req)
{
	this->_req.append(req);
#if DEBUG
	// utils::print_msg_with_crlf(this->_req);
#endif
	if (!_is_complete_header)
	{
		if (!this->check_header_complete())
			return ;
		this->extract_header_info();
		this->_is_complete_header = true;
	}
	if (this->_header_info[METHOD] == "POST" || this->_header_info[METHOD] == "PUT")
		this->handle_post(req);
	else
		this->handle_get();
}

void	TmpRequest::print_request_header(void) const
{
	std::stringstream	ss;
	// if (!this->_is_complete)
	// 	throw std::runtime_error("Request is not completely read yet");
#if DEBUG
	std::map<enum RequestFields, std::string>::const_iterator	it;

	it = this->_header_info.begin();
	for (; it != this->_header_info.end(); it++)
		this->_logger.debug(_fields_string[it->first] + std::string(": ") + it->second);
	if (this->_query.size() > 0)
	{
		this->_logger.debug(" --------------------- ");
		std::map<std::string, std::string>::const_iterator	it2;

		it2 = this->_query.begin();
		for (; it2 != this->_query.end(); it2++)
			this->_logger.debug(it2->first + std::string(": ") + it2->second);
	}
	// if (!this->_body.empty())
	// {
	// 	this->_logger.debug(" ----------- Body ----------- ");
	// 	utils::print_msg_with_crlf(this->_body);
	// }
#else
	this->_logger.info(this->_method + " " + this->_uri + " " + this->_host + ":" + this->_port);
#endif
}

bool	TmpRequest::check_header_complete(void)
{
	if (this->_req.length() < 4)
		throw ServerErrorException(__LINE__, __FILE__, E400, "Invalid request length");
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


	header = this->_req.substr(0, this->_req.find("\r\n\r\n"));
#if DEBUG
	// std::cout << header << std::endl;
#endif
	while ((index = header.find("\r\n")) != std::string::npos)
	{
		header_lines.push_back(header.substr(0, index));
		header = header.substr(index + 2);
	}
	header_lines.push_back(header);
	split = utils::ft_split(header_lines[0]);
	if (!this->check_request_line(split))
		throw ServerErrorException(__LINE__, __FILE__, E400, "Request line error");
	this->_header_info.insert(std::make_pair(METHOD, split[0]));
	if (split[1].find_first_of('?') != std::string::npos)
	{
		this->_header_info.insert(std::make_pair(URI, split[1].substr(0, split[1].find_first_of('?'))));
		this->_header_info.insert(std::make_pair(QUERY, split[1].substr(split[1].find_first_of('?') + 1)));
	}
	else
		this->_header_info.insert(std::make_pair(URI, split[1]));
	this->_header_info.insert(std::make_pair(PROTOCOL, split[2]));
	for (size_t i = 1; i < header_lines.size(); i++)
	{
		// this->_logger.warn(header_lines[i]);
		split = utils::ft_split(header_lines[i]);
		if (split.front() == "Host:")
		{
			this->_header_info.insert(std::make_pair(SERVER_NAME, split[1].substr(0, split[1].find_first_of(':'))));
			this->_header_info.insert(std::make_pair(PORT, split[1].substr(split[1].find_first_of(':') + 1)));
		}
		else if (split.front() == "Content-Type:")
			this->handle_content_type(utils::StrVec(split.begin() + 1, split.end()));
		else if (split.front() == "Transfer-Encoding:")
			this->_header_info.insert(std::make_pair(TRANSFER_ENCODING, split.back()));
		else if (split.front() == "Content-Length:")
			this->_header_info.insert(std::make_pair(CONTENT_LENGTH, split.back()));
		else if (split.front() == "X-Secret-Header-For-Test:")
			this->_header_info.insert(std::make_pair(X_SECRET, split.back()));
	}
	if (this->_header_info.count(SERVER_NAME) == 0)
		throw ServerErrorException(__LINE__, __FILE__, E400, "Missing request host");
}

bool	TmpRequest::check_request_line(const utils::StrVec& vec) const
{
	int	i;

	if (vec.empty())
		return (false);
	if (vec.size() != 3)
		return (false);
	i = 0;
	while (i < 5)
	{
		if (_methods[i] == vec.front())
			break ;
		i++;
	}
	if (i == 5)
		return (false);
	if (vec.back() != "HTTP/1.1")
		return (false);
	if (vec[1][0] != '/')
		return (false);
	return (true);
}

void	TmpRequest::handle_content_type(const utils::StrVec& val)
{
	std::string	type;

	type = val.front();
	if (type[type.length() - 1] == ';')
		type.erase(type.end() - 1);
	this->_header_info.insert(std::make_pair(CONTENT_TYPE, type));
	if (type == "mutlipart/form-data")
	{
		if (val.size() != 2)
			throw ServerErrorException(__LINE__, __FILE__, E400, "Invalid form-data");
		else
			this->_header_info.insert(std::make_pair(BOUNDARY, val.back().substr(val.back().find_first_of('=') + 1)));
	}
}

void	TmpRequest::handle_get(void)
{
	std::string	uri;

	uri = this->_header_info[URI];
	if (uri.find_first_of('?') != std::string::npos)
	{
		std::string	query;
		size_t		index;

		query = uri.substr(uri.find_first_of('?') + 1);
		if (query.length() > 0)
		{
			std::string	single;
			size_t		index_two;
			while (query.length())
			{
				index = query.find_first_of('&');
				single = query.substr(0, index);
				index_two = single.find_first_of('=');
				this->_query.insert(std::make_pair(single.substr(0, index_two), single.substr(index_two + 1)));
				if (index == std::string::npos)
					query.clear();
				else
					query = query.substr(index + 1);
			}
			this->_query.erase("");
		}
	}
	this->_is_complete = true;
}

void	TmpRequest::handle_post(const std::string& req)
{
	if (this->_header_info.count(TRANSFER_ENCODING) > 0 && this->_header_info[TRANSFER_ENCODING] == "chunked")
	{

		if (this->_chunked_filename.empty())
		{
			this->_logger.debug("Receving chunked request...");

			std::string	chunked_body;

			chunked_body = this->_req.substr(this->_req.find("\r\n\r\n") + 4);
			this->_chunked_outfile = new std::ofstream();
			this->_chunked_filename = utils::itoa(std::time(NULL)) + ".chunked";
			this->_chunked_outfile->open(this->_chunked_filename.c_str(), std::ios::app | std::ios::out | std::ios::binary);
			if (!this->_chunked_outfile->good())
				throw ServerErrorException(__LINE__, __FILE__, E500, "Chunked body outfile error");
			this->_chunked_infile = new std::ifstream();
			this->_chunked_infile->open(this->_chunked_filename.c_str(), std::ios::binary);
			if (!this->_chunked_infile->good())
				throw ServerErrorException(__LINE__, __FILE__, E500, "Chunked body infile error");
			this->_chunked_outfile->write(chunked_body.c_str(), chunked_body.size());
			this->_chunked_outfile->flush();
		}
		else //if (end_of_chunk != "0\r\n\r\n")
		{
			this->_chunked_outfile->write(req.c_str(), req.size());
			this->_chunked_outfile->flush();
		}


#if DEBUG
		this->_chunked_debug_size += req.size();
		if (this->_chunked_debug_size >= (CHUNKED_DEBUG * this->_debugged_index))
		{
			this->_debugged_index++;
			this->_logger.debug("Received " + utils::itoa(this->_chunked_debug_size));
		}
#endif
		std::string	end_of_chunk;

		this->_chunked_infile->clear();
		this->_chunked_infile->seekg(-5, std::ios::end);
		char	buf[6];
		std::memset(buf, 0, 6);
		this->_chunked_infile->read(buf, 5);

		if (std::strcmp(buf, "0\r\n\r\n") == 0)
		{
			this->_is_complete = true;
			this->_chunked_outfile->close();
			this->_chunked_infile->close();
			delete this->_chunked_outfile;
			delete this->_chunked_infile;
			this->_unchunked_filename = this->tidy_up_chunked_body();
		}
	}
	else if (this->_header_info.count(CONTENT_LENGTH) > 0)
	{
		size_t	content_length;

		content_length = std::atoi(this->_header_info[CONTENT_LENGTH].c_str());
		if (this->_body.empty())
		{
			std::string body;

			body = this->_req.substr(this->_req.find("\r\n\r\n") + 4);
			if (body.size() > content_length)
				this->_body.append(body.c_str(), content_length);
			else
				this->_body.append(body);
		}
		else
		{
			size_t	remaining;

			remaining = content_length - this->_body.size();
			if (req.size() > remaining)
				this->_body.append(req, remaining);
			else
				this->_body.append(req);
		}
		if (this->_body.size() == content_length)
			this->_is_complete = true;
	}
}

std::string	TmpRequest::tidy_up_chunked_body(void)
{
	std::string		unchunked_filename;
	std::string		body;

	{
		std::ifstream	infile;
		size_t			size;

		infile.open(this->_chunked_filename.c_str(), std::ios::binary);
		if (!infile.good())
			throw ServerErrorException(__LINE__, __FILE__, E500, "Unchunked infile failed");
		infile.seekg(0, infile.end);
		size = infile.tellg();
		infile.seekg(0, infile.beg);
		body.resize(size, '\0');
		infile.read(const_cast<char*>(body.data()), size);
		infile.close();
	}

	{
		std::ofstream	outfile;
		size_t			debug_size = 0;

		unchunked_filename = this->_chunked_filename.substr(0, this->_chunked_filename.find_first_of('.')) + ".unchunked";
		outfile.open(unchunked_filename.c_str(), std::ios::out | std::ios::binary);
		if (!outfile.good())
			throw ServerErrorException(__LINE__, __FILE__, E500, "Unchunked outfile failed");
		for (size_t i = 0; i < body.size();)
		{
			size_t 	j;
			size_t	size;

			j = i;
			while (body[j] != '\r' && body[j + 1] != '\n')
				j++;
			size = utils::to_int(body.substr(i, j - i));
			debug_size += size;
			j += 2;
			outfile.write(&body[j], size);
			while (body[j] != '\r' && body[j+ 1] != '\n')
				j++;
			i = j + 2;
		}
		// while ((index = body.find("\r\n")) != std::string::npos)
		// {
		// 	std::string	tmp;

		// 	size = utils::to_int(body.substr(0, index));
		// 	debug_size += size;
		// 	body = body.substr(index + 2);
		// 	if ((index = body.find("\r\n")) == std::string::npos)
		// 		break ;
		// 	tmp = body.substr(0, index);
		// 	outfile.write(body.data(), size);
		// 	body = body.substr(index + 2);
		// }
		outfile.close();
		#if DEBUG
			this->_logger.debug("Received size " + utils::itoa(debug_size) + " from chunked request");
			this->_logger.debug("Wrote to " + this->_unchunked_filename);
		#endif
	}

	if (std::remove(this->_chunked_filename.c_str()) != 0)
		this->_logger.warn(this->_chunked_filename + " Chunked file remove failed");

	return (unchunked_filename);
}

