#include <iostream>
#include <unistd.h>
#include <string>

int	main(int ac, char** av, char** envp)
{
	int	pipe_to_child[2];
	int	pipe_to_parent[2];
	
	if (pipe(pipe_to_child) < 0)
	{
		perror("pipe to child");
		return (1);
	}
	if (pipe(pipe_to_parent) < 0)
	{
		perror("pipe to parent");
		return (1);
	}
	
	int	pid;
	
	pid = fork();
	if (pid < 0)
	{
		perror("fork failed");
		return (1);
	}
	if (pid == 0)	// child process
	{
		close(pipe_to_child[1]);
		close(pipe_to_parent[0]);
		dup2(pipe_to_child[0], STDIN_FILENO);
		dup2(pipe_to_parent[1], STDOUT_FILENO);
		char*	argv[] = {"/usr/bin/ruby", "cgi.rb", NULL};
		execve("/usr/bin/ruby", argv, envp);
		perror("execve");
        exit(EXIT_FAILURE);
	}
	else	// parent process
	{
		close(pipe_to_child[0]);
		write(pipe_to_child[1], "hello world\n", 13);
		close(pipe_to_parent[1]);
		
		
		char		buf[1024];
		ssize_t		ret;
		std::cout << "read" << std::endl;
		while ((ret = read(pipe_to_parent[0], buf, 1024)) > 0)
			std::cout << buf << std::endl;
		std::cout << "after read" << std::endl;
		int status;
		waitpid(pid, &status, -1);
	}
	
	
	return (0);
}