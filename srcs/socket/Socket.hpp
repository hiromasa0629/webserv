/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 00:27:45 by hyap              #+#    #+#             */
/*   Updated: 2023/02/08 20:10:10 by hyap             ###   ########.fr       */
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

class Socket {
	public:
		typedef struct addrinfo		addrinfo_t;
		typedef struct sockaddr		sockaddr_t;

		Socket(int ai_flags, int ai_family, int ai_socktype, int ai_protocol, const char* hostname, const char* port);
		~Socket(void);
		Socket(const Socket &src);
		Socket	&operator=(const Socket &rhs);

		// Getters
		addrinfo_t*	get_addrinfo(void) const;

	private:
		void		init_addrinfo(int ai_flags, int ai_family, int ai_socktype, int ai_protocol, const char* hostname, const char* port);
		void		init_socket(void);

	protected:
		Socket(void);
		
		std::string		get_ip(void) const;
		unsigned short	get_port(void) const;
		

		addrinfo_t*		_addrinfo;
		int				_socketfd;

};

#endif
