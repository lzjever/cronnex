#pragma once
#include <string>

#include <ghc/fs_fwd.hpp>
namespace fs {
	using namespace ghc::filesystem;
	using ifstream = ghc::filesystem::ifstream;
	using ofstream = ghc::filesystem::ofstream;
	using fstream = ghc::filesystem::fstream;
}


class TemporaryDirectory
{
public:
    enum class TempOpt { none, change_path };
    TemporaryDirectory(TempOpt opt = TempOpt::none);
	~TemporaryDirectory();
	const fs::path& path() const;

private:
    fs::path _path;
    fs::path _orig_dir;
};


//utility functions
bool load_file_simple(const std::string file_name, std::string &file_content);
bool write_file_simple(const std::string file_name, const std::string file_content);


