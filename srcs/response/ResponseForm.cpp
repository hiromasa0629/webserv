/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseForm.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/28 14:53:52 by hyap              #+#    #+#             */
/*   Updated: 2023/02/28 20:11:13 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseForm.hpp"

ResponseForm::ResponseForm(void) {}

ResponseForm::~ResponseForm(void) {}

ResponseForm::ResponseForm(const std::string& body, const std::string& boundary)
{
	this->_status		= 200;
	this->_boundary		= "--" + boundary;
	this->_boundary_end	= "--" + boundary + "--";
	
	// std::cout << body << std::endl;
	this->split_body(body);
	std::cout << "body_size: " << this->_body.size() << std::endl;
	std::cout << std::endl;
	for (size_t i = 0; i < this->_body.size(); i++)
		std::cout << "size: " << this->_body[i].size() << " " << this->_body[i] << std::endl;
	std::cout << std::endl;

	utils::StrVec::iterator	it;
	utils::StrVec::iterator	start;
	
	it = this->_body.begin();
	if (*it == this->_boundary)
		it++;
	start = it;
	for (; it < this->_body.end(); it++)
	{
		if (*it == this->_boundary || *it == this->_boundary_end)
		{
			this->_fields.push_back(ResponseFormField(start, it));
			start = it + 1;
		}
	}
	
	std::cout << this->_fields.size() << std::endl;
	
	for (size_t i = 0; i < this->_fields.size(); i++)
	{
		std::cout << "is_file: " << this->_fields[i].get_is_file() << std::endl;
		std::cout << "value: " << this->_fields[i].get_value() << std::endl;
		std::cout << "name: " << this->_fields[i].get_name() << std::endl;
		std::cout << "filename: " << this->_fields[i].get_filename() << std::endl;
	}
	
}

void	ResponseForm::split_body(const std::string& body)
{
	std::string::const_iterator	it;
	std::string					tmp;
	
	it = body.begin();
	for (; it != body.end(); it++)
	{
		if (*it == '\r' && (it + 1) != body.end() && *(it + 1) == '\n')
		{
			it++;
			if (!tmp.empty())
				this->_body.push_back(tmp);
			tmp.clear();
		}
		else
			tmp.push_back(*it);
	}
	if (!tmp.empty())
		this->_body.push_back(tmp);
}

std::vector<ResponseFormField>	ResponseForm::get_fields(void) const
{
	return (this->_fields);
}

ResponseFormField::ResponseFormField(void) {}

ResponseFormField::~ResponseFormField(void) {}

ResponseFormField::ResponseFormField(utils::StrVec::iterator start, utils::StrVec::iterator end)
{
	this->_is_file	= false;
	this->_value	= "";
	this->_name		= "";
	this->_filename	= "";
	
	utils::StrVec	split;
	
	for (; start < end; start++)
	{
		split = utils::ft_split(*start);
		if (split[0] == "Content-Disposition:")
		{
			for (size_t i = 1; i < split.size(); i++)
			{
				if (split[i][split[i].length() - 1] == ';')
					split[i].erase(split[i].end() - 1);
				if (!std::strncmp(split[i].c_str(), "name=", 5))
					this->_name = std::string(split[i].begin() + 6, split[i].end() - 1);
				else if (!std::strncmp(split[i].c_str(), "filename=", 9))
				{
					this->_filename = std::string(split[i].begin() + 10, split[i].end() - 1);
					this->_is_file = true;
				}
			}
		}
		else if (split[0] == "Content-Type:")
			continue ;
		else
		{
			for (size_t i = 0; i < (*start).size(); i++)
				this->_value.push_back((*start)[i]);
		}
	}
}

bool		ResponseFormField::get_is_file(void) const
{
	return (this->_is_file);
}

std::string	ResponseFormField::get_value(void) const
{
	return (this->_value);
}

std::string	ResponseFormField::get_name(void) const
{
	return (this->_name);
}

std::string	ResponseFormField::get_filename(void) const
{
	return (this->_filename);
}
