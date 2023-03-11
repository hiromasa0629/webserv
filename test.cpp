#include <iostream>
#include <fstream>
#include <vector>


int	main(void)
{
	std::ifstream	infile;
	std::ofstream	outfile;
	
	infile.open("text.txt");
	outfile.open("text.txt", std::ios::app | std::ios::out);
	
	outfile.write("wadafuk", 7);
	outfile.flush();
	
	char	buf[1024];
	std::memset(buf, 0, 1024);
	infile.seekg(-5,std::ios::end);
	infile.read(buf, 1024);
	std::cout << buf << std::endl;
	
	outfile.write("nivekkkkk", 7);
	outfile.flush();
	
	std::memset(buf, 0, 1024);
	infile.clear();
	infile.seekg(-5,std::ios::end);
	infile.read(buf, 1024);
	std::cout << buf << std::endl;
	
	outfile.close();
	infile.close();
	
	return (0);
}