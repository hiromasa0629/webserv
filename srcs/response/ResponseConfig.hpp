/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/07 23:52:54 by hyap              #+#    #+#             */
/*   Updated: 2023/03/11 19:31:12 by hyap             ###   ########.fr       */
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
# include "ResponseCgi.hpp"
# include "Logger.hpp"

struct ResponseConfigUriSegment {
	ResponseConfigUriSegment(const std::string& s, bool is_last);
	std::pair<bool, std::string>	is_cgi(const utils::StrVec& cgis) const;

	std::string	_s;
	bool		_is_last;

};

class ResponseConfig {
	public:
		typedef	std::vector<ResponseConfigUriSegment>	SegmentVec;

		ResponseConfig(void);
		~ResponseConfig(void);
		ResponseConfig(const TmpRequest& req, const ServerConfig& sconfig, char** envp);

		utils::StrVec								get_directives(const std::string& key) const;
		std::string									get_path(void) const;
		const std::pair<bool, ResponseAutoindex>&	get_autoindex(void) const;
		const std::pair<bool, ResponseCgi>&			get_cgi(void) const;
		const std::pair<bool, std::string>&			get_redirection(void) const;
		const std::pair<bool, std::string>&			get_put(void) const;

	private:
		void	configure(const ServerConfig& sconfig);
		bool	is_localhost(const std::string& req_server_name) const;

		/**
		 * @brief Check for server_name, limit_except and client_max_body_size
		 */
		void								handle_error(void) const;

		TmpRequest		_req;
		char**			_envp;
		std::string		_path;

		utils::StrToStrVecMap	_directives;

		std::pair<bool, ResponseAutoindex>	_autoindex;
		std::pair<bool, ResponseCgi>		_cgi;
		std::pair<bool, std::string>		_redirection;
		std::pair<bool, std::string>		_put;
		Logger								_logger;

};

#endif
