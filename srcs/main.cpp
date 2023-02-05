/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 01:19:21 by hyap              #+#    #+#             */
/*   Updated: 2023/02/06 00:11:27 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int	main(void)
{
	Server server(AI_PASSIVE, AF_INET, SOCK_STREAM, 0);

	std::cout << server.get_addrinfo()->ai_flags << std::endl;
	std::cout << server.get_addrinfo()->ai_family << std::endl;
	std::cout << server.get_addrinfo()->ai_socktype << std::endl;
	std::cout << server.get_addrinfo()->ai_protocol << std::endl;
	std::cout << server.get_addrinfo()->ai_addrlen << std::endl;
	std::cout << server.get_addrinfo()->ai_addr->sa_family << std::endl;

	std::cout << server.get_addrinfo()->ai_addr->sa_data << std::endl;

	server.accept_connection();

	return (0);
}
