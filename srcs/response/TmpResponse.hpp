/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TmpResponse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/07 23:19:56 by hyap              #+#    #+#             */
/*   Updated: 2023/03/13 12:46:45 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	TMPRESPONSE_H
# define TMPRESPONSE_H
# include <iostream>
# include "utils.hpp"
# include "TmpRequest.hpp"
# include "ResponseConfig.hpp"
# include "ResponseHeader.hpp"
# include "Config.hpp"
# include <iterator>
# include "Logger.hpp"

# define RESPONSE_BUFFER 65536

class TmpResponse {
	public:
		TmpResponse(void);
		~TmpResponse(void);
		/**
		 * @brief Construct a new Tmp Response object. If any error occurs, throw ServerErrorException
		 *
		 * @param request TmpRequest class
		 * @param sconfig ServerConfig class
		 * @param envp environment variables
		 */
		TmpResponse(const TmpRequest& request, const ServerConfig& sconfig, char** envp);

		/**
		 * @brief Construct a new Tmp Response object
		 *
		 * @param status
		 * @param sconfig
		 */
		TmpResponse(enum StatusCode status, const ServerConfig& sconfig);

		std::string	get_body(void);
		std::string	get_header(void) const;
		bool		get_is_complete_response(void) const;

	private:
		void		handle_redirection(const std::string& uri);
		void		handle_put(const std::string& path);
		void		handle_cgi(const std::string& cgi_msg);
		void		handle_autoindex(const std::string& body);
		void		handle_delete(const std::string& path);
		void		handle_normal(const std::string& path);
		void		handle_error(enum StatusCode status, const ServerConfig& sconfig);

		TmpRequest			_req;
		bool				_is_complete_response;

		ResponseConfig		_response_config;
		ResponseHeader		_header;

		std::string			_body;
		// int					_total_rbuf_sent;

		Logger				_logger;
};

#endif
