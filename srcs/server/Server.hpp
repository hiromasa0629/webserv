/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 00:25:47 by hyap              #+#    #+#             */
/*   Updated: 2023/02/06 00:05:38 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_H
# define SERVER_H
# include <iostream>
# include "Socket.hpp"

class Server : public Socket {
	public:
		Server(int ai_flags, int ai_family, int ai_socktype, int ai_protocol);
		~Server(void);
		Server(const Server &src);
		Server	&operator=(const Server &rhs);

		void	accept_connection(void);

	private:
		Server(void);

		void	init_bind(void);
		void	init_listen(void);


};

#endif
