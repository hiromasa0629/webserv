/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/17 15:01:58 by hyap              #+#    #+#             */
/*   Updated: 2023/02/24 22:31:13 by hyap             ###   ########.fr       */
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
		
		utils::CharVec	get_response_header(void) const;
		const utils::CharVec&	get_body(void) const;

	private:
		bool	has_handled_error(void);
		void	set_directives(const ServerConfig& sconfig);
		void	set_path(void);
		bool	is_cgi(void);
		void	handle_error(void);
		void	handle_cgi(void);
		void	handle_normal(void);
		void	handle_normal_redirect(std::string redirect);
		bool	is_localhost(void) const;
	
		Request									_request;
		std::string								_status;
		utils::StrToStrVecMap					_directives;
		std::string								_path;

		ResponseHeader							_header;
		utils::CharVec							_body;

		Logger									_logger;
};

#endif
