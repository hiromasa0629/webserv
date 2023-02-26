/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseHeader.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/23 16:27:04 by hyap              #+#    #+#             */
/*   Updated: 2023/02/26 16:19:13 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSEHEADER_H
# define RESPONSEHEADER_H
# include <iostream>
# include <map>
# include <iterator>
# include <vector>
# include <sstream>
# include "utils.hpp"

class ResponseHeader {
	public:
		ResponseHeader(void);
		~ResponseHeader(void);

		void				set_status(int status);
		void				set_location(std::string location);
		void				set_content_length(int length);
		std::string			get_response_header(void) const;
		int					get_status(void) const;

		void				construct(void);

	private:
		std::map<int, std::string>	_codes;
		int							_status;
		std::string					_response_header;
		std::string					_location;
		int							_content_length;
};

#endif
