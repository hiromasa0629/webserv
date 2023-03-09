/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseCgi.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/07 20:20:43 by hyap              #+#    #+#             */
/*   Updated: 2023/03/09 22:04:54 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseCgi.hpp"

ResponseCgi::ResponseCgi(void) {}

ResponseCgi::~ResponseCgi(void) {}

ResponseCgi::ResponseCgi(const std::string& path, const std::string& body, const std::string& cmd, char** envp)
	: _path(path), _envp(envp), _body(body), _cmd(cmd)
{

}

void	ResponseCgi::set_envp(const std::string& key, const std::string& value)
{
	std::stringstream	ss;

	ss << key << "=" << value;
	this->_add_envp.push_back(ss.str());
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
		this->child_process(pipe_to_child, pipe_to_parent);
	}
	else
	{
		close(pipe_to_child[0]);
		close(pipe_to_parent[1]);

		write(pipe_to_child[1], this->_body.c_str(), this->_body.size());
		close(pipe_to_child[1]);

		char		buf[READ_BUFFER];
		ssize_t		ret;
		while ((ret = read(pipe_to_parent[0], buf, READ_BUFFER)) > 0)
			this->_response_msg.append(buf);

		int status;
		if (waitpid(pid, &status, 0) == -1)
			throw ServerErrorException(__LINE__, __FILE__, E500, "waitpid() failed");
	}
}

static	std::vector<char*>	construct_envp(char** envp, utils::StrVec add_envp)
{
	std::vector<char*>								sv;
	
	for (size_t i = 0; envp[i] != NULL; i++)
	{
		char*	s = new char [std::strlen(envp[i]) + 1];
		std::strcpy(s, envp[i]);
		sv.push_back(s);
	}
	for (size_t i = 0; i < add_envp.size(); i++)
	{
		char*	s = new char [add_envp[i].length() + 1];
		std::strcpy(s, add_envp[i].c_str());
		sv.push_back(s);
	}
	sv.push_back(NULL);
	return (sv);
}

void	ResponseCgi::child_process(int* pipe_to_child, int* pipe_to_parent)
{
	std::vector<std::string>	tmp;
	std::vector<char*>			envp;

	tmp.push_back(this->_cmd);
	tmp.push_back(this->_path);

	std::vector<char*>			argv;
	for (size_t i = 0; i < tmp.size(); i++)
		argv.push_back(const_cast<char*>(tmp[i].c_str()));
	argv.push_back(NULL);
	
	envp = construct_envp(this->_envp, this->_add_envp);
	for (size_t i = 0; i < envp.size(); i++)
	{
		if (envp[i] == NULL)
			break ;
		std::cerr << envp[i] << std::endl;
	}

	close(pipe_to_child[1]);
	close(pipe_to_parent[0]);
	dup2(pipe_to_child[0], STDIN_FILENO);
	dup2(pipe_to_parent[1], STDOUT_FILENO);

	execve(this->_cmd.c_str(), argv.data(), envp.data());
	for (size_t i = 0; i < envp.size(); i++)
		delete[] envp[i];
	std::perror("Execve failed");
	std::exit(EXIT_FAILURE);
}

const std::string&	ResponseCgi::get_response_msg(void) const
{
	return (this->_response_msg);
}
