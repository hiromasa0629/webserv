/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseCgi.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/07 20:19:47 by hyap              #+#    #+#             */
/*   Updated: 2023/03/07 20:58:40 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSECGI_H
# define RESPONSECGI_H
# include "utils.hpp"
# include <iostream>
# include <unistd.h>
# include "ServerErrorException.hpp"

class ResponseCgi {
	public:
		ResponseCgi(void);
		ResponseCgi(const std::string& path, const std::string& body, const std::string& cmd, char** envp);
		~ResponseCgi(void);
		
		void	execute(void);
		
	private:
		std::string			_path;
		std::vector<char*>	_envp;
		std::string			_body;
		std::string			_cmd;
		std::string			_response_msg;
	
};

#endif