/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/17 15:01:58 by hyap              #+#    #+#             */
/*   Updated: 2023/02/26 20:26:17 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_H
# define RESPONSE_H
# include <iostream>
# include "Request.hpp"
# include "Config.hpp"
# include "utils.hpp"
# include "Logger.hpp"
# include "ResponseHeader.hpp"

class Response {
	public:
		Response(void);
		Response(const Request& request, const ServerConfig& sconfig);
		~Response(void);
		
		std::string				get_response_header(void) const;
		std::string				get_body(void) const;

	private:
		bool	has_handled_error(void);
		void	set_directives(const ServerConfig& sconfig);
		void	set_path(void);
		bool	is_cgi(void);
		void	handle_error(void);
		void	handle_cgi(void);
		void	handle_normal(void);
		void	handle_redirect(void);
		void	handle_autoindex(void);
		bool	is_localhost(void) const;
		void	print_directives(void) const;
		
		void	read_path(void);
		
		void	remove_trailing_slash(std::string& path);

		Request									_request;
		std::string								_path;

		ResponseHeader							_header;
		std::string								_body;
		
		utils::StrToStrVecMap					_s_directives;
		utils::StrToStrVecMap					_l_directives;
		utils::StrToStrVecMap					_directives;

		std::string								_uri;
		std::string								_location_uri;
		std::string								_rest_of_the_uri;
		
		std::string								_host;
		int										_request_body_size;
		std::vector<std::string>				_request_body;
		std::string								_method;
		
		bool									_is_autoindex;
		bool									_is_redirection;
		
		Logger									_logger;
};

#endif
