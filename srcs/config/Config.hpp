/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/11 15:47:07 by hyap              #+#    #+#             */
/*   Updated: 2023/02/19 17:08:28 by hyap             ###   ########.fr       */
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

enum ConfigType {
	SERVER,
	LOCATION
}

class BlockConfig {
	public:
		BlockConfig(void);
		~BlockConfig(void);
		
		virtual void					set_directives(const std::string& s);
		virtual	void					print_directives(void);
		const utils::StrVec&			get_directives(std::string key) const;
		const utils::StrToStrVecMap&	get_directives(void) const;
		ConfigType						get_type(void) const;
		
	protected:
		utils::StrToStrVecMap	_directives;
		ConfigType				_type;
};

class LocationConfig : public BlockConfig {
	public:
		LocationConfig(void);
		LocationConfig(utils::StrVec::iterator start, utils::StrVec::iterator end);
		~LocationConfig(void);

		void	set_directives(const std::string& s);
	private:
};

class ServerConfig : public BlockConfig {
	public:
		typedef std::map< std::string, LocationConfig >	StrToLConfigMap;
		
		ServerConfig(void);
		ServerConfig(utils::StrVec::iterator start, utils::StrVec::iterator end);
		~ServerConfig(void);

		void	set_directives(const std::string& s);
		void	print_directives(void);

		const StrToLConfigMap&	get_lconfig(void) const;

	private:
		bool					has_required_directives(void) const;

		std::map< std::string, LocationConfig >	_lconfig;
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
