/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 00:27:45 by hyap              #+#    #+#             */
/*   Updated: 2023/02/06 00:27:34 by hyap             ###   ########.fr       */
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

class Socket {
	public:
		typedef struct addrinfo		addrinfo_t;
		typedef struct sockaddr		sockaddr_t;

		Socket(int ai_flags, int ai_family, int ai_socktype, int ai_protocol);
		~Socket(void);
		Socket(const Socket &src);
		Socket	&operator=(const Socket &rhs);

		// Getters
		addrinfo_t*	get_addrinfo(void) const;

	private:
		void		init_addrinfo(int ai_flags, int ai_family, int ai_socktype, int ai_protocol);
		void		init_socket(void);

	protected:
		Socket(void);

		addrinfo_t*		_addrinfo;
		int				_socketfd;

};

#endif
