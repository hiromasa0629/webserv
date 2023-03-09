/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseCgi.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/07 20:19:47 by hyap              #+#    #+#             */
/*   Updated: 2023/03/09 15:55:15 by hyap             ###   ########.fr       */
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
		void	child_process(int* pipe_to_child, int* pipe_to_parent);

		const std::string&	get_response_msg(void) const;

	private:

		std::string							_path;
		char**								_envp;
		utils::StrVec						_add_envp;
		std::string							_body;
		std::string							_cmd;
		std::string							_response_msg;

};

#endif
