#include "visual_debug_highlighting.hh"


#if VISUALDEBUG


#include <iostream>


namespace tgm
{



auto operator<<(Logger & lgr, VisualDebugHighlighting const& vdh) -> Logger &
{
    vdh.print(lgr)
        << Logger::remt
        << Logger::nltb << "}";

    return lgr;
}



} // namespace tgm


#endif //VISUALDEBUG