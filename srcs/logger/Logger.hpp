/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/15 15:48:09 by hyap              #+#    #+#             */
/*   Updated: 2023/02/16 16:19:47 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include <string>
#include <fstream>
#include <iostream>
#include <type_traits>
#include "colors.hpp"

class Logger{
	public:
		explicit Logger(std::ostream *default_stream = &std::cout);
		~Logger();
		Logger(const std::string &path, std::ostream *default_stream = &std::cout);

		template < class T >
		void warn(const T& msg) const {
			(*_log_stream) << BOLD << YELLOW << "[Warning] " << RESET << msg << std::endl;
		}

		template < class T > 
		void error(const T& msg) const {
			if (_log_stream == &std::cout)
				std::cerr << "[Error] " << msg << std::endl;
			else
				(*_log_stream) << BOLD << RED <<  "[Error] " << RESET << msg << std::endl;
		}

		template < class T > 
		void info(const T& msg) const {
			(*_log_stream) << BOLD << BLUE << "[Info] " << RESET << msg << std::endl;
		}

		template < class T > 
		void debug(const T& msg) const {
			(*_log_stream) << BOLD << GREEN << "[Debug] " << RESET << msg << std::endl;
		}

		bool isDefaultStream() const;
		
		void listening(void) const;

	private:
		std::string		_log_path;
		std::ostream	*_log_stream;
		bool			_ext;
		bool			_is_default;

		Logger(const Logger &ref);
		Logger &operator=(const Logger &ref);
};

#endif
