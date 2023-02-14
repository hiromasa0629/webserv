/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 01:19:21 by hyap              #+#    #+#             */
/*   Updated: 2023/02/13 23:46:36 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Config.hpp"

int	main(int ac, char** argv)
{
	if (ac != 2)
	{
		std::cout << "Invalid arguments count" << std::endl;
		return (0);
	}
	// server.run();

	Config	config(argv[1]);

	config.print_config();

	Server server(AI_PASSIVE, AF_INET, SOCK_STREAM, 0, "localhost", "8080", argv[1]);

	return (0);
}
