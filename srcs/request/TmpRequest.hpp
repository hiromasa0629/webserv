/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TmpRequest.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/03 17:53:38 by hyap              #+#    #+#             */
/*   Updated: 2023/03/11 18:41:19 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TMPREQUEST_H
# define TMPREQUEST_H
# include <iostream>
# include <map>
# include <iterator>
# include "utils.hpp"
# include <exception>
# include <sstream>
# include "Logger.hpp"
# include <algorithm>
# include "ServerErrorException.hpp"


# define CHUNKED_DEBUG 1000000
class TmpRequest {
	public:
		TmpRequest(void);
		~TmpRequest(void);
		TmpRequest(enum StatusCode status);
		
		std::string		get_request_field(enum RequestFields name) const;
		bool			get_is_complete(void) const;
		enum StatusCode	get_status_code(void) const;
		std::string		get_body(void) const;
		std::string		get_unchunked_filename(void) const;
		
		void		print_request_header(void) const;
		
		/**
		 * @brief append received bytes from recv() into _req.
		 * 
		 * @param req recv() received bytes
		 */
		void		append(const std::string& req);
		
	private:

		/**
		 * @brief if _req length is less than 4, throws exception. Check if header is terminated with crlf
		 * 
		 * @return true if above condition fulfilled
		 * @return false if one of the condition did not meet
		 */
		bool			check_header_complete(void);
		
		/**
		 * @brief Extract header info into _header_info
		 */
		void			extract_header_info(void);
		
		/**
		 * @brief Check request line if method, uri, protocol exsists and correct
		 * 
		 * @param vec first line of request header
		 * @return true if condition fulfilled
		 * @return false if failed one of the condition
		 */
		bool			check_request_line(const utils::StrVec& vec) const;
	
	
		/**
		 * @brief handle the value of content-type by determine chunked and multipart/form-data
		 * 
		 * @param val vector of value of content-type
		 */
		void			handle_content_type(const utils::StrVec& val);
		
		/**
		 * @brief handle GET request. Save query into _query
		 */
		void			handle_get(void);
		
		/**
		 * @brief handle POST request. Determine chunked or normal and save body into _body
		 * 
		 * @param req btyes received by recv()
		 */
		void			handle_post(const std::string& req);
		
		/**
		 * @brief read the chunked body and save them into _req
		 * 
		 * @return body without chunked header
		 */
		std::string			tidy_up_chunked_body(void);

		/**
		 * @brief indicates that this request is complete
		 */
		bool								_is_complete;
		
		/**
		 * @brief indicates that header is completely received
		 */
		bool								_is_complete_header;
		
		/**
		 * @brief consist of request heaeder and body
		 */
		std::string							_req;
		
		/**
		 * @brief final request body
		 */
		std::string							_body;
		
		/**
		 * @brief save untidy chunked body
		 */
		std::string							_chunked_body;
		
		/**
		 * @brief map of <request fields, value>
		 */
		std::map<enum RequestFields, std::string>	_header_info;
		
		/**
		 * @brief GET request qeury <name, value>
		 */
		std::map<std::string, std::string>	_query;
		
		enum StatusCode						_status_code;
		
		Logger								_logger;
		
		/**
		 * @brief static strings to validate request methods
		 */
		static const char*					_methods[];
		
		std::ofstream*						_chunked_outfile;
		std::ifstream*						_chunked_infile;
		std::string							_chunked_filename;
		std::string							_unchunked_filename;
		
		size_t								_chunked_debug_size;
		size_t								_debugged_index;
		
		/**
		 * @brief static strings to print out RequestField enum
		 */
		static const char*					_fields_string[];
};

#endif
