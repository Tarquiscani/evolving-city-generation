#ifndef GM_LOGGER_HH
#define GM_LOGGER_HH


#include <functional>
#include <ostream>
#include <string>
#include <type_traits>


namespace tgm
{



class Logger
{
    public:
        Logger(std::ostream & output_stream) :
            out(output_stream) { }

        Logger(Logger const&) = delete;
        Logger& operator=(Logger const&) = delete;

        template<typename T>
        void print_byVal(T message)
        {
            if (!out) { throw std::runtime_error("Cannot record a message: the stream is in an error state."); }

            out << message;
        }

        template<typename T>
        void print_byRef(T const& message)
        {
            if (!out) { throw std::runtime_error("Cannot record a message: the stream is in an error state."); }

            out << message;
        }

        using ostreamManipulator = std::ostream& (*)(std::ostream&);//std::add_pointer_t<std::ostream& (std::ostream&)>;
        void apply_ostreamManipulator(ostreamManipulator manip)
        {
            if (!out) { throw std::runtime_error("Cannot apply a manip: the stream is in an error state."); }

            manip(out);
        }


        static auto tabs(Logger& logger) -> Logger&
        {
            for (unsigned i = 0; i < logger.indent_count; ++i)
                logger.out << '\t';

            return logger;
        }
        ////
        //	Manipulator that both add a new line and the tabs.
        ////
        static auto nltb(Logger& logger) -> Logger&
        {
            logger.out << "\n";

            tabs(logger);

            return logger;
        }

        static auto addt(Logger& logger) -> Logger&
        {
            ++logger.indent_count;

            return logger;
        }
        static auto remt(Logger& logger) -> Logger&
        {
            if (logger.indent_count == 0)
                throw std::runtime_error("Removing an indentation from a Logger with zero indentations.");

            --logger.indent_count;

            return logger;
        }
        class add_title;

        using LoggerManipulator = auto (Logger&) -> Logger&;
        void apply_LoggerManipulator(std::function<LoggerManipulator> manip)
        {
            if (!out) { throw std::runtime_error("Cannot apply a manip: the stream is in an error state."); }

            manip(*this);
        }
    private:
        unsigned indent_count = 0;
        std::ostream & out;
};


////
//	Overload that manages non-array types.
////
template <typename T,
    typename = std::enable_if_t< !std::is_array_v<T> >,
    typename = std::enable_if_t< !std::is_convertible_v<T, std::function<Logger::LoggerManipulator>> >
>
auto operator<<(Logger& logger, const T& message) -> Logger&
{
    //g_log << " (non-array overload) ";

    logger.print_byRef(message);

    return logger;
}

////
//	Overload that manages string literals.
////
template<std::size_t S >
auto operator<<(Logger& logger, char const (&raw_array)[S]) -> Logger&
{
    //g_log << " (array overload) ";

    const char* pm = raw_array; //decay

    logger.print_byVal(pm);

    return logger;
}

////
//	Overload that manages other raw array types.
////
template<typename T, std::size_t S >
auto operator<<(Logger& logger, T(&raw_array)[S]) -> Logger&
{
    T* pm = raw_array; //decay

    logger.print_byVal("Unexpected non-char raw array.");

    return logger;
}

////
//	Overload that manages std::ostream manipulator types.
////
inline
auto operator<<(Logger& logger, Logger::ostreamManipulator ostream_manip) -> Logger&
{
    //g_log << " (ostreamManipulator overload) ";

    logger.apply_ostreamManipulator(ostream_manip);
    return logger;
}

////
//	Overload that manages LoggerManipulator types.
////
inline
auto operator<<(Logger& logger, std::function<Logger::LoggerManipulator> logger_manip) -> Logger&
{
    //g_log << " (LoggerManipulator overload) ";

    logger.apply_LoggerManipulator(logger_manip);

    return logger;
}




class Logger::add_title 
{
    public:
        add_title(std::string const& title) : _title(title) {}

        auto operator()(Logger& logger) -> Logger&
        {
            logger << Logger::nltb << "////";

            for (decltype(_title)::size_type i = 0; i < _title.size(); ++i)
                logger << '/';

            logger << "////"
                << Logger::nltb << "//  " << _title << "  //"
                << Logger::nltb << "////";

            for (decltype(_title)::size_type i = 0; i < _title.size(); ++i)
                logger << '/';

            logger << "////";

            return logger;
        }

    private:
        std::string _title;
};



} // namespace tgm


#endif //GM_LOGGER_HH