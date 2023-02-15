/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 00:27:45 by hyap              #+#    #+#             */
/*   Updated: 2023/02/15 16:31:00 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_H
# define SOCKET_H
# include <iostream>
# include <sys/socket.h>
# include <sys/types.h>
# include <netdb.h>
# include <exception>
# include <fcntl.h>
# include <unistd.h>
# include <sys/select.h>
# include <sys/time.h>
# include <sstream>
# include "colors.hpp"
# include <poll.h>
# include <vector>
# include <iterator>
# include "Config.hpp"
# include "Logger.hpp"

class Socket {
	public:
		typedef struct addrinfo									addrinfo_t;
		typedef struct sockaddr									sockaddr_t;

		Socket(void);
		Socket(int ai_flags, int ai_family, int ai_socktype, int ai_protocol, const ServerConfig& sconfig);
		~Socket(void);
		Socket(const Socket &src);
		Socket	&operator=(const Socket &rhs);

		int				get_socketfd(void) const;
		std::string		get_ip(void) const;
		std::string		get_port(void) const;
		addrinfo_t*		get_addrinfo(void) const;
		
	private:
		void		init_addrinfo(const addrinfo_t& hint);
		void		init_socket(void);


		addrinfo_t*						_addrinfo;
		int								_socketfd;
		ServerConfig					_config;
		Logger							_logger;
};

std::string			operator+(const char* s, const Socket& socket);
std::ostream&		operator<<(std::ostream& o, const Socket& socket);

#endif
