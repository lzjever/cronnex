#include <functional>
#include <random>
#include <doctest/doctest.h>
#include <ghc/fs_impl.hpp>  // must before the fileutils.h
#include "FileUtils.h"


///////////////////////////////////////////////////////////////////////////////
//TemporaryDirectory
TemporaryDirectory::TemporaryDirectory(TempOpt opt)
{
    static auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    static auto rng = std::bind(std::uniform_int_distribution<int>(0, 35), std::mt19937(static_cast<unsigned int>(seed) ^ static_cast<unsigned int>(reinterpret_cast<ptrdiff_t>(&opt))));
    std::string filename;
    do {
        filename = "tmp_";
        for (int i = 0; i < 8; ++i) {
            filename += "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[rng()];
        }
        _path = fs::canonical(fs::temp_directory_path()) / filename;
    } while (fs::exists(_path));
    fs::create_directories(_path);
    if (opt == TempOpt::change_path) {
        _orig_dir = fs::current_path();
        fs::current_path(_path);
    }
}

TemporaryDirectory::~TemporaryDirectory()
{
    if (!_orig_dir.empty()) {
        fs::current_path(_orig_dir);
    }
    fs::remove_all(_path);
}

const fs::path& TemporaryDirectory::path() const
{ 
	return _path; 
}
//TemporaryDirectory
///////////////////////////////////////////////////////////////////////////////



 bool load_file_simple(const std::string file_name, std::string &file_content)
{
	file_content.clear();
	fs::ifstream ifs(file_name, fs::ifstream::in | fs::ifstream::binary);
	if (ifs.is_open())
	{
	    // get length of file:
	    ifs.seekg (0, ifs.end);
	    fs::ifstream::streamoff length = ifs.tellg();
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
	fs::ofstream ofs(file_name);
	if (ofs.is_open())
	{
		ofs.write (file_content.data(),file_content.size());
		ofs.close();
		return true;
	}
	return false;
}






///////////////////////////////////////////////////////////////////////////////
//doctest
TEST_SUITE("filesystem")
{
	TEST_CASE("tmpfile write/load")
	{
		TemporaryDirectory tmpdir;
		std::string data_write("\00\01\02\03\00", 5);
		bool bw = write_file_simple(tmpdir.path() / "tmp.data", data_write);
		CHECK(bw == true);

		std::string data_load;
		bool bl = load_file_simple(tmpdir.path() / "tmp.data", data_load);
		CHECK(bl == true);

		CHECK(data_load.size() == data_write.size());
		CHECK(data_load.size() == 5);
	}
}