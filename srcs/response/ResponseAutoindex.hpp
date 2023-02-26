/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseAutoindex.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/26 21:12:56 by hyap              #+#    #+#             */
/*   Updated: 2023/02/26 21:47:10 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSEAUTOINDEX_H
# define RESPONSEAUTOINDEX_H
# include <iostream>
# include <sstream>

class ResponseAutoindex {
	public:
		ResponseAutoindex(void);
		~ResponseAutoindex(void);
		ResponseAutoindex(const std::string& path);

	private:
		std::string	_body;
};

#endif