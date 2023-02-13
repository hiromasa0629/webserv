/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/11 15:47:07 by hyap              #+#    #+#             */
/*   Updated: 2023/02/13 16:43:20 by hyap             ###   ########.fr       */
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

struct LocationConfigs {
	LocationConfigs(void);
	~LocationConfigs(void);
	
	std::map< std::string, std::vector<std::string> >	directs;
};

struct ServerConfigs {
	ServerConfigs(void);
	ServerConfigs(utils::vecstr_iterator start, utils::vecstr_iterator end);
	~ServerConfigs(void);
	
	std::vector<LocationConfigs>						lconfigs;
	std::map< std::string, std::vector<std::string> >	directs;
};

class Config {
	public:
		Config(void);
		Config(const char* config_file);
		~Config(void);
		Config(const Config &src);
		Config	&operator=(const Config &rhs);

	private:
		void						save_config(const char* config_file);
		// void						error_handling(void);
		// void						check_server_context(void);
		// void						check_directives(void);
		// bool						is_valid_directives(std::string d);
		void						init_default_directives(void);
		void						parse_config(void);
		

		std::vector<std::string>	_conf; // trimmed
		std::vector<std::string>	_default_server_directives;
		std::vector<std::string>	_default_location_directives;
		
		std::vector<ServerConfigs>		_sconfigs;
};

#endif
