/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 00:25:47 by hyap              #+#    #+#             */
/*   Updated: 2023/03/09 14:41:08 by hyap             ###   ########.fr       */
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
# include <csignal>
# include "TmpResponse.hpp"
# include "TmpRequest.hpp"
# include "ServerErrorException.hpp"

# define TIMEOUT_SEC	1
# define TIMEOUT_USEC	0

class Request;

class Server {
	public:
		typedef struct pollfd	pollfd_t;
		typedef struct timeval	timeval_t;
	
		Server(int ai_flags, int ai_family, int ai_socktype, int ai_protocol, const Config& config, char** envp);
		~Server(void);
		Server(const Server &src);
		// Server	&operator=(const Server &rhs);

		void	run(void);
		

	private:
		Server(void);
		
		/**
		 * @brief bind address to socket
		 */
		void	init_bind(void);
		
		/**
		 * @brief start listening on scoket
		 */
		void	init_listen(void);
		
		/**
		 * @brief insert fd into the fd pool for poll()
		 * 
		 * @param fd socket or client fd
		 * @param event POLLIN or POLLOUT
		 */
		// void	insert_fd_to_fds(int fd, short event);
		
		// poll()
		// void	main_loop(void);
		// void	accept_connection(int fd);
		// void	handle_pollin(const pollfd_t& pollfd);
		// void	handle_pollout(const pollfd_t& pollfd);
		
		
		/**
		 * @brief the main loop where the select() is. select() is being called in every loop
		 * to select readied fd to be read or write from read and write fd pool. After select()
		 * both of the pools will be left with fds which are readied.
		 */
		void	main_loop_select(void);
		
		/**
		 * @brief accept connection if fd is a socket
		 * 
		 * @param fd socket fd
		 * @param maxfd to modify the maxfd value if client fd is larger
		 */
		void	accept_connection_select(int fd, int* maxfd);
		
		/**
		 * @brief handle readied fds by looping from 0 to maxfd to see which fd is readied to be read
		 * 
		 * @param fd client fd
		 */
		void	handle_pollin_select(int fd);
		
		/**
		 * @brief handle readied fds by looping from 0 to maxfd to see which fd is readied to be written
		 * 
		 * @param fd client fd
		 */
		void	handle_pollout_select(int fd);
		
		/**
		 * @brief read client fd
		 * 
		 * @param fd client fd
		 * @return std::string request message
		 */
		std::string			read_request(int fd);
		
		/**
		 * @brief check if its a socketfd
		 * 
		 * @param fd client fd
		 * @return true if fd is a socket
		 * @return false if fd is not a socket
		 */
		bool				is_socketfd(int fd) const;
		
		/**
		 * @brief Get the socket from matching fd
		 * 
		 * @param fd client fd
		 * @return std::vector<Socket>::iterator 
		 */
		std::vector<Socket>::iterator	get_socket_from_fd(int fd);
		
		// std::vector<pollfd_t>		_fds; // poll()
		std::vector<Socket>			_sockets;
		std::pair<fd_set, fd_set>	_fd_sets; // < read, write >
		std::map<int, ServerConfig>	_fd_sconfig;
		std::map<int, TmpRequest>	_fd_requests;
		std::map<int, TmpResponse>	_fd_response;
		timeval_t					_timeval;
		Logger						_logger;
		
		static const char*			_example_res;
		
		char**						_envp;
		
};

#endif
