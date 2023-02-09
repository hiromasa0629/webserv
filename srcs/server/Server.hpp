/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 00:25:47 by hyap              #+#    #+#             */
/*   Updated: 2023/02/09 20:17:08 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_H
# define SERVER_H
# include <iostream>
# include "Socket.hpp"

class Server : public Socket {
	public:
		typedef struct pollfd	pollfd_t;
	
		Server(int ai_flags, int ai_family, int ai_socktype, int ai_protocol, const char* hostname, const char* port);
		~Server(void);
		Server(const Server &src);
		Server	&operator=(const Server &rhs);

		void	run(void);

	private:
		Server(void);

		void	init_bind(void);
		void	init_listen(void);
		void	insert_fd_to_fds(int fd, short event);
		
		void	accept_connection(void);
		void	handle_pollin(const pollfd_t& pollfd);
		void	handle_pollout(const pollfd_t& pollfd);
		
		void	print_request_header(int fd);
		
		void	main_loop(void);
		

		std::vector<pollfd_t>		_fds;
		
		static const char*			_example_res;
};

bool	operator==(const struct pollfd& lhs, const struct pollfd& rhs);

#endif
