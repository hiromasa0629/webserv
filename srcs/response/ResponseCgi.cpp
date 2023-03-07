/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseCgi.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/07 20:20:43 by hyap              #+#    #+#             */
/*   Updated: 2023/03/07 21:07:33 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseCgi.hpp"

ResponseCgi::ResponseCgi(void) {}

ResponseCgi::~ResponseCgi(void) {}

ResponseCgi::ResponseCgi(const std::string& path, const std::string& body, const std::string& cmd, char** envp) 
	: _path(path), _body(body), _cmd(cmd)
{
	for (size_t i = 0; envp[i] != NULL; i++)
	{
		char*	s = new char[std::strlen(envp[i])];
		this->_envp.push_back(s);
	}
	this->_envp.push_back(NULL);
}

void	ResponseCgi::execute(void)
{
	int	pipe_to_child[2];
	int	pipe_to_parent[2];
	
	if (pipe(pipe_to_child) < 0)
		throw ServerErrorException(__LINE__, __FILE__, E500, "Pipe error");
	if (pipe(pipe_to_parent) < 0)
		throw ServerErrorException(__LINE__, __FILE__, E500, "Pipe error");
	
	int	pid;
	
	pid = fork();
	if (pid < 0)
		throw ServerErrorException(__LINE__, __FILE__, E500, "Fork error");
	if (pid == 0)
	{
		std::vector<std::string>	tmp;
		tmp.push_back(this->_cmd);
		tmp.push_back(this->_path);
		tmp.push_back(NULL);
		
		std::vector<char*>			argv;
		for (size_t i = 0; i < tmp.size(); i++)
			argv.push_back(const_cast<char*>(tmp[i].c_str()));
		close(pipe_to_child[1]);
		close(pipe_to_parent[0]);
		dup2(pipe_to_child[0], STDIN_FILENO);
		dup2(pipe_to_parent[1], STDOUT_FILENO);
		
		
	}
}
