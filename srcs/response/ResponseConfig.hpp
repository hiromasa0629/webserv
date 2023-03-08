/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/07 23:52:54 by hyap              #+#    #+#             */
/*   Updated: 2023/03/08 21:15:24 by hyap             ###   ########.fr       */
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

struct ResponseConfigUriSegment {
	ResponseConfigUriSegment(const std::string& s, bool is_last);
	// bool							is_same_extension(const std::string& ext) const;
	std::pair<bool, std::string>	is_cgi(const utils::StrVec& cgis) const;
	
	std::string	s;
	bool		is_last;
	
};

struct ResponseConfigHelper {
	ResponseConfigHelper(void);
	/**
	 * @brief Construct a new Response Config Helper object. Check if its redirect or autoindex, also 
	 * overwrites the ResponseConfig directives;
	 * 
	 * @param l_directive location directives
	 * @param directives Responseconfig directive, to be overwrtitten
	 */
	ResponseConfigHelper(const utils::StrToStrVecMap& l_directive, const std::string& location, utils::StrToStrVecMap& directives);
	
	bool		is_redirect;
	bool		is_autoindex;
	std::string	location_uri;
};

class ResponseConfig {
	public:
		typedef	std::vector<ResponseConfigUriSegment>	SegmentVec;
	
		ResponseConfig(void);
		~ResponseConfig(void);
		ResponseConfig(const TmpRequest& req, const ServerConfig& sconfig, char** envp);
		
		utils::StrVec								get_directives(const std::string& key) const;
		std::string									get_content_type(void) const;
		std::string									get_path(void) const;
		const std::pair<bool, ResponseAutoindex>&	get_autoindex(void) const;
		const std::pair<bool, ResponseCgi>&			get_cgi(void) const;

	private:
		void	configure(const ServerConfig& sconfig);
		void	set_directives(const ServerConfig& sconfig);
		
		/**
		 * @brief First, i will append the root. Then i will check if it has a matching location block and see if it has root.
		 * If has root, i will append the rest of the uri. If does not have root, i will append the whole uri.
		 * Next, check if its a directory. If it is a directory, i will check if it has index, if it does not have index and is not autoindex,
		 * throw forbidden exception. If has index, will try to open to check if it exist, if exist just append the index. If not,
		 * throw exception if its not autoindex, else set autoidnex to true.
		 */
		void	set_path(void);
		
		/**
		 * @brief Check for server_name, limit_except and client_max_body_size
		 */
		void								handle_error(void) const;
		bool								is_localhost(const std::string& req_server_name) const;
		bool								is_images(const std::string& ext) const;
		std::pair<std::string, std::string>	is_cgi(const std::string& path) const;

		TmpRequest		_req;
		char**			_envp;
		std::string		_path;

		utils::StrToStrVecMap	_s_directives;
		utils::StrToStrVecMap	_l_directives;
		utils::StrToStrVecMap	_directives;
		std::string				_rest_of_the_uri;
		std::string				_location_uri;
		
		std::pair<bool, ResponseAutoindex>	_autoindex;
		std::pair<bool, ResponseCgi>		_cgi;
		std::string							_content_type;
		
};

#endif
