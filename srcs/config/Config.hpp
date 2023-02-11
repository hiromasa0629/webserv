/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/11 15:47:07 by hyap              #+#    #+#             */
/*   Updated: 2023/02/11 19:26:38 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_H
# define CONFIG_H
# include <iostream>
# include <fstream>
# include "colors.hpp"
# include <vector>
# include <iterator>

# define DEF_DIRECT (std::string[]){"string"}

class Config {
	public:
		Config(void);
		Config(const char* config_file);
		~Config(void);
		Config(const Config &src);
		Config	&operator=(const Config &rhs);

	private:
		void						save_config(const char* config_file);
		void						error_handling(void);
		void						check_server_context(void);
		void						check_directives(void);
		bool						is_valid_directives(std::string d);
		void						init_default_directives(void);

		std::vector<std::string>	_conf;
		std::vector<std::string>	_default_directives;
};

#endif
