/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseForm.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/28 14:53:49 by hyap              #+#    #+#             */
/*   Updated: 2023/02/28 19:55:52 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSEFORM_H
# define RESPONSEFORM_H
# include <iostream>
# include "utils.hpp"

class ResponseFormField {
	public:
		ResponseFormField(void);
		ResponseFormField(utils::StrVec::iterator start, utils::StrVec::iterator end);
		~ResponseFormField(void);
		
		bool		get_is_file(void) const;
		std::string	get_value(void) const;
		std::string	get_name(void) const;
		std::string	get_filename(void) const;
		
	private:
		bool		_is_file;
		std::string	_value;
		std::string	_name;
		std::string	_filename;
};

class ResponseForm {
	public:
		ResponseForm(void);
		ResponseForm(const std::string& body, const std::string& boundary);
		~ResponseForm(void);
		
		std::vector<ResponseFormField>	get_fields(void) const;
		
	private:
		int								_status;
		utils::StrVec					_body;
		std::string						_boundary;
		std::string						_boundary_end;
		std::vector<ResponseFormField>	_fields;
		
		void	split_body(const std::string& body);
};

#endif
