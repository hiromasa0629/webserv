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
		close(pipe_to_parent[1]);
		
		write(pipe_to_child[1], "hello world\n", 13);
		
		char		buf[1024];
		ssize_t		ret;
		while ((ret = read(pipe_to_parent[0], buf, 1024)) > 0)
		{
			std::cout << "buff: " << std::endl;
			std::cout << buf << std::endl;
			std::cout << "in parent" << std::endl;
		}
		int status;
		waitpid(pid, &status, -1);
	}
	
	
	return (0);
}

// class MyClass {
// 	public:
// 		MyClass(void) : i(42) {}
// 		~MyClass(void) {}
// 		MyClass(const MyClass& src) { std::cout << "copied" << std::endl; }
	
// 		int	i;
// };

// void	main2(MyClass& c)
// {
// 	c.i = 41;
// }

// int	main(void)
// {
// 	MyClass	c;
	
// 	main2(c);
// 	std::cout << c.i << std::endl;
// 	return (0);
// }
