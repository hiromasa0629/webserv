/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 00:25:47 by hyap              #+#    #+#             */
/*   Updated: 2023/02/04 15:59:00 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_H
# define SERVER_H
# include <iostream>
# include "Socket.hpp"

class Server : public Socket {
	public:
		Server(void);
		~Server(void);
		Server(const Server &src);
		Server	&operator=(const Server &rhs);
		
		

	private:
};

#endif
