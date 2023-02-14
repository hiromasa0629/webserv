/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/11 15:47:11 by hyap              #+#    #+#             */
/*   Updated: 2023/02/14 19:36:49 by hyap             ###   ########.fr       */
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
		std::cerr << ERROR("Confg::Config ") << LRED << e.what() << RESET << std::endl;;
		std::exit(1);
	}
}

Config::Config(const Config &src)
{
	*this = src;
}

/***********************************
 *  Functions
***********************************/

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
	ServerConfig			tmp_sconfig;
	size_t					i;

	i = 0;
	while (i < this->_conf.size())
	{
		if (utils::ft_split(this->_conf[i])[0] == "server")
		{
			start = this->_conf.begin() + i++ + 1;
			while (i < this->_conf.size() && utils::ft_split(this->_conf[i])[0] != "server")
				i++;
			if (i == this->_conf.size())
				end = this->_conf.end() - 1;
			else
				end = this->_conf.begin() + i - 1;
			tmp_sconfig = ServerConfig(start, end);
			// If server_name has not existed yet
			if (this->_sconfig.find(tmp_sconfig.get_directives("server_name")[0]) == this->_sconfig.end())
				this->_sconfig.insert(std::make_pair(tmp_sconfig.get_directives("server_name")[0], tmp_sconfig));
		}
		else
			i++;
	}
}

void	Config::print_config(void)
{
	StrToSConfigMap::iterator	it;
	
	it = this->_sconfig.begin();
	for (; it != this->_sconfig.end(); it++)
	{
		std::cout << BOLD << BLUE << "========== Server ==========" << RESET << std::endl;
		it->second.print_directives();
	}
}

const Config::StrToSConfigMap&	Config::get_config(void) const
{
	return (this->_sconfig);
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
			// std::cout << *start << std::endl;
			this->set_directives(*start);
			start++;
		}
	}
	if (!this->has_required_directives())
		throw std::runtime_error("Missing required directives in server block");
	if (this->_directives.find("server_name") == this->_directives.end())
		this->_directives["server_name"] = utils::StrVec(1, "_");
}

void	ServerConfig::set_directives(const std::string& s)
{
	utils::StrVec	split;
	std::string		key;
	utils::StrVec	tmp;

	split = utils::ft_split(s);
	key = split[0];
	if (!utils::is_valid_server_directives(key))
		throw std::runtime_error("\"" + key + "\" invalid server directive");
	split.erase(split.begin());
	if (this->_directives.find(key) == this->_directives.end())
	{
		// handle listen to hostname:port
		if (key == "listen")
		{
			tmp.push_back(split[0].substr(0, split[0].find_first_of(':')));
			tmp.push_back(split[0].substr(split[0].find_first_of(':') + 1, split[0].length()));
			this->_directives[key] = tmp;
		}
		else
			this->_directives[key] = split;
	}
	else // if key already exist insert vector at the end
		this->_directives[key].insert(this->_directives[key].end(), split.begin(), split.end());
}

void	ServerConfig::print_directives(void)
{
	utils::StrToStrVecMap::iterator						it;
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

utils::StrVec	ServerConfig::get_directives(std::string key) const
{
	return (this->_directives.find(key)->second);
}

bool	ServerConfig::has_required_directives(void) const
{
	if (this->_directives.find("listen") == this->_directives.end() || this->_directives.find("root") == this->_directives.end())
		return (false);
	return (true);
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
	if (!utils::is_valid_location_directives(key))
		throw std::runtime_error("\"" + key + "\" invalid location directive");
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

utils::StrVec	LocationConfig::get_directives(std::string key) const
{
	return (this->_directives.find(key)->second);
}
