#pragma once
#include <string>
#include "config.h"


bool load_file_simple(const std::string file_name, std::string &file_content);
bool write_file_simple(const std::string file_name, const std::string file_content);