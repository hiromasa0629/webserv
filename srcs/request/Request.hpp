/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/15 21:24:29 by hyap              #+#    #+#             */
/*   Updated: 2023/02/16 17:47:25 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_H
# define REQUEST_H
# include "Server.hpp"
# include "utils.hpp"

enum Methods {
	GET,
	POST,
	PUT,
	DELETE
};

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
		
		utils::CharVec				_req;
		std::vector<utils::CharVec>	_header;
		std::vector<utils::CharVec>	_body;
		bool						_is_complete;
		Methods						_method;
		std::string					_path;
		std::string					_host;
};


#endif
