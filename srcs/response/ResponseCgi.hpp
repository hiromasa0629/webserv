/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseCgi.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/07 20:19:47 by hyap              #+#    #+#             */
/*   Updated: 2023/03/14 14:56:35 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSECGI_H
# define RESPONSECGI_H
# include "utils.hpp"
# include <iostream>
# include <unistd.h>
# include "ServerErrorException.hpp"
# include "TmpRequest.hpp"
# include <fcntl.h>

# define READ_BUFFER 8192

class ResponseCgi {
	public:
		ResponseCgi(void);
		// ResponseCgi(const std::string& path, const std::string& body, const std::string& cmd, char** envp);
		ResponseCgi(const std::string& path, const TmpRequest& req, const std::string& cmd, char** envp);
		~ResponseCgi(void);

		void	set_envp(const std::string& key, const std::string& value);
		void	execute(void);
		void	child_process(int* pipe_to_child, int* pipe_to_parent);

		const std::string&	get_cgi_filename(void) const;

	private:

		std::string							_path;
		TmpRequest							_req;
		char**								_envp;
		utils::StrVec						_add_envp;
		std::string							_body;
		std::string							_cmd;
		// std::string							_response_msg;
		std::string							_cgi_filename;

		Logger								_logger;

};

#endif
