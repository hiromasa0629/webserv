/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 01:19:21 by hyap              #+#    #+#             */
/*   Updated: 2023/02/11 16:05:46 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int	main(int ac, char** argv)
{
	if (ac != 2)
	{
		std::cout << "Invalid arguments count" << std::endl;
		return (0);
	}
	Server server(AI_PASSIVE, AF_INET, SOCK_STREAM, 0, "localhost", "8080", argv[1]);
	
	server.run();

	return (0);
}
