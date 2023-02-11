/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/11 15:47:11 by hyap              #+#    #+#             */
/*   Updated: 2023/02/11 19:27:12 by hyap             ###   ########.fr       */
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
	this->_default_directives.push_back("listen");
	this->_default_directives.push_back("root");
	this->_default_directives.push_back("error_page");
	this->_default_directives.push_back("location");
	this->_default_directives.push_back("limit_except");
	this->_default_directives.push_back("index");
	this->_default_directives.push_back("redirect");
	this->_default_directives.push_back("autoindex");
}

void	Config::save_config(const char* config_file)
{
	std::ifstream	infile(config_file);
	std::string		line;
	char			ws[6] = {32, 9, 10, 11, 12, 13};

	while (std::getline(infile, line))
	{
		if (line.length() == 0)
			continue ;
		line = line.substr(line.find_first_not_of(ws), line.find_last_not_of(ws) + 1);
		std::cout << line << std::endl;
		this->_conf.push_back(line);
	}
	infile.close();
	this->error_handling();
}

void	Config::error_handling(void)
{
	this->check_server_context();
	this->check_directives();
}

// Check for valid server string, closing braces and seperated by one space
void	Config::check_server_context(void)
{
	std::string		first_line;
	size_t			space_pos;
	
	first_line = this->_conf.front();
	if (first_line.substr(0, first_line.find(" ")) != "server")
		throw std::runtime_error("Invalid server directive");
	if (first_line.back() != '{' || this->_conf.back().front() != '}')
		throw std::runtime_error("Server braces not closed");
	space_pos = first_line.find_first_of(" ");
	if (first_line.substr(0, space_pos) != "server" || first_line.substr(space_pos + 1, first_line.length()) != "{")
		throw std::runtime_error("Expected single space between server and brace");
	
	// If success
	this->_conf.pop_back();
	this->_conf.erase(this->_conf.begin());
}

// Check for valid directives
void	Config::check_directives(void)
{
	std::string							direct;
	std::vector<std::string>::iterator	it;
	
	for (; it < this->_conf.end(); it++)
	{
		direct = this->_conf[i].substr(0, this->_conf[i].find(" "));
		if (!this->is_valid_directives(direct))
			throw std::runtime_error("Invalid directives");
		
	}
}

bool	Config::is_valid_directives(std::string d)
{
	for (size_t i = 0; i < this->_default_directives.size(); i++)
		if (d == this->_default_directives[i])
			return (true);
	return (false);
}

/***********************************
 * Destructors
***********************************/

Config::~Config(void)
{

}
