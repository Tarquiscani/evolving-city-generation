#ifndef GM_FILESYSTEM_UTILITIES_HH
#define GM_FILESYSTEM_UTILITIES_HH


#include <iomanip>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <chrono>


namespace tgm
{



namespace FsUtil
{
	namespace fss = std::filesystem;

	inline auto create_overwriting(std::string const& pathstring) -> std::ofstream
	{
		fss::path path(pathstring, fss::path::generic_format);

		if (!path.has_filename()) { throw std::runtime_error("The path must have a filename."); }
		if (path.has_root_path()) { throw std::runtime_error("The path must be relative."); }


		fss::create_directories(path.relative_path().parent_path());


		if (fss::exists(path))
		{
			if (fss::is_regular_file(path)) 
			{
				if (!fss::remove(path)) { throw std::runtime_error("An error occurred while deleting a file."); }
			}
			else
			{
				throw std::runtime_error("A file already exists and it's not a regular file.");
			}
		}
		

		auto ofstream = std::ofstream{ path, std::ios::trunc };
		if (!ofstream) { throw std::runtime_error("An error occurred when creating the ofstream."); }


		return ofstream;
	}

	inline auto create_without_overwriting(std::string const& pathstring) -> std::ofstream
	{
		fss::path path(pathstring, fss::path::generic_format);
		
		if (!path.has_filename()) { throw std::runtime_error("The path must have a filename."); }
		if (path.has_root_path()) { throw std::runtime_error("The path must be relative."); }


		fss::create_directories(path.relative_path().parent_path());


		fss::path fixed_path;
		for (int i = 0; ; ++i)
		{
			std::ostringstream i_oss;
			i_oss << std::setfill('_') << std::setw(6) << i;

			fixed_path.clear();
			fixed_path = (path.parent_path() / path.stem() += i_oss.str()) += path.extension();

			if (!fss::exists(fixed_path))
				break;
		}


		std::ofstream stream(fixed_path, std::ios::trunc);
		if (!stream)
			throw std::runtime_error("An error occurred when creating an ofstream.");


		// Print the current datetime
		auto now = std::chrono::system_clock::now();
		auto in_time_t = std::chrono::system_clock::to_time_t(now);
		stream << "Unix epoch time: " << in_time_t << std::endl; //TODO: Replace with human readable datetime


		return stream;
	}

	
	inline auto open(std::string const& pathstring) -> std::ifstream
	{
		fss::path path(pathstring, fss::path::generic_format);

		if (!path.has_filename()) { throw std::runtime_error("The path must have a filename."); }
		if (path.has_root_path()) { throw std::runtime_error("The path must be relative."); }


		if (fss::exists(path))
		{
			if (!fss::is_regular_file(path)) { throw std::runtime_error("The file isn't regular."); }
		}
		

		auto ifstream = std::ifstream{ path, std::ios::in };
		if (!ifstream.is_open()) { throw std::runtime_error("An error occurred when opening the ifstream."); }


		return ifstream;
	}

	
	inline auto get_directoryFilenames(std::string const& pathstring) -> std::vector<std::string>
	{
		auto filenames = std::vector<std::string>{};

		auto path = fss::path{ pathstring, fss::path::generic_format };

		if (path.has_filename()) { throw std::runtime_error("The path must be a directory."); }
		if (path.has_root_path()) { throw std::runtime_error("The path must be relative."); }

		fss::create_directories(path.relative_path().parent_path());

		if (fss::is_directory(path))
		{
			for (auto const& de : fss::directory_iterator(path))
			{
				if (de.is_regular_file()) { filenames.push_back(de.path().filename().string()); }
			}
		}

		return filenames;
	}

} //namespace FsUtil



} //namespace tgm


#endif //GM_FILESYSTEM_UTILITIES_HH