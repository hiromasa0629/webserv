/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/13 15:20:33 by hyap              #+#    #+#             */
/*   Updated: 2023/02/16 21:16:37 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "utils.hpp"

namespace utils {
	
bool	is_whitespaces(char c)
{
	for (int i = 0; i < 6; i++)
		if (c == ws[i])
			return (true);
	return (false);
}

bool	is_empty_string(const std::string& s)
{
	for (size_t i = 0; i < s.length(); i++)
		if (!is_whitespaces(s[i]))
			return (false);
	return (true);
}

StrVec	ft_split(const std::string& s)
{
	std::stringstream			ss(s);
	std::string					word;
	StrVec						res;
	
	while (ss >> word)
		res.push_back(word);
	return (res);
}

bool	is_valid_server_directives(std::string s)
{
	size_t				size;
	size_t				i;
	
	size = sizeof(default_server_directives)/sizeof(default_server_directives[0]);
	i = 0;
	while (i < size)
		if (s == default_server_directives[i++])
			return (true);
	return (false);
}

bool	is_valid_location_directives(std::string s)
{
	size_t				size;
	size_t				i;
	
	size = sizeof(default_location_directives)/sizeof(default_location_directives[0]);
	i = 0;
	while (i < size)
		if (s == default_location_directives[i++])
			return (true);
	return (false);
}

}
