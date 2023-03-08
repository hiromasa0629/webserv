#include <iostream>
#include <fstream>
#include <vector>

static	std::vector<std::string>	split_uri(std::string uri)
{
	std::vector<std::string>	sv;
	
	while (uri.size() > 0)
	{
		std::string	tmp		= std::string(uri.begin() + 1, uri.end());
		size_t		index	= tmp.find_first_of('/');
		
		if (index == std::string::npos)
		{
			sv.push_back(uri.substr(0, index));
			break ;
		}
		else
			sv.push_back(uri.substr(0, index + 1));
		uri = uri.substr(index + 1);
	}
	return (sv);
}

int	main(void)
{
	std::string s = "/";
	
	std::vector<std::string>	sv = split_uri(s);
	for (size_t i = 0; i < sv.size(); i++)
		std::cout << sv[i] << std::endl;
	return (0);
}