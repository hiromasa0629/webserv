/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseAutoindex.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/26 21:12:56 by hyap              #+#    #+#             */
/*   Updated: 2023/03/08 22:59:47 by hyap             ###   ########.fr       */
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
		ResponseAutoindex(const std::string& path, const std::string& uri, const std::string& host, const std::string& port);

		std::string	get_body(void) const;

	private:
		void	construct_href(std::stringstream& ss, const char* dirname) const;
		void	construct_body(const std::string& s);

		std::string		_body;
		std::string		_path;
		std::string		_host;
		std::string		_port;
		std::string		_uri;
};

#endif
