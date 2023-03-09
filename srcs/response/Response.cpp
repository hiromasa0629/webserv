// /* ************************************************************************** */
// /*                                                                            */
// /*                                                        :::      ::::::::   */
// /*   Response.cpp                                       :+:      :+:    :+:   */
// /*                                                    +:+ +:+         +:+     */
// /*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
// /*                                                +#+#+#+#+#+   +#+           */
// /*   Created: 2023/02/17 15:03:20 by hyap              #+#    #+#             */
// /*   Updated: 2023/03/07 23:18:18 by hyap             ###   ########.fr       */
// /*                                                                            */
// /* ************************************************************************** */

// #include "Response.hpp"

// Response::Response(void) {}

// Response::~Response(void) {}

// Response::Response(const TmpRequest& request, const ServerConfig& sconfig, char** envp) : _is_complete_response(true), _request(request), _is_autoindex(false), _is_redirection(false), _is_upload(false)
// {
// 	this->_uri					= this->_request.get_request_field(URI);
// 	this->_host					= this->_request.get_request_field(SERVER_NAME);
// 	this->_port					= this->_request.get_request_field(PORT);
// 	this->_request_body			= this->_request.get_body();
// 	this->_request_body_size	= this->_request_body.size();
// 	this->_method				= this->_request.get_request_field(METHOD);
// 	this->_envp					= envp;

// 	this->set_directives(sconfig);

// 	if (this->_directives.find("return") != this->_directives.end())
// 		this->_is_redirection = true;
// 	if (this->_directives.find("autoindex") != this->_directives.end() && this->_directives.find("autoindex")->second[0] == "on")
// 		this->_is_autoindex = true;
// 	if (this->_directives.find("upload") != this->_directives.end() && (this->_method == "POST" || this->_method == "PUT"))
// 		this->_is_upload = true;

// 	if (!this->_is_autoindex && !this->_is_upload)
// 		this->set_path();

// 	this->has_handled_error();

// 	// if (this->has_handled_error())
// 	// 	this->handle_error();
// 	if (this->is_cgi())
// 		this->handle_cgi();
// 	else if (this->_is_redirection)
// 		this->handle_redirect();
// 	else if (this->_is_autoindex)
// 		this->handle_autoindex();
// 	else if (this->_is_upload)
// 		this->handle_upload();
// 	else
// 		this->handle_normal();

// 	if (this->_response_body.size() > MSG_BUFFER)
// 	{
// 		this->_is_complete_response = false;

// 		this->_header.set_is_chunked(true);
// 		this->_header.set_status(S200);
// 		this->_header.set_content_length(0);
// 		this->_header.set_content_type("image/jpg");
// 		this->_header.construct();
// 	}


// 	std::stringstream	ss;

// 	ss << this->_header.get_status() << " " << this->_path;
// 	this->_logger.info(ss.str());
// }

// Response::Response(enum StatusCode status, const ServerConfig& sconfig) : _is_complete_response(true)
// {
// 	utils::StrVec	vec;
// 	// std::string		root;

// 	this->set_directives(sconfig);
// 	this->_header.set_status(status);
// 	// this->handle_error();

// 	if (this->_directives.count("error_page") > 0)
// 	{
// 		vec = this->_directives.at("error_page");
// 		this->_path.clear();
// 		for (size_t i = 0; i < vec.size(); i++)
// 		{
// 			if (std::atoi(vec[i++].c_str()) == this->_header.get_status())
// 			{
// 				this->_path = vec[i];
// 				break ;
// 			}
// 		}
// 		this->read_path();
// 	}
// 	this->_header.set_location("");
// 	this->_header.set_content_length(this->_response_body.size());
// 	this->_header.construct();

// 	// this->construct();
// }

// void	Response::handle_cgi(void)
// {
// 	utils::StrVec	cgis;
// 	ResponseCgi		rcgi;

// 	cgis = this->_directives.at("cgi");
// 	for (size_t i = 0; i < cgis.size(); i++)
// 		if (this->_path.substr(this->_path.length() - 3, this->_path.length()) == cgis[i++])
// 			rcgi = ResponseCgi(this->_path, this->_request_body, cgis[i], this->_envp);
// }

// void	Response::handle_upload(void)
// {
// 	ResponseForm								form;
// 	std::vector<ResponseFormField>				rff;
// 	std::vector<ResponseFormField>::iterator	it;
// 	std::ofstream								outfile;
// 	std::string									tmp_path;

// 	if (this->_method == "POST")
// 	{
// 		form = ResponseForm(this->_request_body, this->_request.get_request_field(BOUNDARY));
// 		this->_path.clear();
// 		this->_path.append(this->_directives["upload"][0]);
// 		rff = form.get_fields();
// 		it = rff.begin();
// 		for (; it < rff.end(); it++)
// 		{
// 			if (!it->get_is_file())
// 				continue ;
// 			tmp_path = this->_path + "/" + it->get_filename();
// 			outfile.open(tmp_path, std::ofstream::out);
// 			outfile.write(it->get_value().c_str(), it->get_value().size());
// 			outfile.close();
// 		}
// 		this->_header.set_status(S200);
// 		this->_header.set_content_length(0);
// 		this->_header.set_location("");
// 		this->_header.construct();
// 	}
// 	else if (this->_method == "PUT")
// 	{
// 		this->_path.clear();
// 		this->_path.append(this->_directives["upload"][0]);
// 		outfile.open(this->_path.append("/").append(this->_uri.substr(this->_uri.find_last_of('/') + 1)), std::ofstream::out);
// 		outfile.write(this->_request_body.c_str(), this->_request_body_size);
// 		outfile.close();

// 		this->_header.set_status(S200);
// 		this->_header.set_content_length(0);
// 		this->_header.set_location("http://" + this->_host + ":" + this->_port + "/" + this->_path);
// 		this->_header.construct();
// 	}


// }

// void	Response::handle_normal(void)
// {
// 	std::ifstream	infile;
// 	std::string		line;
// 	this->_header.set_status(S200);

// 	this->read_path();

// 	this->_header.set_content_length(this->_response_body.size());
// 	this->_header.set_location("");
// 	this->_header.construct();
// }

// void	Response::handle_autoindex(void)
// {
// 	std::ifstream	infile;
// 	DIR*			dir;

// 	this->_path.clear();
// 	this->_path.append(this->_directives["root"][0]);
// 	this->remove_trailing_slash(this->_path);
// 	if (this->_directives.find("index") != this->_directives.end())	// If has index
// 	{
// 		infile.open(this->_path + "/" + this->_directives["index"][0]);
// 		if (infile.good())
// 		{
// 			this->_path.append("/").append(this->_directives["index"][0]);
// 			this->handle_normal();
// 			return ;
// 		}
// 	}
// 	if (!this->_rest_of_the_uri.empty())	// if /autoindex/file.html
// 	{
// 		infile.open(this->_path + this->_rest_of_the_uri);
// 		dir = opendir(std::string().append(this->_path).append(this->_rest_of_the_uri).c_str());
// 		if (infile.good() && !dir)
// 		{
// 			this->_path.append(this->_rest_of_the_uri);
// 			this->handle_normal();
// 			return ;
// 		}
// 		else if (!dir)
// 		{
// 			// this->_header.set_status(404);
// 			// this->handle_error();
// 			throw ServerErrorException(__LINE__, __FILE__, E404, "Not found");
// 			// return ;
// 		}
// 	}
// 	this->_autoindex = ResponseAutoindex(this->_path, this->_rest_of_the_uri, this->_host, this->_port, this->_location_uri);
// 	this->_response_body = this->_autoindex.get_body();

// 	this->_header.set_content_length(this->_response_body.size());
// 	this->_header.set_location("");
// 	this->_header.set_status(S200);
// 	this->_header.construct();
// }

// void	Response::handle_redirect(void)
// {
// 	std::stringstream	ss;
// 	std::string			redirect;

// 	redirect = this->_directives["return"][0];
// 	ss << "http://";
// 	ss << this->_host << ":" << this->_port;
// 	if (redirect.front() != '/')
// 		redirect.insert(redirect.begin(), '/');
// 	ss << redirect;

// 	this->_header.set_content_length(0);
// 	this->_header.set_location(ss.str());
// 	this->_header.set_status(S301);
// 	this->_header.construct();
// }

// void	Response::has_handled_error(void)
// {
// 	utils::StrToStrVecMap::iterator	it;

// 	// Handle invalid request
// 	if (this->_request.get_status_code() != S200)
// 		throw ServerErrorException(__LINE__, __FILE__, this->_request.get_status_code(), "Invalid Request");
// 	// Check server_name
// 	it = this->_directives.find("server_name");
// 	if (it != this->_directives.end())
// 	{
// 		utils::StrVec	sv;

// 		sv = it->second;
// 		if (!std::count(sv.begin(), sv.end(), "_") && !std::count(sv.begin(), sv.end(), this->_host) && !this->is_localhost())
// 			throw ServerErrorException(__LINE__, __FILE__, E404, "Invalid server_name");
// 	}
// 	// check limit_except
// 	it = this->_directives.find("limit_except");
// 	if (it != this->_directives.end() && !std::count(it->second.begin(), it->second.end(), this->_method))
// 		throw ServerErrorException(__LINE__, __FILE__, E405, "Method not allowed");
// 	// Body size
// 	it = this->_directives.find("client_max_body_size");
// 	if (it != this->_directives.end() && std::atoi(it->second[0].c_str()) < this->_request_body_size)
// 		throw ServerErrorException(__LINE__, __FILE__, E413, "Body size too large");
// 	// try open path if not (autoindex and redirection)
// 	// if (!this->_is_autoindex && !this->_is_redirection && this->_method == "GET")
// 	// {
// 	// 	std::ifstream					infile;

// 	// 	infile.open(this->_path);
// 	// 	if (!infile.good())
// 	// 	{
// 	// 		infile.open(this->_path + ".html");
// 	// 		if (!infile.good())
// 	// 			throw ServerErrorException(__LINE__, __FILE__, E404, "Path not found " + this->_path);
// 	// 	}
// 	// }
// #if DEBUG
// 	this->_logger.debug("Passed error handle");
// #endif
// }

// // Set server and location directives and combine them
// void	Response::set_directives(const ServerConfig& sconfig)
// {
// 	std::string										location;
// 	size_t											second_slash_index;
// 	ServerConfig::StrToLConfigMap::const_iterator	it;
// 	utils::StrToStrVecMap::const_iterator			it2;

// 	this->_s_directives = sconfig.get_directives();
// 	this->_directives = this->_s_directives;
// 	second_slash_index = std::string(this->_uri.begin() + 1, this->_uri.end()).find_first_of('/');
// 	if (second_slash_index != std::string::npos)
// 		location = this->_uri.substr(0, second_slash_index + 1);
// 	else
// 		location = this->_uri;
// 	if ((it = sconfig.get_lconfig().find(location)) != sconfig.get_lconfig().end())
// 		this->_l_directives = it->second.get_directives();
// 	it2 = this->_l_directives.begin();
// 	for (; it2 != this->_l_directives.end(); it2++)
// 		this->_directives[it2->first] = it2->second;
// 	if (second_slash_index != std::string::npos)
// 		this->_rest_of_the_uri = this->_uri.substr(second_slash_index + 1, this->_uri.length());
// 	this->_location_uri = location;
// }

// void	Response::set_path(void)
// {
// 	std::string	path;

// 	if (!this->_l_directives.size()) // No matching location block
// 	{
// 		path.append(this->_directives.at("root")[0]);
// 		this->remove_trailing_slash(path);
// 		path.append(this->_uri);
// 		this->_path = path;
// 	}
// 	else // Has matching location block
// 	{
// 		// std::cout << "root: " << this->_directives.find("root")->second[0] << std::endl;
// 		path.append(this->_directives.at("root")[0]);
// 		this->remove_trailing_slash(path);
// 		if (this->_l_directives.count("root") == 0)
// 			path.append(this->_location_uri);
// 		this->remove_trailing_slash(path);
// 		// if has more uri
// 		if (!this->_rest_of_the_uri.empty())
// 			path.append(this->_rest_of_the_uri);
// 		else
// 		{
// 			if (this->_directives.count("index") > 0)
// 				path.append("/").append(this->_directives.find("index")->second[0]);
// 		}
// 	}
// 	this->_path = path;

// 	if (this->_method == "PUT")
// 		return ;

// 	std::ifstream	infile;

// 	infile.open(this->_path);
// 	// std::cout << "this->_path: " << this->_path << std::endl;
// 	if (!infile.good())
// 	{
// 		infile.open(this->_path + ".html");
// 		if (!infile.good())
// 			throw ServerErrorException(__LINE__, __FILE__, E404, this->_path + " read_path failed");
// 		else
// 			this->_path.append(".html");
// 	}
// 	else if (opendir(this->_path.c_str()) != NULL)
// 	{
// 		if (this->_directives.count("index") == 0)
// 			throw ServerErrorException(__LINE__, __FILE__, E403, this->_path + " read_path is a directory");
// 		infile.close();
// 		infile.open(this->_path.append("/").append(this->_directives["index"][0]));
// 		if (!infile.good())
// 			throw ServerErrorException(__LINE__, __FILE__, E404, this->_path + " read_path failed");
// 		else
// 			this->_path.append("/").append(this->_directives["index"][0]);
// 	}
// }

// bool	Response::is_cgi(void)
// {
// 	if (this->_path.length() < 3)
// 		return (false);
// 	if (this->_directives.count("cgi") == 0)
// 		return (false);

// 	utils::StrVec	cgis;

// 	cgis = this->_directives["cgi"];
// 	for (size_t i = 0; i < cgis.size(); i++)
// 		if (this->_path.substr(this->_path.length() - 3, this->_path.length()) == cgis[i++])
// 			return (true);
// 	return (false);
// }

// std::string	Response::get_response_header(void) const
// {
// 	return (this->_header.get_response_header());
// }

// std::string	Response::get_body(void)
// {
// 	if (!this->_is_complete_response)
// 	{
// 		std::string	chunked_body;

// 		if  (this->_response_body.empty())
// 		{
// 			chunked_body.append("0\r\n\r\n");
// 			this->_is_complete_response = true;
// 			return (chunked_body);
// 		}
// 		if (this->_response_body.size() > MSG_BUFFER)
// 		{
// 			chunked_body.append(utils::to_hex(MSG_BUFFER)).append("\r\n");
// 			chunked_body.append(this->_response_body.c_str(), MSG_BUFFER).append("\r\n");
// 			this->_response_body = this->_response_body.substr(MSG_BUFFER);
// 		}
// 		else
// 		{
// 			chunked_body.append(utils::to_hex(this->_response_body.size())).append("\r\n");
// 			chunked_body.append(this->_response_body.c_str(), this->_response_body.size()).append("\r\n");
// 			this->_response_body = this->_response_body.substr(this->_response_body.size());
// 		}
// 		std::cout << "response_body.size(): " << this->_response_body.size() << std::endl;
// 		return (chunked_body);
// 	}
// 	else if (this->_method != "HEAD")
// 		return (this->_response_body);
// 	else
// 		return ("");
// }

// bool	Response::get_is_complete_response(void) const
// {
// 	return (this->_is_complete_response);
// }

// bool	Response::is_localhost(void) const
// {
// 	return (this->_host == "localhost" || this->_host == "127.0.0.1");
// }

// void	Response::print_directives(void) const
// {
// 	utils::StrToStrVecMap::const_iterator	it;

// 	it = this->_directives.begin();
// 	while (it != this->_directives.end())
// 	{
// 		std::cout << CYAN << it->first << " " << RESET;
// 		for (size_t i = 0; i < it->second.size(); i++)
// 			std::cout << (it->second)[i] << " ";
// 		std::cout << std::endl;
// 		it++;
// 	}
// }

// void	Response::read_path(void)
// {
// 	std::ifstream	infile;
// 	char			c;

// 	infile.open(this->_path);
// 	// std::cout << "this->_path: " << this->_path << std::endl;
// 	// if (!infile.good())
// 	// {
// 	// 	infile.open(this->_path.append(".html"));
// 	// 	if (!infile.good())
// 	// 	{
// 	// 		if (!this->_header.get_status())
// 	// 			throw ServerErrorException(__LINE__, __FILE__, E404, this->_path + " read_path failed");
// 	// 		throw ServerErrorException(__LINE__, __FILE__, this->_header.get_status(), this->_path + " read_path failed");
// 	// 	}
// 	// }
// 	// if (opendir(this->_path.c_str()) != NULL)
// 	// {
// 	// 	if (this->_directives.count("index") == 0)
// 	// 		throw ServerErrorException(__LINE__, __FILE__, E403, this->_path + " read_path is a directory");
// 	// 	infile.close();
// 	// 	infile.open(this->_path.append("/").append(this->_directives["index"][0]));
// 	// 	if (!infile.good())
// 	// 		throw ServerErrorException(__LINE__, __FILE__, E404, this->_path + " read_path failed");
// 	// }
// 	while (infile.get(c))
// 	{
// 		if (infile.fail())
// 		{
// 			infile.close();
// 			throw ServerErrorException(__LINE__, __FILE__, E500, "Read path error");
// 		}
// 		this->_response_body.push_back(c);
// 	}
// 	infile.close();
// }

// void	Response::remove_trailing_slash(std::string& path)
// {
// 	if (path.back() == '/')
// 		path.pop_back();
// }

// // std::string	Response::construct(void) const
// // {
// // 	return (std::string().append(this->get_response_header()).append(this->_body));
// // }

// // void	Response::construct_chunked_body(void)
// // {
// // 	std::string	tmp;
// // 	std::string	body;

// // 	body = this->_body;
// // 	while (body.size() > 0)
// // 	{
// // 		if (body.size() > MSG_BUFFER)
// // 		{
// // 			tmp.append(utils::itoa(MSG_BUFFER)).append("\r\n");
// // 			tmp.append(body, MSG_BUFFER).append("\r\n");
// // 			body = body.substr(MSG_BUFFER);
// // 		}
// // 		else
// // 		{
// // 			tmp.append(utils::itoa(body.size())).append("\r\n");
// // 			tmp.append(body, body.size()).append("\r\n");
// // 			body = body.substr(body.size());
// // 		}
// // 	}
// // 	tmp.append(0);
// // }
