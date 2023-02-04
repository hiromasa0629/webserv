/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 00:27:45 by hyap              #+#    #+#             */
/*   Updated: 2023/02/04 16:04:19 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_H
# define SOCKET_H
# include <iostream>
# include <sys/socket.h>
# include <sys/types.h>
# include <netdb.h>
# include <exception>

class Socket {
	public:
		typedef struct addrinfo		addrinfo_t;
		typedef struct sockaddr		sockaddr_t;

		Socket(void);
		~Socket(void);
		Socket(const Socket &src);
		Socket	&operator=(const Socket &rhs);

		// Getters
		addrinfo_t*	get_addrinfo(void) const;
		
		// Setters
		

	private:
		addrinfo_t*		_addrinfo;
		int				_socketfd;
	
	protected:
	
};

#endif
