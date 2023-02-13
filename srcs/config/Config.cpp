/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/11 15:47:11 by hyap              #+#    #+#             */
/*   Updated: 2023/02/13 21:43:11 by hyap             ###   ########.fr       */
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
	this->init_default_directives();
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

}

void	Config::save_config(const char* config_file)
{
	std::ifstream	infile(config_file);
	std::string		line;

	while (std::getline(infile, line))
	{
		if (utils::is_empty_string(line))
			continue ;
		line = line.substr(line.find_first_not_of(utils::ws), line.find_last_not_of(utils::ws) + 1);
		this->_conf.push_back(line);
	}
	infile.close();
	parse_config();
}

void	Config::parse_config(void)
{
	utils::StrVec::iterator	start;
	utils::StrVec::iterator	end;
	size_t					i;

	i = 0;
	while (i < this->_conf.size())
	{
		if (utils::ft_split(this->_conf[i])[0] == "server")  
		{
			start = this->_conf.begin() + i + 1;
			i++;
			while (i < this->_conf.size() && utils::ft_split(this->_conf[i])[0] != "server")
				i++;
			if (i == this->_conf.size())
				end = this->_conf.end() - 1;
			else
				end = this->_conf.begin() + i - 1;
			this->_sconfig.push_back(ServerConfig(start, end));
		}
		else
			i++;
	}
}

void	Config::print_config(void)
{
	for (size_t i = 0; i < this->_sconfig.size(); i++)
	{
		std::cout << BOLD << BLUE << "========== Server ==========" << RESET << std::endl;
		this->_sconfig[i].print_directives();
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

ServerConfig::ServerConfig(void) {}

ServerConfig::~ServerConfig(void) {}

ServerConfig::ServerConfig(utils::StrVec::iterator start, utils::StrVec::iterator end)
{
	utils::StrVec::iterator	first;
	utils::StrVec::iterator	last;
	utils::StrVec			split;
	
	while (start < end)
	{
		split = utils::ft_split(*start);
		if (split[0] == "location")
		{
			first = ++start;
			while (start < end && utils::ft_split(*start)[0] != "location")
				start++;
			if (start == end)
				last = end - 1;
			else
				last = start - 1;
			this->_lconfig.insert(std::make_pair(split[1], LocationConfig(first, last)));
		}
		else
		{
			this->set_directives(*start);
			start++;
		}
	}
}

void	ServerConfig::set_directives(const std::string& s)
{
	utils::StrVec	split;
	std::string		key;
	
	split = utils::ft_split(s);
	key = split[0];
	if (!utils::is_valid_server_directives(key))
		throw std::runtime_error("\"" + key + "\" invalid server directive");
	split.erase(split.begin());
	if (this->_directives.find(key) == this->_directives.end())
		this->_directives[key] = split;
	else
		this->_directives[key].insert(this->_directives[key].end(), split.begin(), split.end());
}

void	ServerConfig::print_directives(void)
{
	utils::StrToStrVecMap::iterator	it;
	std::map< std::string, LocationConfig >::iterator	it2;
	
	it = this->_directives.begin();
	while (it != this->_directives.end())
	{
		std::cout << CYAN << it->first << " " << RESET;
		for (size_t i = 0; i < it->second.size(); i++)
			std::cout << (it->second)[i] << " ";
		std::cout << std::endl;
		it++;
	}
	it2 = this->_lconfig.begin();
	while (it2 != this->_lconfig.end())
	{
		std::cout << RED << it2->first << RESET << std::endl;
		it2->second.print_directives();
		it2++;
	}
}

/***********************************
 *  LocationConfigs
***********************************/

LocationConfig::LocationConfig(void) {}

LocationConfig::~LocationConfig(void) {}

LocationConfig::LocationConfig(utils::StrVec::iterator start, utils::StrVec::iterator end)
{
	while (start < end)
	{
		this->set_directives(*start);
		start++;
	}
}

void	LocationConfig::set_directives(const std::string& s)
{
	utils::StrVec	split;
	std::string		key;
	
	split = utils::ft_split(s);
	key = split[0];
	split.erase(split.begin());
	this->_directives[key] = split;
}

void	LocationConfig::print_directives(void)
{
	utils::StrToStrVecMap::iterator	it;
	
	it = this->_directives.begin();
	while (it != this->_directives.end())
	{
		std::cout << "\t" << CYAN << it->first << " " << RESET;
		for (size_t i = 0; i < it->second.size(); i++)
			std::cout << (it->second)[i] << " ";
		std::cout << std::endl;
		it++;
	}
}
