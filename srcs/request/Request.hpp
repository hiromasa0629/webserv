/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/15 21:24:29 by hyap              #+#    #+#             */
/*   Updated: 2023/02/16 20:26:58 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_H
# define REQUEST_H
# include "Server.hpp"
# include "utils.hpp"

class Request {
	public:
		Request(void);
		~Request(void);
		Request(const utils::CharVec& req);
		
		bool	get_is_complete(void) const;
		void	print_request_header(void) const;
		void	append(const utils::CharVec& req);
		
	private:
		void						check_full_request_header(void);
		bool						is_crlf(utils::CharVec::iterator start, utils::CharVec::iterator end) const;
		void						extract_header_n_body(void);
		std::vector<utils::CharVec>	save_header_n_body(utils::CharVec::iterator start, utils::CharVec::iterator end);
		void						extract_header_info(void);
		
		utils::CharVec				_req;
		std::vector<utils::CharVec>	_header;
		std::vector<utils::CharVec>	_body;
		bool						_is_complete;
		std::string					_method;
		std::string					_uri;
		std::string					_host;
		
		Logger						_logger;
};


#endif
