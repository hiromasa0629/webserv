/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TmpResponse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/07 23:19:56 by hyap              #+#    #+#             */
/*   Updated: 2023/03/07 23:55:42 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	TMPRESPONSE_H
# define TMPRESPONSE_H
# include <iostream>
# include "utils.hpp"
# include "TmpRequest.hpp"
# include "Config.hpp"

class TmpResponse {
	public:
		TmpResponse(void);
		~TmpResponse(void);
		TmpResponse(const TmpRequest& request, const ServerConfig& sconfig, char** envp);


	private:
		TmpRequest	_req;
		char**		_envp;
};

#endif
