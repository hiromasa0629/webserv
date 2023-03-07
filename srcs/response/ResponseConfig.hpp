/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/07 23:52:54 by hyap              #+#    #+#             */
/*   Updated: 2023/03/08 01:06:44 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSECONFIG_H
# define RESPONSECONFIG_H
# include <iostream>
# include "utils.hpp"
# include "Config.hpp"
# include "TmpRequest.hpp"
# include "ServerErrorException.hpp"
# include <dirent.h>
# include "ResponseAutoindex.hpp"

class ResponseConfig {
	public:
		ResponseConfig(void);
		~ResponseConfig(void);
		ResponseConfig(const TmpRequest& req, const ServerConfig& sconfig);

	private:
		void	set_directives(const ServerConfig& sconfig);
		void	set_path(void);

		TmpRequest		_req;
		std::string		_path;

		utils::StrToStrVecMap	_s_directives;
		utils::StrToStrVecMap	_l_directives;
		utils::StrToStrVecMap	_directives;
		std::string				_rest_of_the_uri;
		std::string				_location_uri;

		ResponseAutoindex		_autoindex;
};

#endif
