/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/11 15:47:07 by hyap              #+#    #+#             */
/*   Updated: 2023/02/17 18:51:49 by hyap             ###   ########.fr       */
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

class LocationConfig {
	public:
		LocationConfig(void);
		LocationConfig(utils::StrVec::iterator start, utils::StrVec::iterator end);
		~LocationConfig(void);

		void	set_directives(const std::string& s);
		void	print_directives(void);

		utils::StrVec	get_directives(std::string key) const;
	private:
		utils::StrToStrVecMap	_directives;
};

class ServerConfig {
	public:
		typedef std::map< std::string, LocationConfig >	StrToLConfigMap;
		
		ServerConfig(void);
		ServerConfig(utils::StrVec::iterator start, utils::StrVec::iterator end);
		~ServerConfig(void);

		void	set_directives(const std::string& s);
		void	print_directives(void);

		utils::StrVec			get_directives(std::string key) const;
		const StrToLConfigMap&	get_lconfig(void) const;

	private:
		bool					has_required_directives(void) const;

		std::map< std::string, LocationConfig >	_lconfig;
		utils::StrToStrVecMap					_directives;
};

class Config {
	public:
		typedef std::map< std::string, ServerConfig >	StrToSConfigMap;
	
		Config(void);
		Config(const char* config_file);
		~Config(void);
		Config(const Config &src);

		void								print_config(void);
		const StrToSConfigMap&				get_sconfig(void) const;
		StrToSConfigMap::const_iterator		find_sconfig_by_host_port(const std::string& newhost, const std::string& newport) const;

	private:
		void						save_config(const char* config_file);
		void						parse_config(void);

		std::vector<std::string>				_conf; // trimmed
		StrToSConfigMap							_sconfig;
};

#endif
