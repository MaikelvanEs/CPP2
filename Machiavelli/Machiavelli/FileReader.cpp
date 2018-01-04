#include "FileReader.h"

FileReader::FileReader()
{
	
}

FileReader::~FileReader()
{
	
}

std::vector<std::string> FileReader::readFile(const std::string fileName) const
{
	std::vector<std::string> result;
	std::ifstream file (fileName);
	if (file.is_open())
	{
		std::string line;
		while (getline(file, line))
		{
			result.push_back(line);
		}
	}
	file.close();
	return result;
}
