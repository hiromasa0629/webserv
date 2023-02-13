/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/11 15:47:07 by hyap              #+#    #+#             */
/*   Updated: 2023/02/13 21:11:19 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_H
# define CONFIG_H
# include <iostream>
# include <fstream>
# include "colors.hpp"
# include <vector>
# include <iterator>
# include "utils.hpp"
# include <map>

struct LocationConfig {
	LocationConfig(void);
	LocationConfig(utils::StrVec::iterator start, utils::StrVec::iterator end);
	~LocationConfig(void);
	
	void	set_directives(const std::string& s);
	void	print_directives(void);
	
	utils::StrToStrVecMap	_directives;
};

struct ServerConfig {
	ServerConfig(void);
	ServerConfig(utils::StrVec::iterator start, utils::StrVec::iterator end);
	~ServerConfig(void);
	
	void	set_directives(const std::string& s);
	void	print_directives(void);
	
	std::map< std::string, LocationConfig >	_lconfig;
	utils::StrToStrVecMap					_directives;
	utils::StrVec							_default_server_directives;
};

class Config {
	public:
		Config(void);
		Config(const char* config_file);
		~Config(void);
		Config(const Config &src);
		Config	&operator=(const Config &rhs);
		
		void						print_config(void);

	private:
		void						save_config(const char* config_file);
		void						init_default_directives(void);
		void						parse_config(void);
		
		std::vector<std::string>	_conf; // trimmed
		std::vector<std::string>	_default_server_directives;
		std::vector<std::string>	_default_location_directives;
		
		std::vector<ServerConfig>		_sconfig;
};

#endif
