/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/15 21:24:29 by hyap              #+#    #+#             */
/*   Updated: 2023/02/27 20:34:18 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_H
# define REQUEST_H
# include "utils.hpp"
# include "Logger.hpp"

class Request {
	public:
		Request(void);
		~Request(void);
		Request(const std::string& req);
		
		bool	get_is_complete(void) const;
		void	print_request_header(void) const;
		void	append(const std::string& req);
		
		std::string					get_method(void) const;
		std::string					get_uri(void) const;
		std::string					get_host(void) const;
		std::string					get_port(void) const;
		std::string					get_body(void) const;
		int							get_body_size(void) const;
		bool						get_is_empty_request(void) const;
		
	private:
		void						check_full_request_header(void);
		bool						is_crlf(std::string::iterator start, std::string::iterator end) const;
		void						extract_header_n_body(void);
		std::vector<std::string>	save_header(std::string::iterator start, std::string::iterator end);
		std::string					save_body(std::string::iterator start, std::string::iterator end);
		void						extract_header_info(void);
		
		std::string					_req;
		utils::StrVec				_header;
		std::string					_body;
		bool						_is_complete;
		std::string					_method;
		std::string					_uri;
		std::string					_host;
		std::string					_port;
		int							_content_length;
		
		Logger						_logger;
		bool						_is_empty_request;
};


#endif
