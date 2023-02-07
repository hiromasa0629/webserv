/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 01:19:21 by hyap              #+#    #+#             */
/*   Updated: 2023/02/07 20:08:42 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int	main(void)
{
	Server server(AI_PASSIVE, AF_INET, SOCK_STREAM, 0, "localhost", "8080");

	// std::cout << "ai_flags: " << server.get_addrinfo()->ai_flags << std::endl;
	// std::cout << "ai_family: " << server.get_addrinfo()->ai_family << std::endl;
	// std::cout << "ai_socktype: " << server.get_addrinfo()->ai_socktype << std::endl;
	// std::cout << "ai_protocol: " << server.get_addrinfo()->ai_protocol << std::endl;
	// std::cout << "ai_addrlen: " << server.get_addrinfo()->ai_addrlen << std::endl;

	server.accept_connection();

	return (0);
}
