/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/11 15:47:07 by hyap              #+#    #+#             */
/*   Updated: 2023/02/14 19:04:29 by hyap             ###   ########.fr       */
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
		ServerConfig(void);
		ServerConfig(utils::StrVec::iterator start, utils::StrVec::iterator end);
		~ServerConfig(void);

		void	set_directives(const std::string& s);
		void	print_directives(void);

		utils::StrVec	get_directives(std::string key) const;

	private:
		bool					has_required_directives(void) const;

		std::map< std::string, LocationConfig >	_lconfig;
		utils::StrToStrVecMap					_directives;
};

class Config {
	public:
		typedef std::map< std::string, ServerConfig >	StrToSConfigMap;
		typedef std::map< std::string, LocationConfig >	StrToLConfigMap;
	
		Config(void);
		Config(const char* config_file);
		~Config(void);
		Config(const Config &src);

		void								print_config(void);
		const StrToSConfigMap&				get_config(void) const;

	private:
		void						save_config(const char* config_file);
		void						parse_config(void);

		std::vector<std::string>				_conf; // trimmed
		std::map< std::string, ServerConfig >	_sconfig;
};

#endif
