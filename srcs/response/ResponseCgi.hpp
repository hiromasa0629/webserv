/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseCgi.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/07 20:19:47 by hyap              #+#    #+#             */
/*   Updated: 2023/03/08 23:39:14 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSECGI_H
# define RESPONSECGI_H
# include "utils.hpp"
# include <iostream>
# include <unistd.h>
# include "ServerErrorException.hpp"

# define READ_BUFFER 4096

class ResponseCgi {
	public:
		ResponseCgi(void);
		ResponseCgi(const std::string& path, const std::string& body, const std::string& cmd, char** envp);
		~ResponseCgi(void);

		void	set_envp(const std::string& key, const std::string& value);
		void	execute(void);

		const std::string&	get_response_msg(void) const;

	private:

		std::string			_path;
		std::vector<char*>	_envp;
		std::string			_body;
		std::string			_cmd;
		std::string			_response_msg;

};

#endif
