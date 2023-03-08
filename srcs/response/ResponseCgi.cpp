/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseCgi.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyap <hyap@student.42kl.edu.my>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/07 20:20:43 by hyap              #+#    #+#             */
/*   Updated: 2023/03/08 23:39:49 by hyap             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseCgi.hpp"

ResponseCgi::ResponseCgi(void) {}

ResponseCgi::~ResponseCgi(void)
{
	for (size_t i = 0; i < this->_envp.size(); i++)
		delete[] this->_envp[i];
}

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

void	ResponseCgi::set_envp(const std::string& key, const std::string& value)
{
	std::stringstream	ss;
	std::string			s;

	ss << key << "=" << value;
	ss >> s;
	this->_envp.insert(this->_envp.end() - 1, const_cast<char*>(s.c_str()));
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

		execve(this->_cmd.c_str(), argv.data(), this->_envp.data());

		std::perror("Execve failed");
		std::exit(EXIT_FAILURE);
	}
	else
	{
		close(pipe_to_child[0]);
		close(pipe_to_parent[1]);

		write(pipe_to_child[1], this->_body.c_str(), this->_body.size());

		char		buf[READ_BUFFER];
		ssize_t		ret;
		while ((ret = read(pipe_to_parent[0], buf, READ_BUFFER)) > 0)
			this->_response_msg.append(buf);

		int status;
		waitpid(pid, &status, -1);

		if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
			throw ServerErrorException(__LINE__, __FILE__, E500, "Child failed");
	}
}

const std::string&	ResponseCgi::get_response_msg(void) const
{
	return (this->_response_msg);
}
