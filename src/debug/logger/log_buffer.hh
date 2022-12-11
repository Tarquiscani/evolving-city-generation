#ifndef GM_LOG_BUFFER_HH
#define GM_LOG_BUFFER_HH


#include <fstream>
#include <iostream>
#include <sstream>

// This file will be included in almost all headers. So keep its dependencies at the minimum to avoid circular dependencies.


namespace tgm
{



class LogBuffer : public std::stringbuf
{
    private:
        static inline auto & out_console_stream = std::cout;
        static auto create_out_file_stream() -> std::ofstream;
        static inline auto out_file_stream = create_out_file_stream();

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