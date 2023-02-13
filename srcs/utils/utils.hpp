/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/13 15:21:24 by hyap              #+#    #+#             */
/*   Updated: 2023/02/13 16:44:40 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
# define UTILS_H
# include <iostream>
# include <vector>
# include <map>


namespace utils {

const char	ws[6] = {32, 9, 10, 11, 12, 13};
typedef std::vector<std::string>::iterator	vecstr_iterator;
	
bool		is_whitespaces(char c);
bool		is_empty_string(const std::string& s);
std::string	get_nth_word(const std::string& s, int n);

}

#endif

