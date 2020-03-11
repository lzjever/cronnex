#include <doctest/doctest.h>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <cstring>
#include "FileUtils.h"


 bool load_file_simple(const std::string file_name, std::string &file_content)
{
	file_content.clear();
	std::ifstream ifs(file_name, std::ifstream::in | std::ifstream::binary);
	if (ifs.is_open())
	{
	    // get length of file:
	    ifs.seekg (0, ifs.end);
	    int length = ifs.tellg();
	    ifs.seekg (0, ifs.beg);

		file_content.resize(length);
		ifs.seekg(0);
		ifs.read(&file_content[0], length);
		ifs.close();

		return true;
	}
	return false;
}

bool write_file_simple(const std::string file_name, const std::string file_content)
{
	std::ofstream ofs(file_name);
	if (ofs.is_open())
	{
		ofs.write (file_content.data(),file_content.size());
		ofs.close();
		return true;
	}
	return false;
}

//doctest
TEST_CASE("FileUtils.write_load_file_simple") 
{
	std::string file_content = "\x00\x01\x02";
}
