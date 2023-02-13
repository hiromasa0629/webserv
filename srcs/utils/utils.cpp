/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/13 15:20:33 by hyap              #+#    #+#             */
/*   Updated: 2023/02/13 16:45:12 by hyap             ###   ########.fr       */
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

std::string	get_nth_word(const std::string& s, int n)
{
	size_t	start;
	size_t	end;
	size_t	i;
	int		count;
	
	start = 0;
	end = 0;
	count = 0;
	i = 0;
	while (i < s.length())
	{
		if (is_whitespaces(s[i]))
			i++;
		else if (!is_whitespaces(s[i]))
		{
			start = i;
			while (!is_whitespaces(s[i]))
				i++;
			end = i + 1;
			count++;
		}
		if (count == n)
			break ;
	}
	return (s.substr(start, end));
}

}
