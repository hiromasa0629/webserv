/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/11 15:47:11 by hyap              #+#    #+#             */
/*   Updated: 2023/02/13 16:44:52 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

/***********************************
 * Constructors
***********************************/

Config::Config(void)
{

}

Config::Config(const char* config_file)
{	
	
	try
	{
		this->save_config(config_file);
	}
	catch(const std::exception& e)
	{
		std::cerr << ERROR("Config ") << LRED << e.what() << RESET << std::endl;;
	}
}

Config::Config(const Config &src)
{
	*this = src;
}

Config	&Config::operator=(const Config &rhs)
{
	(void)rhs;
	return (*this);
}

/***********************************
 *  Functions
***********************************/

void	Config::init_default_directives(void)
{
	this->_default_server_directives.push_back("listen");
	this->_default_server_directives.push_back("root");
	this->_default_server_directives.push_back("error_page");
	this->_default_server_directives.push_back("location");
	this->_default_server_directives.push_back("limit_except");
	this->_default_server_directives.push_back("index");
	this->_default_server_directives.push_back("client_max_body_size");
	this->_default_server_directives.push_back("cgi");
	
	this->_default_location_directives.push_back("root");
	this->_default_location_directives.push_back("error_page");
	this->_default_location_directives.push_back("limit_except");
	this->_default_location_directives.push_back("index");
	this->_default_location_directives.push_back("redirect");
	this->_default_location_directives.push_back("autoindex");
	this->_default_location_directives.push_back("cgi");
}

void	Config::save_config(const char* config_file)
{
	std::ifstream	infile(config_file);
	std::string		line;
	// char	ws[6] = {32, 9, 10, 11, 12, 13};

	while (std::getline(infile, line))
	{
		if (utils::is_empty_string(line))
			continue ;
		line = line.substr(line.find_first_not_of(utils::ws), line.find_last_not_of(utils::ws) + 1);
		std::cout << line << std::endl;
		this->_conf.push_back(line);
	}
	infile.close();
	parse_config();
}

void	Config::parse_config(void)
{
	std::vector<std::string>::iterator	start;
	std::vector<std::string>::iterator	end;
	size_t								i;

	i = 0;
	while (i < this->_conf.size())
	{
		if (utils::get_nth_word(this->_conf[i], 1) == "server")
		{
			start = this->_conf.begin() + i;
			i++;
			while (utils::get_nth_word(this->_conf[i], 1) != "server" && i < this->_conf.size())
				i++;
			if (i == this->_conf.size())
				end = this->_conf.end();
			else
				end = this->_conf.begin() + i - 1;
			
		}
		else
			i++;
	}
}

/***********************************
 * Destructors
***********************************/

Config::~Config(void)
{

}

/***********************************
 *  ServerConfigs
***********************************/

ServerConfigs::ServerConfigs(void) {}

ServerConfigs::~ServerConfigs(void) {}



/***********************************
 *  LocationConfigs
***********************************/

LocationConfigs::LocationConfigs(void) {}

LocationConfigs::~LocationConfigs(void) {}
