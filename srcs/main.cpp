/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/04 01:19:21 by hyap              #+#    #+#             */
/*   Updated: 2023/03/07 20:30:45 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Config.hpp"

int	main(int ac, char** argv, char** envp)
{
	if (ac != 2)
	{
		std::cout << "Invalid arguments count" << std::endl;
		return (0);
	}
	Config	config(argv[1]);

	config.print_config();

	Server server(AI_PASSIVE, AF_INET, SOCK_STREAM, 0, config, envp);
	server.run();
	return (0);
}
