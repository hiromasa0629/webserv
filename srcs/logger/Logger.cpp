/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/15 15:50:14 by hyap              #+#    #+#             */
/*   Updated: 2023/02/15 17:27:50 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"

Logger::Logger(std::ostream *default_stream) : _log_path(), _log_stream(default_stream), _ext(false), _is_default(true){
	if (_log_stream != &std::cout && _log_stream != &std::cerr)
		_ext = true;
	if (_log_stream != &std::cout)
		this->_is_default = false;
}

Logger::~Logger(){
	if (_log_stream != &std::cout && _log_stream != &std::cerr && _ext == false)
		delete _log_stream;
}

Logger::Logger(const Logger &ref) : _log_path(), _log_stream(&std::cout), _ext(false){
	*this = ref;
}

Logger &Logger::operator=(const Logger &ref) {
	std::ofstream	*temp;

	if (ref._log_stream == &std::cout || ref._log_stream == &std::cerr) {
		this->_log_stream = ref._log_stream;
	}
	else {
		this->_log_path = ref._log_path;
		temp = new std::ofstream(ref._log_path);
		if (temp->is_open() == false){
			this->warn("Unable to set log path. Defaulting to default stream");
			return *this;
		}
		this->_log_stream = temp;
		this->_ext = false;
		this->_is_default = ref._is_default;
	}
	return *this;
}

Logger::Logger(const std::string &path, std::ostream *default_stream) : _log_path(path), _log_stream(default_stream), _ext(false), _is_default(true) {
	std::ofstream	*temp;

	if (_log_stream != &std::cout)
		this->_is_default = false;
	if (_log_stream != &std::cout && _log_stream != &std::cerr)
		_ext = true;
	if (_log_path.empty() == true){
		this->warn("Path to log is empty. Defaulting to default stream");
		return;
	}
	temp = new std::ofstream(path);
	if (temp->is_open() == false){
		this->warn("Unable to set log path. Defaulting to default stream");
		return;
	}
	this->_log_stream = temp;
}

bool Logger::isDefaultStream() const {
	return this->_is_default;
}
