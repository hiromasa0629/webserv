/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TmpResponse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/07 23:19:56 by hyap              #+#    #+#             */
/*   Updated: 2023/03/08 13:51:27 by hyap             ###   ########.fr       */
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
		void		handle_redirection(void);
		std::string	read_file(void);
	
		TmpRequest			_req;
		char**				_envp;
		bool				_is_complete_resposne;
		
		ResponseConfig		_response_config;
		ResponseHeader		_header;
		
		std::string			_body;
		
		Logger				_logger;
};

#endif
