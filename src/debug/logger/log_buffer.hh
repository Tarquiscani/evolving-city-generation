#ifndef GM_LOG_BUFFER_HH
#define GM_LOG_BUFFER_HH


#include <fstream>
#include <iostream>
#include <sstream>

#include "utilities/filesystem_utilities.hh"


namespace tgm
{



class LogBuffer : public std::stringbuf
{
    private:
        static inline auto & out_console_stream = std::cout;
        static inline auto out_file_stream = FsUtil::create_unique("_logs/log.txt", true);

    public:
        virtual int sync() override
        {
            out_console_stream << this->str();
            out_file_stream << this->str(); 
            out_file_stream.flush();

            // Clear the buffer
            this->str({});

            return 0;
        }
};

inline auto g_log_buffer = LogBuffer{};



} // namespace tgm


#endif //GM_LOG_BUFFER_HH