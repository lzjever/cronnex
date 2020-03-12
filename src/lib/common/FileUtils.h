#pragma once
#include <string>

bool load_file_simple(const std::string file_name, std::string &file_content);
bool write_file_simple(const std::string file_name, const std::string file_content);