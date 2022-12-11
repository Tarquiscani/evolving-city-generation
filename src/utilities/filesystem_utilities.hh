#ifndef GM_FILESYSTEM_UTILITIES_HH
#define GM_FILESYSTEM_UTILITIES_HH


#include <iomanip>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <chrono>


#include "time_utilities.hh"


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

    inline auto create_unique(std::string const& pathstring, bool const prepend_datetime = false) -> std::ofstream
    {
        auto path = fss::path(pathstring, fss::path::generic_format);
        
        if (!path.has_filename()) { throw std::runtime_error("The path must have a filename."); }
        if (path.has_root_path()) { throw std::runtime_error("The path must be relative."); }


        fss::create_directories(path.relative_path().parent_path());

        auto unique_path = fss::path{};
        auto unique_path_found = false;
        for (int i = 0; !unique_path_found; ++i)
        {
            auto unique_index = std::ostringstream{};
            unique_index << std::setfill('_') << std::setw(6) << i << "_";

            auto unique_filename = unique_index.str() + path.stem().string() + path.extension().string();
            if (prepend_datetime)
            {
                unique_filename = Time::human_readable_datetime() + unique_filename;
            }

            unique_path.clear();
            unique_path = path.parent_path() / unique_filename;

            unique_path_found = !fss::exists(unique_path);
        }


        auto stream = std::ofstream{ unique_path, std::ios::trunc };
        if (!stream) { throw std::runtime_error("An error occurred when creating an ofstream."); }


        stream << "File creation time: " << Time::human_readable_datetime() << std::endl;


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