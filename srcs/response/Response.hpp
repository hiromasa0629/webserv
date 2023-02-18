/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/17 15:01:58 by hyap              #+#    #+#             */
/*   Updated: 2023/02/18 18:42:34 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_H
# define RESPONSE_H
# include <iostream>
# include "Request.hpp"
# include "Config.hpp"
# include "utils.hpp"
# include "Logger.hpp"

class Response {
	public:
		Response(void);
		Response(const Request& request, const ServerConfig& sconfig);
		~Response(void);

	private:
		void	construct_response(void);
		bool	is_cgi(void) const;
	
		BlockConfig				_config;
		Request					_request;
		utils::CharVec			_body;
		std::string				_status;
		Logger					_logger;
		utils::StrToStrVecMap	_directives;
};

#endif