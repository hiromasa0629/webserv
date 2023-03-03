/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TmpRequest.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/03 17:53:38 by hyap              #+#    #+#             */
/*   Updated: 2023/03/03 22:50:22 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TMPREQUEST_H
# define TMPREQUEST_H
# include <iostream>
# include <map>
# include <iterator>
# include "utils.hpp"

class TmpRequest {
	public:
		TmpRequest(void);
		~TmpRequest(void);
		
		std::string	get_req(void) const;
		bool		get_is_complete(void) const;
		
		void		append(const std::string& req);
		
	private:
		bool			check_header_complete(void);
		void			extract_header_info(void);
	
		bool								_is_complete;
		std::string							_req;
		std::string							_header;
		std::map<std::string, std::string>	_header_info;
		
		static const char					_methods[];
};

#endif