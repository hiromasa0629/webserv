/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 00:25:47 by hyap              #+#    #+#             */
/*   Updated: 2023/02/25 16:57:53 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_H
# define SERVER_H
# include <iostream>
# include "Socket.hpp"
# include "Config.hpp"
# include "Logger.hpp"
# include <sys/select.h>
# include "Request.hpp"
# include "Response.hpp"

# define TIMEOUT_SEC	1
# define TIMEOUT_USEC	0
# define BUFFER_SIZE	1024

class Request;

class Server {
	public:
		typedef struct pollfd	pollfd_t;
		typedef struct timeval	timeval_t;
	
		Server(int ai_flags, int ai_family, int ai_socktype, int ai_protocol, const Config& config);
		~Server(void);
		Server(const Server &src);
		Server	&operator=(const Server &rhs);

		void	run(void);

	private:
		Server(void);

		void	init_bind(void);
		void	init_listen(void);
		void	insert_fd_to_fds(int fd, short event);
		
		/* poll()
		void	main_loop(void);
		void	accept_connection(int fd);
		void	handle_pollin(const pollfd_t& pollfd);
		void	handle_pollout(const pollfd_t& pollfd);
		*/
		void	main_loop_select(void);
		void	accept_connection_select(int fd, int* maxfd);
		void	handle_pollin_select(int fd);
		void	handle_pollout_select(int fd);
		
		std::vector<char>	read_request_header(int fd);
		bool				is_socketfd(int fd) const;
		
		std::vector<Socket>::iterator	get_socket_from_fd(int fd);
		
		std::vector<pollfd_t>		_fds; // poll()
		std::vector<Socket>			_sockets;
		std::pair<fd_set, fd_set>	_fd_sets; // < read, write >
		std::map<int, ServerConfig>	_fd_sconfig;
		std::map<int, Request>		_fd_requests;
		static const char*			_example_res;
		timeval_t					_timeval;
		Logger						_logger;
};

#endif
