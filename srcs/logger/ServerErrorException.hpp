/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerErrorException.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/05 19:20:40 by hyap              #+#    #+#             */
/*   Updated: 2023/03/05 19:44:48 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERERROREXCEPTION_H
# define SERVERERROREXCEPTION_H
# include <iostream>
# include <exception>
# include "utils.hpp"

class ServerErrorException : public std::exception {
	public:
		ServerErrorException(int line, const char* file, enum StatusCode _status, const std::string& msg);
		virtual ~ServerErrorException(void) throw();
		virtual const char*	what(void) const throw();
		enum StatusCode		get_status(void) const throw();
	private:
		int				_line;
		enum StatusCode	_status;
		std::string		_msg;
};

#endif
