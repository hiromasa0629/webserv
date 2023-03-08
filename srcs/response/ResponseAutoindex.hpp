/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseAutoindex.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/26 21:12:56 by hyap              #+#    #+#             */
/*   Updated: 2023/03/08 18:36:46 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSEAUTOINDEX_H
# define RESPONSEAUTOINDEX_H
# include <iostream>
# include <sstream>
# include <dirent.h>
# include "ServerErrorException.hpp"

class ResponseAutoindex {
	public:
		ResponseAutoindex(void);
		~ResponseAutoindex(void);
		ResponseAutoindex(const std::string& path, const std::string& rest_of_the_uri, const std::string& host, const std::string& port);
		// ResponseAutoindex(const std::string& path, const std::string& rest_of_the_uri, const std::string& host, const std::string& port, const std::string& location_uri);
		
		std::string	get_body(void) const;

	private:
		void	construct_href(std::stringstream& ss, const char* dirname) const;
		void	construct_body(const std::string& s);
	
		std::string		_body;
		std::string		_path;
		// std::string		_rest_of_the_uri;
		std::string		_host;
		std::string		_port;
		// std::string		_location_uri;
};

#endif
