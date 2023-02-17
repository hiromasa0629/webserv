/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/17 15:01:58 by hyap              #+#    #+#             */
/*   Updated: 2023/02/17 18:45:54 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_H
# define RESPONSE_H
# include <iostream>
# include "Request.hpp"
# include "Config.hpp"
# include "utils.hpp"

class Response {
	public:
		Response(void);
		Response(const Request& request, const Config& config);
		~Response(void);

	private:
		ServerConfig	_sconfig;
		Request			_request;
		utils::CharVec	_body;
};

#endif