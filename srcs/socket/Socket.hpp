/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 00:27:45 by hyap              #+#    #+#             */
/*   Updated: 2023/02/14 18:12:51 by hyap             ###   ########.fr       */
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

class Socket {
	public:
		typedef struct addrinfo									addrinfo_t;
		typedef struct sockaddr									sockaddr_t;
		typedef struct std::pair< addrinfo_t *, std::string >	AddrToStrPair;
		typedef struct std::map < int, AddrToStrPair >			SocketFdsMap;

		Socket(int ai_flags, int ai_family, int ai_socktype, int ai_protocol, const Config& config);
		~Socket(void);
		Socket(const Socket &src);
		Socket	&operator=(const Socket &rhs);

		// Getters
		// addrinfo_t*	get_addrinfo(void) const;

	private:
		void		init_addrinfo(const addrinfo_t& hint);
		void		init_socket(void);

	protected:
		Socket(void);
		
		std::string		get_ip(addrinfo_t *a) const;
		unsigned short	get_port(addrinfo_t *a) const;
		bool			is_socketfd(int fd);
		

		std::vector<addrinfo_t *>		_addrinfos;
		Config							_config;
		SocketFdsMap					_socketfds;	
};

#endif
