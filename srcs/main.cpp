/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 01:19:21 by hyap              #+#    #+#             */
/*   Updated: 2023/02/04 01:43:02 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int	main(void)
{
	try
	{
		Socket	socket;
		std::cout << socket.get_addrinfo()->ai_flags << std::endl;
		std::cout << socket.get_addrinfo()->ai_family << std::endl;
		std::cout << socket.get_addrinfo()->ai_socktype << std::endl;
		std::cout << socket.get_addrinfo()->ai_protocol << std::endl;
		std::cout << socket.get_addrinfo()->ai_addrlen << std::endl;
		std::cout << socket.get_addrinfo()->ai_addr->sa_family << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}


	return (0);
}
