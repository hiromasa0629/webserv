/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TmpResponse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/07 23:19:56 by hyap              #+#    #+#             */
/*   Updated: 2023/03/09 01:13:05 by hyap             ###   ########.fr       */
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
		TmpResponse(const TmpRequest& request, const ServerConfig& sconfig, char** envp);


	private:
		void		handle_redirection(const std::string& uri);
		void		handle_put(const std::string& path);
		void		handle_cgi(const std::string& cgi_msg);
		void		handle_autoindex(const std::string& body);
		void		handle_delete(const std::string& path);
		void		handle_normal(const std::string& path);
		std::string	read_file(const std::string& path);

		TmpRequest			_req;
		bool				_is_complete_resposne;

		ResponseConfig		_response_config;
		ResponseHeader		_header;

		std::string			_body;

		Logger				_logger;
};

#endif
