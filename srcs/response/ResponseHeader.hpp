/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseHeader.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/23 16:27:04 by hyap              #+#    #+#             */
/*   Updated: 2023/02/24 23:33:39 by hyap             ###   ########.fr       */
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
		void				construct(void);
		utils::CharVec		get_responds_header(void) const;
		void				set_content_length(int length);

	private:
		std::map<int, std::string>	_codes;
		int							_status;
		utils::CharVec				_responds_header;
		std::string					_location;
		int							_content_length;
};

#endif
