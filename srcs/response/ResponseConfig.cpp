/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/07 23:55:57 by hyap              #+#    #+#             */
/*   Updated: 2023/03/15 15:14:17 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseConfig.hpp"

ResponseConfig::ResponseConfig(void) {}

ResponseConfig::~ResponseConfig(void) {}

ResponseConfig::ResponseConfig(const TmpRequest& req, const ServerConfig& sconfig, char** envp)
	: _req(req), _envp(envp),
		_autoindex(std::make_pair(false, ResponseAutoindex())),
		_cgi(std::make_pair(false, ResponseCgi())),
		_redirection(std::make_pair(false, std::string())),
		_put(std::make_pair(false, std::string()))
{
	this->configure(sconfig);
	if (this->_redirection.first)
		return ;
}

utils::StrVec	ResponseConfig::get_directives(const std::string& key) const
{
	if (this->_directives.count(key) > 0)
		return (this->_directives.at(key));
	return (utils::StrVec());
}

std::string	ResponseConfig::get_path(void) const
{
	return (this->_path);
}

const std::pair<bool, ResponseAutoindex>&	ResponseConfig::get_autoindex(void) const
{
	return (this->_autoindex);
}

const std::pair<bool, ResponseCgi>&	ResponseConfig::get_cgi(void) const
{
	return (this->_cgi);
}

const std::pair<bool, std::string>&	ResponseConfig::get_redirection(void) const
{
	return (this->_redirection);
}

const std::pair<bool, std::string>&	ResponseConfig::get_put(void) const
{
	return (this->_put);
}

static	ResponseConfig::SegmentVec	split_uri(std::string uri)
{
	ResponseConfig::SegmentVec	sv;

	while (uri.size() > 0)
	{
		std::string	tmp		= std::string(uri.begin() + 1, uri.end());
		size_t		index	= tmp.find_first_of('/');

		if (index == std::string::npos)
		{
			sv.push_back(ResponseConfigUriSegment(uri.substr(0, index), true));
			break ;
		}
		else
			sv.push_back(ResponseConfigUriSegment(uri.substr(0, index + 1), false));
		uri = uri.substr(index + 1);
	}
	return (sv);
}

static void	overwrite_directives(const utils::StrToStrVecMap& l_directives, utils::StrToStrVecMap& directives, bool& is_redirect, bool& is_autoindex)
{
	utils::StrToStrVecMap::const_iterator	it;

	it = l_directives.begin();
	for (; it != l_directives.end(); it++)
	{
		directives[it->first] = it->second;
		if (it->first == "autoindex" && it->second[0] == "on")
			is_autoindex = true;
		if (it->first == "return")
			is_redirect = true;
	}
}

void	ResponseConfig::configure(const ServerConfig& sconfig)
{
	SegmentVec						segmented_uri;
	bool							is_redirect		= false;
	bool							is_autoindex	= false;
	ServerConfig::StrToLConfigMap	l_config;


	this->_directives = sconfig.get_directives();
	l_config = sconfig.get_lconfig();
	this->_path = this->_directives["root"][0];
	segmented_uri = split_uri(this->_req.get_request_field(URI));
	for (size_t i = 0; i < segmented_uri.size(); i++)
	{
		if (i == 0 && l_config.count(segmented_uri[i]._s) > 0)
		{
			utils::StrToStrVecMap			l_directives = l_config[segmented_uri[i]._s].get_directives();
			overwrite_directives(l_config[segmented_uri[i]._s].get_directives(), this->_directives, is_redirect, is_autoindex);

			if (l_directives.count("root"))
				this->_path = this->_directives["root"][0];
			else
				this->_path.append(segmented_uri[i]._s);
		}
		else
			this->_path.append(segmented_uri[i]._s);

		if (i == 0)
			this->handle_error();

		if (i == 0)
		{
			std::string	cgi_path = this->_path;
			int			tmp_j = 0;
			if (l_config.count(segmented_uri[i]._s) > 0)
				tmp_j = 1;
			for (size_t j = tmp_j; j < segmented_uri.size(); j++)
			{
				if (j != 0)
					cgi_path.append(segmented_uri[j]._s);

				std::pair<bool, std::string>	cgi_pair;

				if (this->_directives.count("cgi") && (cgi_pair = segmented_uri[j].is_cgi(this->_directives["cgi"])).first)
				{
					std::string	path_info;

					for (size_t k = j + 1; k < segmented_uri.size(); k++)
						path_info.append(segmented_uri[k]._s);
					if (path_info.empty())
						path_info = this->_req.get_request_field(URI);
					this->_cgi.first = true;
					this->_cgi.second = ResponseCgi(cgi_path, this->_req, cgi_pair.second, this->_envp);
					this->_cgi.second.set_envp("PATH_INFO", path_info);
#if DEBUG
					this->_logger.debug("Executing " + cgi_path);
#endif
					this->_cgi.second.execute();

					return ;
				}
			}
		}

		if (is_redirect)
		{
			std::string	return_uri;
			std::string	rest_of_the_uri;

			this->_redirection.first = true;
			return_uri = this->_directives["return"][0];
			if (return_uri.substr(0, 7) == "http://" || return_uri.substr(0, 8) == "https://")
			{
				this->_redirection.second = return_uri;
				return ;
			}
			if (return_uri[0] != '/')
				return_uri.insert(return_uri.begin(), '/');
			this->_redirection.second.append(return_uri);
			for (size_t j = i + 1; j < segmented_uri.size(); j++)
				rest_of_the_uri.append(segmented_uri[j]._s);
			this->_redirection.second.append(rest_of_the_uri);

			return ;
		}

		if (this->_req.get_request_field(METHOD) == "PUT")
		{
			this->_put.first = true;
			if (this->_directives.count("upload") > 0)
			{
				std::string	upload = this->_directives["upload"][0];
				this->_put.second.append(upload);

				std::string	rest_of_the_uri;
				for (size_t j = i + 1; j < segmented_uri.size(); j++)
					rest_of_the_uri.append(segmented_uri[j]._s);
				this->_put.second.append(rest_of_the_uri);
			}
			else
			{
				this->_put.second.append(this->_directives["root"][0]);
				this->_put.second.append(this->_req.get_request_field(URI));
			}

			return ;
		}

		if (segmented_uri[i]._is_last)
		{
			DIR*	dir;

			dir = NULL;
			if ((dir = opendir(this->_path.c_str())) != NULL && is_autoindex)
			{
				this->_autoindex.first = true;
				this->_autoindex.second = ResponseAutoindex(this->_path, this->_req.get_request_field(URI), this->_req.get_request_field(SERVER_NAME), this->_req.get_request_field(PORT));
				closedir(dir);
			}
			else if (dir != NULL)
			{
				if (this->_directives.count("index") == 0)
				{
					closedir(dir);
					throw ServerErrorException(__LINE__, __FILE__, E404, this->_path + " Not found");
				}

				this->_path.append("/").append(this->_directives["index"][0]);

				std::ifstream	infile(this->_path.c_str());

				if (!infile.good())
				{
					closedir(dir);
					throw ServerErrorException(__LINE__, __FILE__, E404, this->_path + " Not found");
				}
				infile.close();
				closedir(dir);
			}
			else
			{
				std::ifstream	infile;

				infile.open(this->_path.c_str());
				if (!infile.good())
				{
					infile.open((this->_path + ".html").c_str());
					if (!infile.good())
						throw ServerErrorException(__LINE__, __FILE__, E404, this->_path + " Not found");
					this->_path.append(".html");
				}
				infile.close();
			}
		}
	}
}

void	ResponseConfig::handle_error(void) const
{
	if (this->_req.get_status_code() != S200)
		throw ServerErrorException(__LINE__, __FILE__, this->_req.get_status_code(), "Bad request");
	if (this->_directives.count("server_name") > 0)
	{
		utils::StrVec	sv;
		std::string		req_server_name;

		sv = this->_directives.at("server_name");
		req_server_name = this->_req.get_request_field(SERVER_NAME);
		if (!std::count(sv.begin(), sv.end(), "_") && !std::count(sv.begin(), sv.end(), req_server_name) && !this->is_localhost(req_server_name))
			throw ServerErrorException(__LINE__, __FILE__, E404, req_server_name + " invalid server_name");
	}

	if (this->_directives.count("limit_except") > 0)
	{
		utils::StrVec	sv;

		sv = this->_directives.at("limit_except");
		if (!std::count(sv.begin(), sv.end(), this->_req.get_request_field(METHOD)))
			throw ServerErrorException(__LINE__, __FILE__, E405, "Method not allowed");
	}
	if (this->_directives.count("client_max_body_size") > 0)
	{
		size_t	client_max_body_size;

		client_max_body_size = std::atoi(this->_directives.at("client_max_body_size")[0].c_str());
		if (client_max_body_size < this->_req.get_body().size())
			throw ServerErrorException(__LINE__, __FILE__, E413, "Body size too large");
		if (!this->_req.get_unchunked_filename().empty())
		{
			std::ifstream	infile;
			size_t			size;

			infile.open(this->_req.get_unchunked_filename().c_str(), std::ios::binary);
			if (!infile.good())
				throw ServerErrorException(__LINE__, __FILE__, E500, "Handle error infile failed");
			infile.seekg(0, infile.end);
			size = infile.tellg();
			infile.close();
			if (client_max_body_size < size)
				throw ServerErrorException(__LINE__, __FILE__, E413, "Body size too large");
		}
	}
}

bool	ResponseConfig::is_localhost(const std::string& req_server_name) const
{
	return (req_server_name == "localhost" || req_server_name == "127.0.0.1");
}

ResponseConfigUriSegment::ResponseConfigUriSegment(const std::string& s, bool is_last) : _s(s), _is_last(is_last) {}

std::pair<bool, std::string>	ResponseConfigUriSegment::is_cgi(const utils::StrVec& cgis) const
{
	for (size_t i = 0; i < cgis.size(); i++)
	{
		std::string	ext = cgis[i++];
		std::string cmd = cgis[i];

		if (_s.length() < ext.length())
			continue ;
		if (_s.substr(_s.length() - ext.length()) == ext)
			return (std::make_pair(true, cmd));
	}
	return (std::make_pair(false, std::string()));
}

