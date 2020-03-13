#pragma once
#include <string>

#if defined(__cplusplus) && __cplusplus >= 201703L && defined(__has_include)
#if __has_include(<filesystem>)
#define GHC_USE_STD_FS
#include <filesystem>
namespace fs {
	using namespace std::filesystem;
	using ifstream = std::ifstream;
	using ofstream = std::ofstream;
	using fstream = std::fstream;
}
#endif
#endif
#ifndef GHC_USE_STD_FS
#include <ghc/fs_fwd.hpp>
namespace fs {
	using namespace ghc::filesystem;
	using ifstream = ghc::filesystem::ifstream;
	using ofstream = ghc::filesystem::ofstream;
	using fstream = ghc::filesystem::fstream;
}
#endif


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


