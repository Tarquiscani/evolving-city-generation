#ifndef GM_TEST_LOGGER_H
#define GM_TEST_LOGGER_H


#include <fstream>
#include <filesystem>
#include <sstream>

#include "utilities/filesystem_utilities.hh"

#include "debug/logger/logger.h"


namespace tgm
{



class TestLogger
{
	public:
		TestLogger(std::string const& dir_name_, std::string const& file_name_) :
			dir_name(dir_name_), file_name(file_name_) { }

		TestLogger(TestLogger const&) = delete;
		TestLogger& operator=(TestLogger const&) = delete;

		void begin_report()
		{
			if (reporting)
				throw std::runtime_error("Cannot begin a new report if the previous one has not ended.");

			output = FsUtil::create_unique("_debug/testlogs/" + dir_name + "/" + file_name, true);
			if (!output)
				throw std::runtime_error("An error occurred when creating the report file.");

			logger = std::make_unique<Logger>(output);

			reporting = true;
		}

		void end_report()
		{
			if (!reporting)
				throw std::runtime_error("Cannot end a not begun report.");

			logger.reset();

			output.close();
			if(!output)
				throw std::runtime_error("An error occurred when closing the report file.");

			reporting = false;
		}

		template<typename T>
		void record_message(T const& message)
		{
			if(!reporting)
				throw std::runtime_error("Cannot record a message in a non-begun TestLogger.");
			if(!output)
				throw std::runtime_error("Cannot record a message: the stream is in an error state.");

			*logger << message;
		}

	private:
		bool reporting = false;
		std::ofstream output;
		std::unique_ptr<Logger> logger;

		std::string dir_name;
		std::string file_name;
};


template<typename T>
auto operator<<(TestLogger& tl, T const& message) -> TestLogger&
{
	tl.record_message(message);

	return tl;
}



}


#endif //GM_TEST_LOGGER_H