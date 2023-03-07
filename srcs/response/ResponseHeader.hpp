/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseHeader.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/23 16:27:04 by hyap              #+#    #+#             */
/*   Updated: 2023/03/07 13:42:28 by hyap             ###   ########.fr       */
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

		void				set_status(enum StatusCode status);
		void				set_location(std::string location);
		void				set_content_length(int length);
		void				set_is_chunked(bool is_chunked);
		void				set_content_type(std::string type);
		std::string			get_response_header(void) const;
		enum StatusCode		get_status(void) const;

		void				construct(void);

	private:
		std::map<int, std::string>	_codes;
		enum StatusCode				_status;
		std::string					_response_header;
		std::string					_location;
		int							_content_length;
		std::string					_content_type;
		bool						_is_chunked;
};

#endif
