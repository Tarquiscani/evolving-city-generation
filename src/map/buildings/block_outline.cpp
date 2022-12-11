#include "block_outline.hh"


#include <sstream>
#include <stdexcept>


namespace tgm
{



void BlockOutline::assert_angleCount() const
{
    if (    convex_count - 4 != concave_count
        && concave_count - 4 != convex_count )
    {
        std::ostringstream oss; oss << "Unexpected situation. The angles should follow a special rule:"
                                    << "either the concave angles are 4 more than the convex angles (internal outline);"
                                    << "or the convex angles are 4 more than the concave angles (external oultine)."
                                    << "Tertium non datur.";
        throw std::runtime_error(oss.str());
    }
}



} //namespace tgm