/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/13 15:21:24 by hyap              #+#    #+#             */
/*   Updated: 2023/03/05 19:23:00 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
# define UTILS_H
# include <iostream>
# include <vector>
# include <map>
# include <sstream>
# include <sys/poll.h>

# define BUFFER_SIZE	100
# define DEBUG 1

enum RequestFields {
	METHOD,
	URI,
	SERVER_NAME,
	PORT,
	PROTOCOL,
	CONTENT_LENGTH,
	CONTENT_TYPE,
	TRANSFER_ENCODING,
	BOUNDARY
};

enum StatusCode {
	E400	= 400,	// Bad Request
	E404	= 404,	// Not found
	E405	= 405,	// Method Not Allowed
	E413	= 413,	// Body Size Too Large
	E500	= 500,	// Internal Server Error
	S200	= 200,
	E0		= 0		//	Empty request
};

namespace utils {

const char	ws[] = " \t\n\v\f\r";
const std::string	default_server_directives[] = {"listen",
													"root",
													"error_page",
													"index",
													"server_name",
													"cgi",
													"upload"};

const std::string	default_location_directives[] = {"return",
														"root",
														"autoindex",
														"limit_except",
														"index",
														"client_max_body_size",
														"cgi",
														"upload"};

typedef std::vector<char>						CharVec;
typedef std::vector<std::string>				StrVec;
typedef std::map< std::string, StrVec >			StrToStrVecMap;
typedef std::pair< std::string, StrVec >		StrToStrVecPair;

bool									is_whitespaces(char c);
bool									is_empty_string(const std::string& s);
StrVec									ft_split(const std::string& s);
bool									is_valid_server_directives(std::string s);
bool									is_valid_location_directives(std::string s);
void									charvec_shrink_to_fit(CharVec& cv);

void									print_msg_with_crlf(const std::string& s);

}

bool			operator==(const struct pollfd& lhs, const struct pollfd& rhs);
std::ostream&	operator<<(std::ostream& o, const utils::CharVec cv);

#endif

